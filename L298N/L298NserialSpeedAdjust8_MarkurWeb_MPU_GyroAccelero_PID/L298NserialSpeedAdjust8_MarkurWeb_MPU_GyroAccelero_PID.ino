#include "marquerWeb.h"
#include "mpu9250.h"
#include <math.h>

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12
#define ENB 13

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

#define LED 2

/* Mpu9250 object */
bfs::Mpu9250 imu;

// MPU variables
float AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

const int maxSpeed = 255; //max PWM value written to motor speed pin
const int minSpeed = 160; //min PWM value at which motor moves
float angle; //due to how I oriented my MPU9250 on my car, angle = roll
float targetAngle = 0;
int equilibriumSpeed = 230; //rough estimate of PWM at the speed pin of the stronger motor
int leftSpeedVal;
int rightSpeedVal;
bool isDriving = false; //is the car driving forward or rotating/stationary
bool prevIsDriving = true; //equals isDriving in the previous iteration of void loop()
bool paused = true; //is the program paused

bool reverseA = false; // Direction flag for motor A
bool reverseB = false; // Direction flag for motor B

float lastWebPrintTime = 0;

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

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  delay(500);
  digitalWrite(LED, HIGH);

  // =============== Marquer WEB ===============
  setupMarquerWeb();

  delay(500);
  digitalWrite(LED, LOW);

  // Set the callback function to handle POST messages
  setWebMessageCallback([](String message) {
    Serial.println("Received message via POST:");
    Serial.println(message);
    // Input Handling
    changeSpeed(message);
  });

  // =============== Marquer WEB END ===============

  // =============== MPU9250 ===============
  /* Start the I2C bus */
  Wire.begin();
  Wire.setClock(400000);
  /* I2C bus,  0x68 address */
  imu.Config(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
  /* Initialize and configure IMU */
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while (1) {}
  }
  /* Set the sample rate divider */
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while (1) {}
  }
  // Calculate the sensor error
  calculateError();
  // =============== MPU9250 END ===============

  // Set all motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable the motors
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Initialize motors in stopped state
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Wait a moment for serial communication to stabilize
  delay(100);

  ledBlink(5);
}

void loop() {

  // Read IMU data
  readSensorData();

  // Control logic for driving and rotating
  if (!paused) {
    if (isDriving != prevIsDriving) {
      leftSpeedVal = equilibriumSpeed;
      prevIsDriving = isDriving;
    }
    if (isDriving) {
      driving();
    } else {
      rotate();
    }
  }

  
  if(lastWebPrintTime <= (currentTime-100)){
    WebPrint("yaw: ");
    WebPrint(yaw);
    WebPrint("\tLSpeed: ");
    WebPrint(leftSpeedVal);
    WebPrint("\tRSpeed: ");
    WebPrint(rightSpeedVal);
    WebPrint("\n");
    lastWebPrintTime = currentTime;
    loopMarquerWeb();
  }
  
  delay(20); // Short delay to avoid overloading the Serial communication
}

void readSensorData() {
  if (imu.Read()) {
    // Read accelerometer data
    AccX = imu.accel_x_mps2();
    AccY = imu.accel_y_mps2();
    AccZ = imu.accel_z_mps2();
    // Calculate accelerometer angles
    accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX;
    accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY;

    // Read gyroscope data
    previousTime = currentTime;
    currentTime = millis();
    elapsedTime = (currentTime - previousTime) / 1000.0; // Convert to seconds
    GyroX = imu.gyro_x_radps() - GyroErrorX;
    GyroY = imu.gyro_y_radps() - GyroErrorY;
    GyroZ = imu.gyro_z_radps() - GyroErrorZ;

    // Calculate gyroscope angles
    gyroAngleX += GyroX * elapsedTime;
    gyroAngleY += GyroY * elapsedTime;
    yaw += GyroZ * elapsedTime;

    // Combine accelerometer and gyroscope data
    roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
    pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
    angle = roll;
  }
}

void driving() {
  int deltaAngle = round(targetAngle - angle);
  forward();
  if (deltaAngle != 0) {
    controlSpeed();
    rightSpeedVal = maxSpeed;
    analogWrite(ENA, rightSpeedVal);
    analogWrite(ENB, leftSpeedVal);
  }
}

void controlSpeed() {
  int deltaAngle = round(targetAngle - angle);
  int targetGyroX;

  // Proportional control
  if (deltaAngle > 30) {
    targetGyroX = 60;
  } else if (deltaAngle < -30) {
    targetGyroX = -60;
  } else {
    targetGyroX = 2 * deltaAngle;
  }

  if (round(targetGyroX - GyroX) == 0) {
    return;
  } else if (targetGyroX > GyroX) {
    leftSpeedVal = changeSpeed(leftSpeedVal, -1);
  } else {
    leftSpeedVal = changeSpeed(leftSpeedVal, +1);
  }
}

void rotate() {
  int deltaAngle = round(targetAngle - angle);
  int targetGyroX;
  if (abs(deltaAngle) <= 1) {
    stopCar();
  } else {
    if (angle > targetAngle) {
      left();
    } else if (angle < targetAngle) {
      right();
    }

    // Proportional control
    if (abs(deltaAngle) > 30) {
      targetGyroX = 60;
    } else {
      targetGyroX = 2 * abs(deltaAngle);
    }

    if (round(targetGyroX - abs(GyroX)) == 0) {
      return;
    } else if (targetGyroX > abs(GyroX)) {
      leftSpeedVal = changeSpeed(leftSpeedVal, +1);
    } else {
      leftSpeedVal = changeSpeed(leftSpeedVal, -1);
    }
    rightSpeedVal = leftSpeedVal;
    analogWrite(ENA, rightSpeedVal);
    analogWrite(ENB, leftSpeedVal);
  }
}

int changeSpeed(int motorSpeed, int increment) {
  motorSpeed += increment;
  if (motorSpeed > maxSpeed) {
    motorSpeed = maxSpeed;
  } else if (motorSpeed < minSpeed) {
    motorSpeed = minSpeed;
  }
  return motorSpeed;
}

void calculateError() {
  // When this function is called, ensure the car is stationary.
  c = 0;
  while (c < 200) {
    readSensorData();
    AccErrorX += (atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI);
    AccErrorY += (atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI);
    GyroErrorX += GyroX;
    GyroErrorY += GyroY;
    GyroErrorZ += GyroZ;
    c++;
  }
  AccErrorX /= 200;
  AccErrorY /= 200;
  GyroErrorX /= 200;
  GyroErrorY /= 200;
  GyroErrorZ /= 200;
  Serial.println("Gyroscope calibrated");
}

void forward() {
  digitalWrite(IN1, !reverseA);
  digitalWrite(IN2, reverseA);
  digitalWrite(IN3, !reverseB);
  digitalWrite(IN4, reverseB);
}

void left() {
  digitalWrite(IN1, reverseA);
  digitalWrite(IN2, !reverseA);
  digitalWrite(IN3, !reverseB);
  digitalWrite(IN4, reverseB);
}

void right() {
  digitalWrite(IN1, !reverseA);
  digitalWrite(IN2, reverseA);
  digitalWrite(IN3, reverseB);
  digitalWrite(IN4, !reverseB);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void changeSpeed(String message) {
  message.trim();
  if (message.startsWith("A")) {
    isDriving = true;
    paused = false;
  } else if (message.startsWith("DB")) {
    targetAngle -= 90;
    if (targetAngle <= -180) {
      targetAngle += 360;
      paused = false;
    }
    isDriving = false;
  } else if (message.startsWith("DA")) {
    targetAngle += 90;
    if (targetAngle > 180) {
      targetAngle -= 360;
      
    }
    isDriving = true;
    paused = false;
  } else if (message.startsWith("C")) {
    isDriving = false;
    paused = true;
    stopCar();
    Serial.print("Hit!");
  }
}
