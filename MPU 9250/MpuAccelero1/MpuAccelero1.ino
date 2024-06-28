//Calibrate accelrometer. 3 Axis

#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

/* Bias variables */
float accelBiasX = 0.0;
float accelBiasY = 0.0;
float accelBiasZ = 0.0;

void setup() {
  /* Serial to display data */
  Serial.begin(115200);
  while(!Serial) {}
  /* Start the I2C bus */
  Wire.begin();
  Wire.setClock(400000);
  /* I2C bus,  0x68 address */
  imu.Config(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
  /* Initialize and configure IMU */
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while(1) {}
  }
  /* Set the sample rate divider */
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while(1) {}
  }

  // Calibrate accelerometer to find the bias (simple average over 100 samples)
  float sumX = 0.0;
  float sumY = 0.0;
  float sumZ = 0.0;
  int sampleCount = 0;
  while (sampleCount < 100) {
    if (imu.Read()) {
      sumX += imu.accel_x_mps2();
      sumY += imu.accel_y_mps2();
      sumZ += imu.accel_z_mps2();
      sampleCount++;
      Serial.print(sampleCount); Serial.print(". "); Serial.print(sampleCount/100.0*100.0); Serial.print("%. ");
      Serial.print("Accel X: "); Serial.print(imu.accel_x_mps2(),6); Serial.print(" m/s²\t");
      Serial.print("Accel Y: "); Serial.print(imu.accel_y_mps2(),6); Serial.print(" m/s²\t");
      Serial.print("Accel Z: "); Serial.print(imu.accel_z_mps2(),6); Serial.print(" m/s²\t");
      Serial.print("\n");
      delay(10);
    }
  }
  accelBiasX = sumX / 100.0;
  accelBiasY = sumY / 100.0;
  accelBiasZ = sumZ / 100.0;

  Serial.print("\nCalibrated!\n");
  Serial.print("Accel X: "); Serial.print(accelBiasX,6); Serial.print(" m/s²\n");
  Serial.print("Accel Y: "); Serial.print(accelBiasY,6); Serial.print(" m/s²\n");
  Serial.print("Accel Z: "); Serial.print(accelBiasZ,6); Serial.print(" m/s²\n");
  Serial.print("\n");
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    // Get accelerometer data and correct for bias
    float ax = imu.accel_x_mps2() - accelBiasX;
    float ay = imu.accel_y_mps2() - accelBiasY;
    float az = imu.accel_z_mps2() - accelBiasZ;

    // Print out the corrected accelerometer values
    Serial.print("Accel X: "); Serial.print(ax,6); Serial.print(" m/s²\t");
    Serial.print("Accel Y: "); Serial.print(ay,6); Serial.print(" m/s²\t");
    Serial.print("Accel Z: "); Serial.print(az,6); Serial.println(" m/s²");

    delay(100);
  }
}
