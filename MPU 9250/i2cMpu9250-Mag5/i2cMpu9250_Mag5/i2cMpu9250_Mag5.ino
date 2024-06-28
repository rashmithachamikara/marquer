#include <Wire.h>
#include <MPU9250_asukiaaa.h>

// Create an MPU9250 object
MPU9250_asukiaaa mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MPU9250
  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();
  mpu.beginMag();

  // Calibrate the magnetometer
  //mpu.magCalibrate();
}

void loop() {
  // Update sensor readings
  mpu.accelUpdate();
  mpu.gyroUpdate();
  mpu.magUpdate();

  // Get magnetometer readings
  float magX = mpu.magX();
  float magY = mpu.magY();
  float magZ = mpu.magZ();

  // Calculate heading
  float heading = atan2(magY, magX) * 180 / PI;
  if (heading < 0) {
    //heading += 360;
  }

  Serial.print("X: ");
  Serial.print(magX);
  Serial.print("\tY: ");
  Serial.print(magY);
  Serial.print("\tZ: ");
  Serial.print(magZ);
  Serial.print("\tHeading: ");
  Serial.print(heading);
  Serial.print("\n");

  delay(50);
}
