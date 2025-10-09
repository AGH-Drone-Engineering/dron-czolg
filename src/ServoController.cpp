#include "ServoController.h"

ServoController::ServoController() : attached(false) {
    // Domyślne kąty odpowiadają pozycji RIDE
    currentAngles[LEFT_FRONT] = RIDE_LEFT_FRONT;
    currentAngles[LEFT_REAR] = RIDE_LEFT_REAR;
    currentAngles[RIGHT_FRONT] = RIDE_RIGHT_FRONT;
    currentAngles[RIGHT_REAR] = RIDE_RIGHT_REAR;
}

// Odczepia serwa
ServoController::~ServoController() {
    if (!attached) return;
    for (int i = 0; i < SERVO_COUNT; ++i) {
        servos[i].detach();
    }
    attached = false;
}

// Ogranicz kąt do zakresu z config.h
int clampAngle(int angle) {
    if (angle < SERVO_MIN_ANGLE) return SERVO_MIN_ANGLE;
    if (angle > SERVO_MAX_ANGLE) return SERVO_MAX_ANGLE;
    return angle;
}

// Attach serw i ustawienie ich na domyślną pozycję
void ServoController::begin() {
    if (attached) return;

    // przypięcie serw do pinów
    servos[LEFT_FRONT].attach(SERVO_PIN_LEFT_FRONT);
    servos[LEFT_REAR].attach(SERVO_PIN_LEFT_REAR);
    servos[RIGHT_FRONT].attach(SERVO_PIN_RIGHT_FRONT);
    servos[RIGHT_REAR].attach(SERVO_PIN_RIGHT_REAR);

    // ustawienie pozycji startowej (RIDE)
    setServoAngle(LEFT_FRONT, RIDE_LEFT_FRONT);
    setServoAngle(LEFT_REAR, RIDE_LEFT_REAR);
    setServoAngle(RIGHT_FRONT, RIDE_RIGHT_FRONT);
    setServoAngle(RIGHT_REAR, RIDE_RIGHT_REAR);

    attached = true;
}

// Przełącz wszystkie serwa na jedną z pozycji
void ServoController::setStablePosition(Position pos) {
    // jeśli serwa nieprzypięte, przypnij je automatycznie
    if (!attached) begin();

    if (pos == RIDE) {
        setServoAngle(LEFT_FRONT, RIDE_LEFT_FRONT);
        setServoAngle(LEFT_REAR, RIDE_LEFT_REAR);
        setServoAngle(RIGHT_FRONT, RIDE_RIGHT_FRONT);
        setServoAngle(RIGHT_REAR, RIDE_RIGHT_REAR);
    } else if (pos == FLIGHT) {
        setServoAngle(LEFT_FRONT, FLIGHT_LEFT_FRONT);
        setServoAngle(LEFT_REAR, FLIGHT_LEFT_REAR);
        setServoAngle(RIGHT_FRONT, FLIGHT_RIGHT_FRONT);
        setServoAngle(RIGHT_REAR, FLIGHT_RIGHT_REAR);
    }
}

// Ustaw wszystkie serva na zadany kąt (ten sam kąt dla wszystkich)
void ServoController::setEachAngle(int angleDeg) {
    if (!attached) begin();
    int a = clampAngle(angleDeg);
    for (int i = 0; i < SERVO_COUNT; i++) {
        servos[i].write(a);
        currentAngles[i] = a;
    }
}

// Ustaw jedno servo na zadany kąt
void ServoController::setServoAngle(Servo servo, int angleDeg) {
    if (!attached) begin();

    int a = clampAngle(angleDeg);

    servos[servo].write(a);
    currentAngles[servo] = a;
}

// Pobierz ostatnio ustawiony kąt danego serwa
int ServoController::getCurrentAngle(Servo servo) const { return currentAngles[servo]; }
