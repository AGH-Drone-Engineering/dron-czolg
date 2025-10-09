#ifndef CONFIG_H
#define CONFIG_H

// ---------------------------
// Konfiguracja Serva
// ---------------------------

// Piny obsługujące serva
#define SERVO_PIN_LEFT_FRONT  2
#define SERVO_PIN_LEFT_REAR   3
#define SERVO_PIN_RIGHT_FRONT 4
#define SERVO_PIN_RIGHT_REAR  5

// Min/max kąty do ograniczeń
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// Stabilne pozycje (kąty w stopniach dla każdego serwa)
#define RIDE_LEFT_FRONT  90
#define RIDE_LEFT_REAR   90
#define RIDE_RIGHT_FRONT 90
#define RIDE_RIGHT_REAR  90

#define FLIGHT_LEFT_FRONT  30
#define FLIGHT_LEFT_REAR   30
#define FLIGHT_RIGHT_FRONT 30
#define FLIGHT_RIGHT_REAR  30

#endif // CONFIG_H
