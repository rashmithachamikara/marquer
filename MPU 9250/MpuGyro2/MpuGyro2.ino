#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

/* Variables for yaw calculation */
double yaw = 0.0;
unsigned long lastTime;
//double gyroBiasZ = 0.0; // Set this to the calibrated bias value
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
  // Calibrate gyroscope to find the bias (simple average over 100 samples)
  // double sum = 0.0;
  // int i = 0;
  // while(i<5000){
  //   if (imu.Read()) {
  //     sum += imu.gyro_z_radps();
  //     delay(10);
  //     Serial.println(imu.gyro_z_radps(),6);
  //     i++;
  //   }
  // }
  // Serial.print("gyroBiasZ: ");
  // Serial.println(sum,6);
  // gyroBiasZ = sum / 5000.0;
  // Serial.print("gyroBiasZ: ");
  // Serial.println(gyroBiasZ,6);

  // Initialize time
  lastTime = millis();
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    // Get gyroscope z-axis data and correct for bias
    //double gzRound = imu.gyro_z_radps() - gyroBiasZ; //Without rounding
    double gzRound = round((imu.gyro_z_radps() - gyroBiasZ)*100.00)/100.00; //Remove the gyro bias then round. Up to 2 Decimal points sensor does not include significant errors
    double gz = (gzRound)*(180.0 / PI); //Convert Radians per second to degrees per second

    // Calculate dt
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    // Integrate gyroscope data for yaw
    yaw += gz * dt;

    // Print out the yaw value
    Serial.print("Yaw: "); Serial.print(yaw,6);
    //Serial.print("\tYaw: "); Serial.print(yaw*(180.0 / PI));

    // Print out other sensor values if needed
    // Serial.print(imu.new_imu_data());
    // Serial.print("\t");
    // Serial.print(imu.new_mag_data());
    Serial.print("\t");
    Serial.print(imu.accel_x_mps2());
    Serial.print("\t");
    Serial.print(imu.accel_y_mps2());
    Serial.print("\t");
    Serial.print(imu.accel_z_mps2());
    Serial.print("\t");
    Serial.print(imu.gyro_x_radps());
    Serial.print("\t");
    Serial.print(imu.gyro_y_radps());
    Serial.print("\t");
    Serial.print(imu.gyro_z_radps(),6);
    Serial.print("\t");
    Serial.print(gz, 6); //Corrected
    Serial.print("\n");
    // Serial.print(imu.mag_x_ut());
    // Serial.print("\t");
    // Serial.print(imu.mag_y_ut());
    // Serial.print("\t");
    // Serial.print(imu.mag_z_ut());
    // Serial.print("\t");
    // Serial.print(imu.die_temp_c());
    // Serial.print("\n");

    delay(100);
  }
}
