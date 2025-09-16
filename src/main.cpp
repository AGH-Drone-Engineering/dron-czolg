
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

  // Get sensor events using getters
  sensors_event_t a = mpuSensor.getAccelEvent();
  sensors_event_t g = mpuSensor.getGyroEvent();
  sensors_event_t temp = mpuSensor.getTempEvent();

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
  Serial.println(mpuSensor.getYaw());

  Serial.println("");
  delay(20);
}
