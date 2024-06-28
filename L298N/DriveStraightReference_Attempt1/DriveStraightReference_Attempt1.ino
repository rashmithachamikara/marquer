/*To understand the context and purpose of this code, visit:
 * https://www.instructables.com/How-to-Make-a-Robot-Car-Drive-Straight-and-Turn-Ex/
 * This code makes references to steps on this Instructables website
 * written by square1a on 7th July 2022
 * 
 * Acknowledgement:
 * some of the MPU6050-raw-data-extraction code in void loop() and most in calculateError() are written by Dejan from:
 * https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
*/
#include <Wire.h>
#include "mpu9250.h"
#include "marquerWeb.h"

//control pins for left and right motors
const int leftSpeed = 12; //means pin 9 on the Arduino controls the speed of left motor
const int rightSpeed = 13;
const int left1 = 27; //left 1 and left 2 control the direction of rotation of left motor
const int left2 = 14;
const int right1 = 25;
const int right2 = 26;

#define LED 2

/* Mpu9250 object */
bfs::Mpu9250 imu;

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ; //linear acceleration
float GyroX, GyroY, GyroZ; //angular velocity
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ; //used in void loop()
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
float lastWebPrintTime;
int c = 0;

const int maxSpeed = 200; //max PWM value written to motor speed pin. It is typically 255.
const int minSpeed = 80; //min PWM value at which motor moves
float angle; //due to how I orientated my MPU6050 on my car, angle = roll
float targetAngle = 0;
int equilibriumSpeed = 180; //rough estimate of PWM at the speed pin of the stronger motor, while driving straight 
//and weaker motor at maxSpeed
int leftSpeedVal;
int rightSpeedVal;
bool isDriving = false; //it the car driving forward OR rotate/stationary
bool prevIsDriving = true; //equals isDriving in the previous iteration of void loop()
bool paused = true; //is the program paused

void setup() {
  Serial.begin(115200);
  
  // =============== MPU6050 ===============
  // Wire.begin();                      // Initialize comunication
  // Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  // Wire.write(0x6B);                  // Talk to the register 6B
  // Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  // Wire.endTransmission(true);        //end the transmission
  // Call this function if you need to get the IMU error values for your module
  // =============== MPU6050 Ends ===============

  // =============== MPU9250 ===============
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
  // =============== MPU9250 END ===============

  // =============== Marquer WEB ===============
  digitalWrite(LED, HIGH);
  setupMarquerWeb();

  delay(500);
  digitalWrite(LED,LOW);

  // Set the callback function to handle POST messages
  setWebMessageCallback([](String message) {
    Serial.println("Received message via POST:");
    Serial.println(message);
    // Input Handling
    webInput(message);
  });

  // =============== Marquer WEB END ===============
  ledBlink(5);

  //Calibration goes here
  //calculateError(); //Did manual calibration, no runtimecalibration needed!
  //Manually specified these error values as figured out manually
  AccErrorX = 0.602773;
  AccErrorY = 1.015732;
  GyroErrorX = 0;
  GyroErrorY = 0;
  //GyroErrorZ = -0.018400;
  GyroErrorZ = -0.019030;

  delay(20);

  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(leftSpeed, OUTPUT);
  pinMode(rightSpeed, OUTPUT);
  currentTime = micros();
}

void loop() {
  // === Read accelerometer (on the MPU6050) data === //
  readAcceleration();
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; //AccErrorX is calculated in the calculateError() function
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY;
  

  // === Read gyroscope (on the MPU6050) data === //
  previousTime = currentTime;
  currentTime = micros();
  elapsedTime = (currentTime - previousTime) / 1000000; // Divide by 1000 to get seconds
  readGyro();
  // Correct the outputs with the calculated error values
  //GyroX -= GyroErrorX; //GyroErrorX is calculated in the calculateError() function
  //GyroY -= GyroErrorY;
  //GyroZ -= GyroErrorZ;
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX += (GyroX-GyroErrorX) * elapsedTime; // deg/s * s = deg
  gyroAngleY += (GyroY-GyroErrorY) * elapsedTime;
  yaw += (GyroZ-GyroErrorZ) * elapsedTime * (180.0 / PI); // Convert Rads/s to Deg/s
  //combine accelerometer- and gyro-estimated angle values. 0.96 and 0.04 values are determined through trial and error by other people
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  angle = yaw; //if you mounted MPU6050 in a different orientation to me, angle may not = roll. It can roll, pitch, yaw or minus version of the three
  //for me, turning right reduces angle. Turning left increases angle.
  
  // Print the values on the serial monitor
  if(Serial.available()){
    char c = Serial.read ();
    if (c == 'w') { //drive forward
      Serial.println("forward");
      isDriving = true;
    } else if (c == 'a') { //turn left
      Serial.println("left");
      targetAngle += 90;
      if (targetAngle > 180){
        targetAngle -= 360;
      }
      isDriving = false;
    } else if (c == 'd') { //turn right
      Serial.println("right");
      targetAngle -= 90;
      if (targetAngle <= -180){
        targetAngle += 360;
      }
      isDriving = false;
    } else if (c == 'q') { //stop or brake
      Serial.println("stop");
      isDriving = false;
    } else if (c == 'i') { //print information. When car is stationary, GyroX should approx. = 0. 
      Serial.print("angle: ");
      Serial.println(angle);
      Serial.print("targetAngle: ");
      Serial.println(targetAngle);
      Serial.print("GyroX: ");
      Serial.println(GyroX);
      Serial.print("elapsedTime (in ms): "); //estimates time to run void loop() once
      Serial.println(elapsedTime * pow(10, 3));
      Serial.print("equilibriumSpeed: ");
      Serial.println(equilibriumSpeed);
    } else if (c == 'p') { //pause the program
      paused = !paused;
      stopCar();
      isDriving = false;
      Serial.println("key p was pressed, which pauses/unpauses the program");
    }
  }

  static int count;
  static int countStraight;
  if (count < 6){  
    count ++;
  } else { //runs once after void loop() runs 7 times. void loop runs about every 2.8ms, so this else condition runs every 19.6ms or 50 times/second
    count = 0;
    if (!paused){
      if (isDriving != prevIsDriving){
          leftSpeedVal = equilibriumSpeed;
          countStraight = 0;
          Serial.print("mode changed, isDriving: ");
          Serial.println(isDriving);
      }
      if (isDriving) {
        if (abs(targetAngle - angle) < 3){
          if (countStraight < 20){
            countStraight ++;
          } else {
            countStraight = 0;
            equilibriumSpeed = leftSpeedVal; //to find equilibrium speed, 20 consecutive readings need to indicate car is going straight
            Serial.print("EQUILIBRIUM reached, equilibriumSpeed: ");
            Serial.println(equilibriumSpeed);
          }
        } else {
          countStraight = 0;
        }
        driving();
      } else {
        rotate();
      }
      prevIsDriving = isDriving;
    }
  }

  if(lastWebPrintTime <= (millis()-100)){
    WebPrint("Gx:"); WebPrint(GyroX);
    WebPrint("\tYaw:"); WebPrint(yaw);
    WebPrint("\tTar:"); WebPrint(targetAngle); 
    WebPrint("\tL:"); WebPrint(leftSpeedVal); 
    WebPrint("\tR:"); WebPrint(rightSpeedVal);
    WebPrint("\n");
    lastWebPrintTime = millis();
    loopMarquerWeb();
  }
  
}

void webInput(String input){
  // React to marquer web
    char c = Serial.read ();
    if (input.startsWith("A")) { //drive forward
      Serial.println("forward");
      isDriving = true;
    } else if (input.startsWith("DA")) { //turn left
      Serial.println("left");
      targetAngle += 90;
      if (targetAngle > 180){
        targetAngle -= 360;
      }
      isDriving = false;
    } else if (input.startsWith("DB")) { //turn right
      Serial.println("right");
      targetAngle -= 90;
      if (targetAngle <= -180){
        targetAngle += 360;
      }
      isDriving = false;
    } else if (input.startsWith("Q")) { //stop or brake
      Serial.println("stop");
      isDriving = false;
    } else if (input.startsWith("R")) { //Reset stats
      angle = 0;
      yaw = 0;
      targetAngle = 0;
    } else if (input.startsWith("P")) { //pause the program
      paused = !paused;
      stopCar();
      isDriving = false;
      Serial.println("key p was pressed, which pauses/unpauses the program");
    }
}

void driving (){//called by void loop(), which isDriving = true
  int deltaAngle = round(targetAngle - angle); //rounding is neccessary, since you never get exact values in reality
  forward();
  if (deltaAngle != 0){
    controlSpeed ();
    rightSpeedVal = maxSpeed;
    analogWrite(rightSpeed, rightSpeedVal);
    analogWrite(leftSpeed, leftSpeedVal);
  }
}

void controlSpeed (){//this function is called by driving ()
  int deltaAngle = round(targetAngle - angle);
  int targetGyroX;
  
  //setting up propoertional control, see Step 3 on the website
  if (deltaAngle > 30){
      targetGyroX = 60;
  } else if (deltaAngle < -30){
    targetGyroX = -60;
  } else {
    targetGyroX = 2 * deltaAngle;
  }
  
  if (round(targetGyroX - GyroX) == 0){
    ;
  } else if (targetGyroX > GyroX){
    leftSpeedVal = changeSpeed(leftSpeedVal, -1); //would increase GyroX
  } else {
    leftSpeedVal = changeSpeed(leftSpeedVal, +1);
  }
}

void rotate (){//called by void loop(), which isDriving = false
  int deltaAngle = round(targetAngle - angle);
  int targetGyroX;
  if (abs(deltaAngle) <= 1){
    stopCar();
  } else {
    if (angle > targetAngle) { //turn left
      left();
    } else if (angle < targetAngle) {//turn right
      right();
    }

    //setting up propoertional control, see Step 3 on the website
    if (abs(deltaAngle) > 30){
      targetGyroX = 60;
    } else {
      targetGyroX = 2 * abs(deltaAngle);
    }
    
    if (round(targetGyroX - abs(GyroX)) == 0){
      ;
    } else if (targetGyroX > abs(GyroX)){
      leftSpeedVal = changeSpeed(leftSpeedVal, +1); //would increase abs(GyroX)
    } else {
      leftSpeedVal = changeSpeed(leftSpeedVal, -1);
    }
    rightSpeedVal = leftSpeedVal;
    analogWrite(rightSpeed, rightSpeedVal);
    analogWrite(leftSpeed, leftSpeedVal);
  }
}   

int changeSpeed (int motorSpeed, int increment){
  motorSpeed += increment;
  if (motorSpeed > maxSpeed){ //to prevent motorSpeed from exceeding 255, which is a problem when using analogWrite
    motorSpeed = maxSpeed;
  } else if (motorSpeed < minSpeed){
    motorSpeed = minSpeed;
  }
  return motorSpeed;
}

void calculateError() {
  //When this function is called, ensure the car is stationary. See Step 2 for more info
  
  // Read accelerometer values 200 times
  c = 0;
  while (c < 200) {
    readAcceleration();
    // Sum all readings
    AccErrorX += (atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI);
    AccErrorY += (atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI);
    c++;
  }
  //Divide the sum by 200 to get the error value, since expected value of reading is zero
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  
  // Read gyro values 200 times
  while (c < 200) {
    readGyro();
    // Sum all readings
    GyroErrorX += GyroX;
    GyroErrorY += GyroY;
    GyroErrorZ += GyroZ;
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  Serial.println(GyroErrorX);
  Serial.println(GyroErrorY);
  Serial.println(GyroErrorZ);
}

void readAcceleration() {
  // Wire.beginTransmission(MPU);
  // Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  // Wire.endTransmission(false);
  // Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  // //For a range of +-2g, we need to divide the raw values by 16384, according to the MPU6050 datasheet
  // AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  // AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  // AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value

  // =============== MPU9250 ===============
  if (imu.Read()){
    AccX = imu.accel_x_mps2(); // X-axis value
    AccY = imu.accel_y_mps2(); // Y-axis value
    AccZ = imu.accel_z_mps2(); // Z-axis value
  }
  // =============== MPU9250 Ends ===============
}

void readGyro() {
  // Wire.beginTransmission(MPU);
  // Wire.write(0x43);
  // Wire.endTransmission(false);
  // Wire.requestFrom(MPU, 6, true);
  // GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  // GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  // GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  // =============== MPU9250 ===============
  if (imu.Read()){
    GyroX = imu.gyro_x_radps(); // X-axis value
    GyroY = imu.gyro_y_radps(); // Y-axis value
    GyroZ = imu.gyro_z_radps(); // Z-axis value
  }
  // =============== MPU9250 Ends ===============
}

void stopCar(){
  digitalWrite(right1, LOW);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(left2, LOW);
  analogWrite(rightSpeed, 0);
  analogWrite(leftSpeed, 0);
}

void forward(){ //drives the car forward, assuming leftSpeedVal and rightSpeedVal are set high enough
  digitalWrite(right1, HIGH); //the right motor rotates FORWARDS when right1 is HIGH and right2 is LOW
  digitalWrite(right2, LOW);
  digitalWrite(left1, HIGH);
  digitalWrite(left2, LOW);
}

void left(){ //rotates the car left, assuming speed leftSpeedVal and rightSpeedVal are set high enough
  digitalWrite(right1, LOW);
  digitalWrite(right2, HIGH);
  digitalWrite(left1, HIGH);
  digitalWrite(left2, LOW);
}

void right(){
  digitalWrite(right1, HIGH);
  digitalWrite(right2, LOW);
  digitalWrite(left1, LOW);
  digitalWrite(left2, HIGH);
}

void ledBlink(int times) {
  for (int i = 0; i <= times; i++) {
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
  }
  delay(100);
  digitalWrite(LED, LOW);
}