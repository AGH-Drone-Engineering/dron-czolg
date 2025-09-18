
#include "mpu6050_sensor.h"
#include "sbus_receiver.h"

extern "C" {
  #include "pid.h"
  #include "config.h"
  #include "motor.h"
}

Mpu6050Sensor mpuSensor;
sensors_event_t a, g, temp;

// PID
static pid_s pid_yaw, pid_roll, pid_pitch;
static float pid_yaw_ctrl, pid_roll_ctrl, pid_pitch_ctrl;
static float yaw_sp, throttle_sp;
static float yaw_rate, pitch, roll;
static float rc_throttle, rc_steer;
static float esc_in_val_left;
static float esc_in_val_right;

unsigned long dt;

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

  pid_init(&pid_yaw, 1, 0.01, 1, -100, 100);
  pid_init(&pid_roll, 1, 0.01, 1, -100, 100);
  pid_init(&pid_pitch, 1, 0.01, 1, -100, 100);
}

void loop() {

  mpuSensor.computeOrientation();

  // Get sensor events using getters
  sensors_event_t a = mpuSensor.getAccelEvent();
  sensors_event_t g = mpuSensor.getGyroEvent();
  sensors_event_t temp = mpuSensor.getTempEvent();

  // Update setpoints (from radio RX) TODO
  rc_throttle = 0;  // forward/back
  rc_steer = 0;     // left/right

  // Read SBUS data
  if (sbus_rx.Read()) {
    sbus_data = sbus_rx.data();
    rc_throttle = sbus_data.ch[RC_THROTTLE_CH];
    rc_steer = sbus_data.ch[RC_STEER_CH];
  }

  yaw_sp = rc_steer * STEER_COEF;
  throttle_sp = rc_throttle * THROTTLE_COEF;

  // PID Update
  yaw_rate = mpuSensor.getYawRate();
  pid_yaw_ctrl = pid_compute(&pid_yaw, yaw_rate, yaw_sp, dt);
  roll = mpuSensor.getRoll();
  pid_roll_ctrl = pid_compute(&pid_roll, roll, 0, dt);
  pitch = mpuSensor.getPitch();
  pid_pitch_ctrl = pid_compute(&pid_pitch, pitch, 0, dt);

  // Motor control
  esc_in_val_left = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
  esc_in_val_right = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;
  setTrackPWM(esc_in_val_left, esc_in_val_right);

  // Print PID output
  Serial.print("PID Control Yaw: ");
  Serial.print(pid_yaw_ctrl);
  Serial.print(", Roll: ");
  Serial.print(pid_roll_ctrl);
  Serial.print(", Pitch: ");
  Serial.println(pid_pitch_ctrl);

  // // Print accelerometer readings
  // Serial.print("Acceleration X: ");
  // Serial.print(a.acceleration.x);
  // Serial.print(", Y: ");
  // Serial.print(a.acceleration.y);
  // Serial.print(", Z: ");
  // Serial.print(a.acceleration.z);
  // Serial.println(" m/s^2");

  // // Print gyroscope readings
  // Serial.print("Rotation X: ");
  // Serial.print(g.gyro.x);
  // Serial.print(", Y: ");
  // Serial.print(g.gyro.y);
  // Serial.print(", Z: ");
  // Serial.print(g.gyro.z);
  // Serial.println(" rad/s");

  // // Print temperature
  // Serial.print("Temperature: ");
  // Serial.print(temp.temperature);
  // Serial.println(" degC");

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
