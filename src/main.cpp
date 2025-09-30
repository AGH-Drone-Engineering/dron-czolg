
#include "mpu6050_sensor.h"

extern "C" {
#include "main.h"
#include "motor.h"
#include "pid.h"
#include "sbus_receiver.h"
}

Mpu6050Sensor mpuSensor;
sensors_event_t a, g, temp;

// PID
static pid3d_s pid_inner, pid_outer;
static float pid_yaw_ctrl, pid_roll_ctrl, pid_pitch_ctrl;
static float yaw_sp, roll_sp, pitch_sp, throttle_sp, roll_desired,
    pitch_desired;
static float yaw_rate, pitch, roll, roll_rate, pitch_rate;
static float rc_throttle, rc_steer, rc_roll, rc_pitch, rc_yaw_rate,
    rc_mode_switch;
static motors_pwm_s motors_pwm;

unsigned long last_time_us;
float dt;
vehicle_mode_t mode;

void globalPidInit() {
  pid_init(&pid_inner.pid_yaw, 1, 0.01, 1, -100, 100);
  pid_init(&pid_inner.pid_roll, 1, 0.01, 1, -100, 100);
  pid_init(&pid_inner.pid_pitch, 1, 0.01, 1, -100, 100);
}

void changeToCopter() {
  mode = MODE_COPTER;
  Serial.println("Switched to COPTER mode");
  globalPidInit();
  pid_init(&pid_outer.pid_pitch, 1, 0.01, 1, -100, 100);
  pid_init(&pid_outer.pid_roll, 1, 0.01, 1, -100, 100);

  // TODO: Servo changing to copter position
}

void changeToTank() {
  mode = MODE_TANK;
  Serial.println("Switched to TANK mode");
  globalPidInit();

  // TODO: Servo changing to tank position
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  if (!mpuSensor.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Found!");
  Serial.println("");
  delay(100);

  // Sbus init
  sbus_rx.Begin();
  Serial.println("SBUS Reader Started");

  // TODO: tune PID parameters
  pid_init(&pid_inner.pid_yaw, 1, 0.01, 1, -100, 100);
  pid_init(&pid_inner.pid_roll, 1, 0.01, 1, -100, 100);
  pid_init(&pid_inner.pid_pitch, 1, 0.01, 1, -100, 100);

  if (mode == MODE_COPTER) {
    pid_init(&pid_outer.pid_pitch, 1, 0.01, 1, -100, 100);
    pid_init(&pid_outer.pid_roll, 1, 0.01, 1, -100, 100);
  }

  last_time_us = micros();
}

void loop() {

  unsigned long now_us = micros();
  dt = (now_us - last_time_us) / 1000000.0f;
  if (dt <= 0)
    dt = 0.001f;
  last_time_us = now_us;

  mpuSensor.computeOrientation();

  // Get sensor events using getters
  sensors_event_t a = mpuSensor.getAccelEvent();
  sensors_event_t g = mpuSensor.getGyroEvent();
  sensors_event_t temp = mpuSensor.getTempEvent();

  yaw_rate = mpuSensor.getYawRate();
  roll = mpuSensor.getRoll();
  pitch = mpuSensor.getPitch();
  roll_rate = mpuSensor.getRollRate();
  pitch_rate = mpuSensor.getPitchRate();

  if (sbus_rx.Read()) {
    sbus_data = sbus_rx.data();
    rc_mode_switch = sbus_data.ch[RC_MODE_CH];
  } else {
    // No new data, skip this loop iteration
    return;
  }

  // Determine mode based on switch position
  if (rc_mode_switch > 1000) {
    if (mode != MODE_COPTER) {
      changeToCopter();
    }
  } else {
    if (mode != MODE_TANK) {
      changeToTank();
    }
  }

  // PID Update
  switch (mode) {
  case MODE_TANK:
    // Read SBUS data
    rc_throttle = sbus_data.ch[RC_THROTTLE_CH];
    rc_steer = sbus_data.ch[RC_STEER_CH];

    yaw_sp = rc_steer * STEER_COEF;
    throttle_sp = rc_throttle * THROTTLE_COEF_TANK;
    roll_sp = 0;
    pitch_sp = 0;

    pid_yaw_ctrl = pid_compute(&pid_inner.pid_yaw, yaw_rate, yaw_sp, dt);
    pid_roll_ctrl = pid_compute(&pid_inner.pid_roll, roll, roll_sp, dt);
    pid_pitch_ctrl = pid_compute(&pid_inner.pid_pitch, pitch, pitch_sp, dt);

    motors_pwm.fl = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
    motors_pwm.fr = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;
    motors_pwm.bl = motors_pwm.fl;
    motors_pwm.br = motors_pwm.br;
    break;
  case MODE_COPTER:
    // Read SBUS data
    rc_throttle = sbus_data.ch[RC_THROTTLE_CH];
    rc_yaw_rate = sbus_data.ch[RC_STEER_CH];
    rc_pitch = sbus_data.ch[RC_PITCH_CH];
    rc_roll = sbus_data.ch[RC_ROLL_CH];

    yaw_sp = rc_yaw_rate * YAW_RATE_COEF;
    throttle_sp = rc_throttle * THROTTLE_COEF_COPTER;
    roll_sp = rc_roll * ROLL_COEF;
    pitch_sp = rc_pitch * PITCH_COEF;

    pid_yaw_ctrl = pid_compute(&pid_inner.pid_yaw, yaw_rate, yaw_sp, dt);
    roll_desired = pid_compute(&pid_outer.pid_roll, roll, roll_sp, dt);
    pitch_desired = pid_compute(&pid_outer.pid_pitch, pitch, pitch_sp, dt);
    pid_roll_ctrl =
        pid_compute(&pid_inner.pid_roll, roll_rate, roll_desired, dt);
    pid_pitch_ctrl =
        pid_compute(&pid_inner.pid_pitch, pitch_rate, pitch_desired, dt);

    motors_pwm.fr = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
    motors_pwm.fl = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl + pid_pitch_ctrl;
    motors_pwm.br = throttle_sp + pid_yaw_ctrl + pid_roll_ctrl - pid_pitch_ctrl;
    motors_pwm.bl = throttle_sp - pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;
    break;
  default:
    break;
  }

  setVehiclePWM(&motors_pwm);

  // Print PID output
  Serial.print("PID Control Yaw: ");
  Serial.print(pid_yaw_ctrl);
  Serial.print(", Roll: ");
  Serial.print(pid_roll_ctrl);
  Serial.print(", Pitch: ");
  Serial.println(pid_pitch_ctrl);

  // Print accelerometer readings
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  // Print gyroscope readings
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  // Print temperature
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  // Print orientation
  Serial.print("Pitch: ");
  Serial.print(mpuSensor.getPitch());
  Serial.print(" Roll: ");
  Serial.print(mpuSensor.getRoll());
  Serial.print(" Yaw (Relative): ");
  Serial.print(mpuSensor.getYaw());
  Serial.print(" Yaw Rate: ");
  Serial.println(mpuSensor.getYawRate());

  Serial.println("");
  delay(LOOP_DT);
}
