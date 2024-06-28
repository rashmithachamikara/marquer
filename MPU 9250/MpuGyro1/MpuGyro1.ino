#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

float currentAngle = 0.0;
unsigned long previousTime = 0;

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

  previousTime = millis();
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - previousTime) / 1000.0; // Convert to seconds
    previousTime = currentTime;
    
    // Read angular velocity around the Z-axis
    float angularVelocityZ = imu.gyro_z_radps()-; // Angular velocity in radians per second
    float angularVelocityZ_deg = angularVelocityZ * 180.0 / PI; // Convert to degrees per second

    // Calculate the change in angle
    float deltaAngle = angularVelocityZ_deg * deltaTime;

    // Update the current angle
    currentAngle += deltaAngle;

    // Print the current angle and other details to the Serial Monitor
    Serial.print("Current Angle: ");
    Serial.print(currentAngle);
    Serial.print(" degrees\t");
    Serial.print("Angular Velocity: ");
    Serial.print(angularVelocityZ_deg);
    Serial.print(" deg/s\t");
    Serial.print("Delta Time: ");
    Serial.print(deltaTime);
    Serial.println(" s");

    // Print raw sensor data
    Serial.print("Gyro X: ");
    Serial.print(imu.gyro_x_radps());
    Serial.print(" rad/s\t");
    Serial.print("Gyro Y: ");
    Serial.print(imu.gyro_y_radps());
    Serial.print(" rad/s\t");
    Serial.print("Gyro Z: ");
    delay(100); // Adjust the delay as needed
  }
}
