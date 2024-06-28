/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2021 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

/*
Set Baud rate to 115200

Pins
VCC -> 3.3v
GND -> gnd
SCL -> D22
SDA -> D21
*/

#include "mpu9250.h"

/* Mpu9250 object */
bfs::Mpu9250 imu;

int az,az2,az3, x,y,z;
float heading, headingDegrees, headingFiltered, declination;

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
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {
    Serial.print(imu.new_imu_data());
    Serial.print("\t");
    Serial.print(imu.new_mag_data());
    Serial.print("\t");
    Serial.print(imu.mag_x_ut());
    Serial.print("\t");
    Serial.print(imu.mag_y_ut());
    Serial.print("\t");
    Serial.print(imu.mag_z_ut());
    Serial.print("\t");

    //Get angle
    x=imu.mag_x_ut();
    y=imu.mag_y_ut();
    z=imu.mag_z_ut();
    //az = 90-atan(y/x)*180/M_PI;
    //az2 = atan2(y, x) * 180 / M_PI;


    //Calculating Heading
  heading = atan2(y, x);
 
  // Correcting the heading with the declination angle depending on your location
  // You can find your declination angle at: https://www.ngdc.noaa.gov/geomag-web/
  // At my location it's 4.2 degrees => 0.073 rad
  declination = 0.033335; 
  heading += declination;
  
  // Correcting when signs are reveresed
  if(heading <0) heading += 2*PI;

  // Correcting due to the addition of the declination angle
  if(heading > 2*PI)heading -= 2*PI;

  headingDegrees = heading * 180/PI; // The heading in Degrees unit

  // Smoothing the output angle / Low pass filter 
  headingFiltered = headingFiltered*0.85 + headingDegrees*0.15;



    Serial.print(az);
    Serial.print("\t");
    Serial.print(headingFiltered);
    
    Serial.print("\t");
    Serial.print(imu.die_temp_c());
    Serial.print("\n");
  }
}
