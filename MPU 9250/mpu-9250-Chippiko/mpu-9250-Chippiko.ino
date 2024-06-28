//#include "quaternionFilters.h"
#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 myIMU;

//offset setting for North
float offsetCalibration = -0.5;

void setup() {
  /* Serial to display data */
  Serial.begin(115200);
  while(!Serial) {}

  /* Start the I2C bus */
  Wire.begin();
  Wire.setClock(400000);
  delay(2000);

  /* I2C bus,  0x68 address */
  myIMU.Config(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);

  /* Initialize and configure IMU */
  if (!myIMU.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while(1) {}
  }
  /* Set the sample rate divider */
  if (!myIMU.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while(1) {}
  }
}
void loop() {
  //myIMU.readMagData(myIMU.magCount);

  //myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
  //myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
  //myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];

  //myIMU.updateTime();

  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  //MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD, myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my, myIMU.mx, myIMU.mz, myIMU.deltat);
  MahonyQuaternionUpdate(myIMU.accel_x_mps2(), myIMU.accel_y_mps2(), myIMU.accel_z_mps2(), myIMU.gyro_x_radps() * DEG_TO_RAD, myIMU.gyro_y_radps() * DEG_TO_RAD, myIMU.gyro_z_radps() * DEG_TO_RAD, myIMU.mag_y_ut, myIMU.mag_x_ut, myIMU.mag_z_ut /*, myIMU.deltat*/);


  float heading = atan2(myIMU.mag_x_ut, myIMU.mag_y_ut) + offsetCalibration;

  // How to get the magnetic declination angle
  // is available in the article https://bit.ly/3Zd6e2z
  float declination = -0.3333;

  heading += declination;

  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  float angle = (heading * 180 / M_PI);
  
  Serial.println(angle);

  delay(150);
}