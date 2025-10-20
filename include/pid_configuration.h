// need to set these defines in cpp code
// now there are some default values set in motor.cpp
// there should be new functions to set these values from outside in pid3d.h probably

// Inner (rate) loop gains
#define PID_INNER_ROLL_KP 1.0f
#define PID_INNER_ROLL_KI 0.01f
#define PID_INNER_ROLL_KD 1.0f
#define PID_INNER_PITCH_KP 1.0f
#define PID_INNER_PITCH_KI 0.01f
#define PID_INNER_PITCH_KD 1.0f
#define PID_INNER_YAW_KP 1.0f
#define PID_INNER_YAW_KI 0.01f
#define PID_INNER_YAW_KD 1.0f

// Outer (angle) loop gains
#define PID_OUTER_ROLL_KP 1.0f
#define PID_OUTER_ROLL_KI 0.01f
#define PID_OUTER_ROLL_KD 1.0f
#define PID_OUTER_PITCH_KP 1.0f
#define PID_OUTER_PITCH_KI 0.01f
#define PID_OUTER_PITCH_KD 1.0f
#define PID_OUTER_YAW_KP 0.0f
#define PID_OUTER_YAW_KI 0.0f
#define PID_OUTER_YAW_KD 0.0f

// Output limits (per axis/loop)
#define PID_INNER_ROLL_OUT_MIN -100.0f
#define PID_INNER_ROLL_OUT_MAX 100.0f
#define PID_INNER_PITCH_OUT_MIN -100.0f
#define PID_INNER_PITCH_OUT_MAX 100.0f
#define PID_INNER_YAW_OUT_MIN -100.0f
#define PID_INNER_YAW_OUT_MAX 100.0f

#define PID_OUTER_ROLL_OUT_MIN -50.0f
#define PID_OUTER_ROLL_OUT_MAX 50.0f
#define PID_OUTER_PITCH_OUT_MIN -50.0f
#define PID_OUTER_PITCH_OUT_MAX 50.0f
#define PID_OUTER_YAW_OUT_MIN -50.0f
#define PID_OUTER_YAW_OUT_MAX 50.0f

// Optional: integral clamp and D filter time constants (0 = off)
#define PID_INNER_ROLL_I_MIN -100.0f
#define PID_INNER_ROLL_I_MAX 100.0f
#define PID_INNER_PITCH_I_MIN -100.0f
#define PID_INNER_PITCH_I_MAX 100.0f
#define PID_INNER_YAW_I_MIN -100.0f
#define PID_INNER_YAW_I_MAX 100.0f

#define PID_OUTER_ROLL_I_MIN -50.0f
#define PID_OUTER_ROLL_I_MAX 50.0f
#define PID_OUTER_PITCH_I_MIN -50.0f
#define PID_OUTER_PITCH_I_MAX 50.0f
#define PID_OUTER_YAW_I_MIN -50.0f
#define PID_OUTER_YAW_I_MAX 50.0f

#define PID_INNER_ROLL_D_TAU 0.00f
#define PID_INNER_PITCH_D_TAU 0.00f
#define PID_INNER_YAW_D_TAU 0.00f
#define PID_OUTER_ROLL_D_TAU 0.00f
#define PID_OUTER_PITCH_D_TAU 0.00f
#define PID_OUTER_YAW_D_TAU 0.00f
