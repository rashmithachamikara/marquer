#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

/* Variables for yaw calculation */
double yaw = 0.0;
unsigned long lastTime;
double gyroBiasZ = -0.019000; // Caluclated with repeated data collection average

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

  delay(100);

  // Initialize time
  lastTime = millis();
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    // Get gyroscope z-axis data and correct for bias
    //double gzRound = imu.gyro_z_radps() - gyroBiasZ; //Without rounding
    double gzRound = round((imu.gyro_z_radps() - gyroBiasZ)*100.00)/100.00; //With rounding. Remove the gyro bias then round. Up to 2 Decimal points sensor does not include significant errors
    double gz = (gzRound)*(180.0 / PI); //Convert Radians per second to degrees per second

    // Calculate Delta Time (dt)
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    // Integrate gyroscope data for yaw
    yaw += gz * dt;

    // Print out the yaw value
    Serial.print("Yaw: "); Serial.print(yaw,6);

    // Print out other sensor values if needed
    Serial.print("\t");
    Serial.print(imu.gyro_z_radps(), 6); //Raw Gyro
    Serial.print("\t");
    Serial.print(imu.gyro_z_radps() - gyroBiasZ, 6); //Corrected Raw Gyro
    Serial.print("\t");
    Serial.print(round((imu.gyro_z_radps() - gyroBiasZ)*100.00)/100.00, 6); //Corrected and Rounded Raw Gyro
    Serial.print("\t");
    Serial.print(gz, 6); //Corrected
    Serial.print("\n");

    delay(100);
  }
}
