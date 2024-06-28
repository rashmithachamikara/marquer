#include <Wire.h>
#include "mpu9250.h"
#include <MadgwickAHRS.h>

// Define MPU9250 instance
//MPU9250 mpu(Wire, 0x68);
//int status;
/* Mpu9250 object */
bfs::Mpu9250 imu;

// Define Madgwick filter instance
Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float deltat; // integration interval for both filter schemes

void setup() {
  Serial.begin(115200);
  Wire.begin();

  while(!Serial) {}
  /* Start the I2C bus */
  //Wire.begin();
  //Wire.setClock(400000);
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

  // Set up the filter
  filter.begin(100);
  
  // Set integration interval
  microsPerReading = 1000000 / 100; // 100 Hz
  microsPrevious = micros();
}

void loop() {

  // Calculate delta time
  unsigned long microsNow = micros();
  deltat = (microsNow - microsPrevious) / 1000000.0f;
  microsPrevious = microsNow;
  if (imu.Read()) {
    // Update the filter
    filter.update(imu.gyro_x_radps(), imu.gyro_y_radps(), imu.gyro_z_radps(),
                imu.accel_x_mps2(), imu.accel_y_mps2(), imu.accel_z_mps2(),
                imu.mag_x_ut(), imu.mag_y_ut(), imu.mag_z_ut());

    // Get the yaw, pitch, and roll values
    float yaw = filter.getYaw();
    float pitch = filter.getPitch();
    float roll = filter.getRoll();

    // Print the yaw value
    Serial.print("Yaw: ");
    Serial.println(yaw);
  }
  

  delay(10);
}
