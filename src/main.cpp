
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
static float yaw_sp, roll_sp, pitch_sp, throttle_sp, roll_desired, pitch_desired;
static float yaw_rate, pitch, roll, roll_rate, pitch_rate;
static float rc_throttle, rc_steer, rc_roll, rc_pitch, rc_yaw_rate, rc_mode_switch;
static motors_pwm_s motors_pwm;

unsigned long last_time_us;
float dt;
vehicle_mode_t mode;

void globalPidInit() {
    pid_init(&pid_inner.pid_yaw, 1, 0.01, 1, -100, 100);
    pid_init(&pid_inner.pid_roll, 1, 0.01, 1, -100, 100);
    pid_init(&pid_inner.pid_pitch, 1, 0.01, 1, -100, 100);
}

void resetPwm() {
    motors_pwm.bl = 0;
    motors_pwm.br = 0;
    motors_pwm.fl = 0;
    motors_pwm.fr = 0;
    motors_pwm.tl = 0;
    motors_pwm.tr = 0;
    setVehiclePWM(&motors_pwm);
}

void changeToCopter() {
    mode = MODE_COPTER;
    Serial.println("Switched to COPTER mode");
    globalPidInit();
    pid_init(&pid_outer.pid_pitch, 1, 0.01, 1, -100, 100);
    pid_init(&pid_outer.pid_roll, 1, 0.01, 1, -100, 100);
    resetPwm();

    // TODO: Servo changing to copter position

    // small delay
    delay(1000);
}

void changeToTank() {
    mode = MODE_TANK;
    Serial.println("Switched to TANK mode");
    globalPidInit();
    resetPwm();

    // TODO: Servo changing to tank position

    // small delay
    delay(1000);
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

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

    // Default mode
    mode = DEFAULT_MODE;
    if (mode == MODE_TANK) {
        changeToTank();
    } else {
        changeToCopter();
    }

    last_time_us = micros();
}

void loop() {
    unsigned long now_us = micros();
    dt = (now_us - last_time_us) / 1000000.0f;
    if (dt <= 0) dt = 0.001f;
    last_time_us = now_us;

    mpuSensor.computeOrientation();

    yaw_rate = mpuSensor.getYawRate();
    roll = mpuSensor.getRoll();
    pitch = mpuSensor.getPitch();
    roll_rate = mpuSensor.getRollRate();
    pitch_rate = mpuSensor.getPitchRate();

    if (sbus_rx.Read()) {
        sbus_data = sbus_rx.data();
        rc_mode_switch = sbus_data.ch[RC_MODE_CH];

        // TODO: implement proper failsafe handling instead of destroying drone
        // maybe something like if we lost signal for over a second then the
        // failsafe starts working?
        if (sbus_data.failsafe || sbus_data.lost_frame) {
            setVehiclePWM(0);  // Stop motors or enter safe mode
            Serial.println("SBUS FAILSAFE or LOST FRAME!");
            return;
        }
    } else {
        setVehiclePWM(0);  // Stop motors or enter safe mode
        Serial.println("No SBUS data, stopping motors!");
        return;
    }

    // Determine mode based on switch position
    if (rc_mode_switch > 1000) {
        // Ensure all motors are stopped before switching modes
        if (mode != MODE_COPTER && motors_pwm.tl <= SWITCH_MOTOR_PWM_THRESHOLD &&
            motors_pwm.tr <= SWITCH_MOTOR_PWM_THRESHOLD) {
            changeToCopter();
        }
    } else {
        // Ensure all motors are stopped before switching modes
        if (mode != MODE_TANK && motors_pwm.bl <= SWITCH_MOTOR_PWM_THRESHOLD &&
            motors_pwm.br <= SWITCH_MOTOR_PWM_THRESHOLD && motors_pwm.tl <= SWITCH_MOTOR_PWM_THRESHOLD &&
            motors_pwm.tr <= SWITCH_MOTOR_PWM_THRESHOLD) {
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

            motors_pwm.tl = throttle_sp - pid_yaw_ctrl + pid_roll_ctrl + pid_pitch_ctrl;
            motors_pwm.tr = throttle_sp + pid_yaw_ctrl - pid_roll_ctrl - pid_pitch_ctrl;
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
            pid_roll_ctrl = pid_compute(&pid_inner.pid_roll, roll_rate, roll_desired, dt);
            pid_pitch_ctrl = pid_compute(&pid_inner.pid_pitch, pitch_rate, pitch_desired, dt);

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
