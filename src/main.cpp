
#include "Mpu6050Sensor.h"

Mpu6050Sensor mpuSensor;
sensors_event_t a, g, temp;

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
}

void loop() {

  mpuSensor.computeOrientation();

  Serial.print("Pitch: ");
  Serial.print(mpuSensor.getPitch());
  Serial.print(" Roll: ");
  Serial.println(mpuSensor.getRoll());

  delay(500);
}
