//Calibration with scalling

/*
X-Axis Calibration:

Positive X: Orient the sensor with the X-axis pointing upwards (positive gravity direction). This means the top of the sensor should face the sky.
Negative X: Flip the sensor so that the X-axis points downwards (negative gravity direction). The top of the sensor should face downwards.
Y-Axis Calibration:

Positive Y: Orient the sensor so that the Y-axis points upwards (positive gravity direction). This means one of the sides of the sensor should face upwards.
Negative Y: Flip the sensor so that the Y-axis points downwards (negative gravity direction). One of the sides of the sensor should face downwards.
Z-Axis Calibration:

Positive Z: Orient the sensor so that the Z-axis points upwards (positive gravity direction). This means the front of the sensor (where the pins are) should face upwards.
Negative Z: Flip the sensor so that the Z-axis points downwards (negative gravity direction). The front of the sensor should face downwards.
*/

#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

/* Bias and scale factor variables */
float accelBiasX = 0.0;
float accelBiasY = 0.0;
float accelBiasZ = 0.0;
float accelScaleX = 1.0;
float accelScaleY = 1.0;
float accelScaleZ = 1.0;

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

  calibrateAccelerometer();
}

void calibrateAccelerometer() {
  float accelX[6], accelY[6], accelZ[6];
  const int samples = 100;
  float sumX, sumY, sumZ;

  // Measure +X direction
  Serial.println("Calibrating +X direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[0] = sumX / samples;
  accelY[0] = sumY / samples;
  accelZ[0] = sumZ / samples;
  delay(2000);

  // Measure -X direction
  Serial.println("Calibrating -X direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[1] = sumX / samples;
  accelY[1] = sumY / samples;
  accelZ[1] = sumZ / samples;
  delay(2000);

  // Measure +Y direction
  Serial.println("Calibrating +Y direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[2] = sumX / samples;
  accelY[2] = sumY / samples;
  accelZ[2] = sumZ / samples;
  delay(2000);

  // Measure -Y direction
  Serial.println("Calibrating -Y direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[3] = sumX / samples;
  accelY[3] = sumY / samples;
  accelZ[3] = sumZ / samples;
  delay(2000);

  // Measure +Z direction
  Serial.println("Calibrating +Z direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[4] = sumX / samples;
  accelY[4] = sumY / samples;
  accelZ[4] = sumZ / samples;
  delay(2000);

  // Measure -Z direction
  Serial.println("Calibrating -Z direction...");
  stageWait();
  delay(2000);
  measureAcceleration(sumX, sumY, sumZ, samples);
  accelX[5] = sumX / samples;
  accelY[5] = sumY / samples;
  accelZ[5] = sumZ / samples;
  delay(2000);

  // Calculate bias and scale factors
  accelBiasX = (accelX[0] + accelX[1]) / 2.0;
  accelBiasY = (accelY[2] + accelY[3]) / 2.0;
  accelBiasZ = (accelZ[4] + accelZ[5]) / 2.0;

  accelScaleX = (2.0 * 9.81) / (accelX[0] - accelX[1]);
  accelScaleY = (2.0 * 9.81) / (accelY[2] - accelY[3]);
  accelScaleZ = (2.0 * 9.81) / (accelZ[4] - accelZ[5]);

  Serial.println("Calibration Complete");
  Serial.print("Accel Bias X: "); Serial.println(accelBiasX);
  Serial.print("Accel Bias Y: "); Serial.println(accelBiasY);
  Serial.print("Accel Bias Z: "); Serial.println(accelBiasZ);
  Serial.print("Accel Scale X: "); Serial.println(accelScaleX);
  Serial.print("Accel Scale Y: "); Serial.println(accelScaleY);
  Serial.print("Accel Scale Z: "); Serial.println(accelScaleZ);

  delay(1000);
}

void measureAcceleration(float &sumX, float &sumY, float &sumZ, int samples) {
  sumX = 0.0;
  sumY = 0.0;
  sumZ = 0.0;
  int sampleCount = 0;
  while (sampleCount < samples) {
    if (imu.Read()) {
      sumX += imu.accel_x_mps2();
      sumY += imu.accel_y_mps2();
      sumZ += imu.accel_z_mps2();
      sampleCount++;
      Serial.print(sampleCount); Serial.print(". "); 
      Serial.print(imu.accel_x_mps2(),6); Serial.print("\t"); 
      Serial.print(imu.accel_y_mps2(),6); Serial.print("\t"); 
      Serial.print(imu.accel_z_mps2(),6); Serial.print("\t"); 
      Serial.print("\n"); 
      delay(10); 
    }
  }
}

void stageWait(){
  Serial.println("Input 'n' to proceed to the next stage!");
  while(1){
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == 'n') {
        Serial.println("Proceeding to the next stage!");
        return;
      }
    }
    delay(10);
  } 
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    // Get accelerometer data, correct for bias and apply scale factor
    float ax = (imu.accel_x_mps2() - accelBiasX) * accelScaleX;
    float ay = (imu.accel_y_mps2() - accelBiasY) * accelScaleY;
    float az = (imu.accel_z_mps2() - accelBiasZ) * accelScaleZ;

    // Print out the corrected accelerometer values
    Serial.print("Accel X: "); Serial.print(ax); Serial.print(" m/s²\t");
    Serial.print("Accel Y: "); Serial.print(ay); Serial.print(" m/s²\t");
    Serial.print("Accel Z: "); Serial.print(az); Serial.println(" m/s²");

    delay(100);
  }
}
