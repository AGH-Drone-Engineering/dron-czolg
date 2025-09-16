#pragma once

typedef struct {
  float Kp, Ki, Kd;
  float prev_err;
  float integral;
  float out_min, out_max;
} pid_s;

// pid_init - Initialize PID controller.
// @param pid Pointer to a struct holding PID parameters
// @param Kp Proportional gain
// @param Ki Integral gain
// @param Kd Differential gain
// @param out_max Output max value
// @param out_min Output min value
void pid_init(pid_s *pid, float Kp, float Ki, float Kd, float out_max, float out_min);

// pid_compute - Return PID value based on measurement.
float pid_compute(pid_s *pid, float meas, float sp, float dt);