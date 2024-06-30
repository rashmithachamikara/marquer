/*

================= Marquer Main Code =========================

For now, copied from: Components/L298N/"L298NserialSpeedAdjust16_MarkurWeb_Gyro_PID_Turn_Overshootv2"

L298N Motor Control
Display encoder feedback input
PID for moving straight
PID for turning
Truning continously till turned 
Tuned for turning
Recover from stalling
Add turn speed value as a global variable
Changed PID system to run in realtime with 50ms dt clock

===============================================================

*/

#include "marquerWeb.h"
#include "mpu9250.h" //By brain taylor 

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12 //Right Motor Speed
#define ENB 13 //Left Motor Speed

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

#define LED 2

// ======== L298N =========
int speedA = 0; // Speed for motor A (0 to 255) Right
int speedB = 0; // Speed for motor B (0 to 255) Left

bool reverseA = false; // Direction flag for motor A
bool reverseB = false; // Direction flag for motor B

unsigned long lastSerialUpdateTime = 0;
//===========================

// ========= Encoders =========

// Constants for encoder resolution
const float ENCODER_RESOLUTION = 40; // counts per rotation
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;
int lastEncoder1Count = 0;
int lastEncoder2Count = 0;
unsigned long lastInterruptTime1 = 0;
unsigned long lastInterruptTime2 = 0;

void IRAM_ATTR handleEncoder1Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > 5){
    encoder1Count++;
    lastInterruptTime1 = interruptTime;
  }
}

void IRAM_ATTR handleEncoder2Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > 5){
    encoder2Count++;
    lastInterruptTime2 = interruptTime;
  }
}
//===========================

// ========= MPU9250 =========
/* Mpu9250 object */
bfs::Mpu9250 imu;

/* Variables for yaw calculation */
double yaw = 0.0;
unsigned long lastTimeMpu;
double gyroBiasZ = -0.019000; // Caluclated with repeated data collection average

// PID constants
double desiredYaw = 0.0; // Target yaw angle
double Kp = 2.5;
double Ki = 0.5;
double Kd = 1;
double integral = 0;
double lastError = 0;
double dt = 0.1; // Time interval for PID calculation
unsigned long lastTime = 0;
unsigned long lastPidUpdateTime = 0;
int minPidSpeed = 60;
int maxPidSpeed = 180;

//Turning Variables
double turnTargetYaw = 0.00;
int turnDirection;
int turning = 0; //Context switch. Change to more sophisticated method later
//int turnSpeedA = 0; //Caclulation variables. Probably unnecessary
//int turnSpeedB = 0; //Caclulation variables. Probably unnecessary
int turnBaseSpeed = 80; //Speed for turning
double turnIntegral = 0;
double lastTurnError = 0;
bool overshootMode = false;
//unsigned long lastTimeTurned = 0; //Probably unnecessary
double turnErrorMargin = 2;
unsigned long turnCompletionTime = 0;
bool checkingOvershoot = false;
double overshootMargin = 3;

//===========================

void setup() {
  Serial.begin(115200);
  // Wait a moment for serial communication to stabilize
  delay(100);

  pinMode(LED,OUTPUT);
  delay(500);
  digitalWrite(LED,HIGH);

  // =============== Marquer WEB ===============
  setupMarquerWeb();

  delay(500);
  digitalWrite(LED,LOW);

  // Set the callback function to handle POST messages
  setWebMessageCallback([](String message) {
    Serial.println("Received message via POST:");
    Serial.println(message);
    // Input Handling
    changeSpeed(message);
  });

  // =============== Marquer WEB END ===============

  //========= L298N =========
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
  //===========================

  //========= Encoders =========
  // Configure encoder pins as inputs
  pinMode(ENCODER1_PIN, INPUT);
  pinMode(ENCODER2_PIN, INPUT);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(ENCODER1_PIN), handleEncoder1Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2_PIN), handleEncoder2Interrupt, RISING);
  //===========================

  // ========= MPU9250 =========
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
  // Initialize time
  lastTimeMpu = millis();
  //===========================

  ledBlink(5);
}

void loop() {
  loopMarquerWeb();

  unsigned long currentTime = millis();

  //========= MPU 9250 =========
  /* Check if data read */
  if (imu.Read()) {
    //Get gyroscope z-axis data and correct for bias
    double gzRound = round((imu.gyro_z_radps() - gyroBiasZ) * 100.00) / 100.00; //With rounding
    double gz = (gzRound) * (180.0 / PI); //Convert Radians per second to degrees per second

    // Calculate Delta Time (dt)
    unsigned long currentTimeMpu = millis();
    double dtMpu = (currentTimeMpu - lastTimeMpu) / 1000.0;
    lastTimeMpu = currentTimeMpu;

    // Integrate gyroscope data for yaw
    yaw += gz * dtMpu;
  }
  //===========================

  //=========== PID ===========
  //Do PID per 100s
  if (currentTime - lastPidUpdateTime >= 50){
    dt = (currentTime - lastTime) / 1000.0; // Convert to seconds. Comment to use constant time

    // PID calculations for yaw control
    double error = desiredYaw - yaw;
    integral += error * dt;
    double derivative = (error - lastError) / dt;

    double pidOutput = Kp * error + Ki * integral + Kd * derivative;

    //Only apply PID if not turning
    if (turning == 0){
      // Adjust motor speeds based on PID output, if speed>20
      if (speedA>20 && speedB>20) {
        speedA = constrain(speedA - pidOutput, minPidSpeed, maxPidSpeed);
        speedB = constrain(speedB + pidOutput, minPidSpeed, maxPidSpeed);
      }
    }

    lastError = error;   
    lastTime = currentTime;
    lastPidUpdateTime = currentTime;
    //WebPrintln("Hit!"+String(currentTime));
  }
  //===========================

  //========= Turn =========
  if (turning == 1){
    Serial.println("Hit MainLoop");
    turn();
  }
  //===========================

  // Control motor direction and speed
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
  
  if (reverseA) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  if (reverseB) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  // Once every 200ms stuff
  if (currentTime - lastSerialUpdateTime >= 200) {
    float encoder1Speed = (encoder1Count - lastEncoder1Count) * (1000.0 / ENCODER_RESOLUTION); // Rotations per second
    float encoder2Speed = (encoder2Count - lastEncoder2Count) * (1000.0 / ENCODER_RESOLUTION); // Rotations per second

    // Send data to Serial Plotter in a single line with comma-separated values
    Serial.print("Encoder1Speed:"); Serial.print(encoder1Speed);Serial.print(", ");
    Serial.print("Encoder2Speed:"); Serial.print(encoder2Speed);Serial.print("\t");

    // Print out the yaw value
    Serial.print("Yaw: "); Serial.print(yaw,6);
    Serial.print("\n");

    lastEncoder1Count = encoder1Count;
    lastEncoder2Count = encoder2Count;
    lastSerialUpdateTime = currentTime;

    WebPrint("sR:"+String(speedA));
    WebPrint(", sL:"+String(speedB));
    WebPrint(", EncL:"+String(encoder1Count/ENCODER_RESOLUTION));
    WebPrint(", WncR:"+String(encoder2Count/ENCODER_RESOLUTION));
    //WebPrint("yaw:"+String(yaw));
    WebPrintln(", yaw:"+String(yaw));

  }

  //Check for serial input
  if (Serial.available() > 0) {  // Check if there is any serial input available
    String input = Serial.readStringUntil('\n');  // Read the input until newline character
    changeSpeed(input);  // Pass the input to the changeSpeed function
  }

  //delay(10); // Short delay to avoid overloading the Serial communication
}

void turn() {
  bool turned = false;
  //Direction 0 - right. 1 - left
  Serial.println("Hit turn() function"); 

  //PID constants for turning. Change these names to unique names different from drive straight names after testings
  double Kp = 0.6;
  double Ki = 0.4;
  double Kd = 1.5;

  //Turn PID
  double turnError = abs(turnTargetYaw - yaw);
  turnIntegral += turnError * dt; //Integral should not exceed 200. No need. Such value is abnormal
  double turnDerivative = (turnError - lastTurnError) / dt;

  //Constrain the turn integral
  turnIntegral = constrain(turnIntegral, -200, 200);

  double error = (Kp * turnError) + (Ki * turnIntegral) + (Kd * turnDerivative);
  lastTurnError = turnError;

  //Set turn direction. Yaw should be 0 at the beginning of the turn
  if (turnTargetYaw > yaw) {
    turnDirection = 0; //Right
  } else {
    turnDirection = 1; //Left
  }

  //Anti Motor Stall. Pause and give a massive speed suddenly if integral is too much (which means it stayed in once yaw for long)
  if (turnIntegral >= 180) {
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    WebPrintln("Anti Stall System Activated!");
    turnIntegral = 100; //Reduce the integral a bit
    delay(100); //Wait a bit to recover
  }

  //Turn Right
  if (turnDirection == 0) {
    //Configure right motor to reverse
    reverseA = true;
    reverseB = false;
  }
  //Turn left
  else if (turnDirection == 1) {
    reverseA = false;
    reverseB = true;
  }

  //Apply power
  if(!checkingOvershoot){
    speedA = constrain(turnBaseSpeed + error, 80, 180);
    speedB = constrain(turnBaseSpeed + error, 80, 180);
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
  }
  //In overshoot mode, apply a constant power
  if(overshootMode){
    speedA = constrain(turnBaseSpeed*1.4 , 80, 180);
    speedB = constrain(turnBaseSpeed*1.4, 80, 180);
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
  }

  //Check if target Yaw reached
  if (yaw >= (turnTargetYaw - turnErrorMargin) && yaw <= (turnTargetYaw + turnErrorMargin)) {  
    turned = true;
  } else {
    turned = false;
  }

  if (turned && !checkingOvershoot) {
    overshootMode = false; //Set the overshoot mode to false if not set yet
    //Active break
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    delay(10);
    //Halt after turning
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    reverseA = false;
    reverseB = false;
    //yaw = 0;
    turnIntegral = 0;
    lastTurnError = 0;

    //Start checking for overshoot after some time
    turnCompletionTime = millis();
    checkingOvershoot = true;
  }

  if (checkingOvershoot) {
    //Check for overshoot after 1 second
    if (millis() - turnCompletionTime >= 1000) {
      if (yaw > turnTargetYaw + overshootMargin || yaw < turnTargetYaw - overshootMargin) {
        overshootMode = true;
        Serial.println("Overshoot detected!");
        WebPrintln("Lower margin: " + String(turnTargetYaw-overshootMargin));
        WebPrintln("Upper margin: " + String(turnTargetYaw+overshootMargin));
        WebPrintln("Current YAw: " + String(yaw));
        WebPrintln("Significant Overshoot Detected. Retrying!");
      } else {
        Serial.println("No overshoot.");
        WebPrintln("No overshoot.");
        //Finalize turning
        Serial.println("Turned!");
        WebPrintln("Turned");
        //Halt after turning
        speedA = 0;
        speedB = 0;
        analogWrite(ENA, speedA);
        analogWrite(ENB, speedB);
        reverseA = false;
        reverseB = false;
        turnIntegral = 0;
        lastTurnError = 0;
        overshootMode = false;
        turning = 0;
      }
      checkingOvershoot = false; // Stop checking after the first check
    }
  }
}

//Function to blink built-in LED a given amount of times
void ledBlink(int times){
  for (int i=0; i<=times; i++){
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
  }
  delay(100);
  digitalWrite(LED,LOW);
}

void changeSpeed(String input) {
  input.trim(); // Remove any extra whitespace
  input.toUpperCase();

  if (input.startsWith("A")) {
    speedA = input.substring(1).toInt();
    speedA = constrain(speedA, 0, 255); // Constrain speed to 0-255
    //turnSpeedA = speedA;
    Serial.print("Speed of Motor A set to: ");
    Serial.println(speedA);
  } else if (input.startsWith("B")) {
    speedB = input.substring(1).toInt();
    speedB = constrain(speedB, 0, 255); // Constrain speed to 0-255
    //turnSpeedB = speedB;
    Serial.print("Speed of Motor B set to: ");
    Serial.println(speedB);
  } else if (input.startsWith("C")) {
    int speed = input.substring(1).toInt();
    speed = constrain(speed, 0, 255); // Constrain speed to 0-255
    speedA = speed;
    speedB = speed;
    Serial.print("Speed of both Motors A and B set to: ");
    Serial.println(speed);
  } else if (input.startsWith("P")) {
    Kp = input.substring(1).toDouble();
    Serial.print("Kp Set to: ");
    Serial.println(Kp);
    WebPrintln("Kp Set to: " + String(Kp));
  } else if (input.startsWith("I")) {
    Ki = input.substring(1).toDouble();
    Serial.print("Ki Set to: ");
    Serial.println(Ki);
    WebPrintln("Ki Set to: " + String(Ki));
  } else if (input.startsWith("D")) {
    Kd = input.substring(1).toDouble();
    Serial.print("Kd Set to: ");
    Serial.println(Kd);
    WebPrintln("Kd Set to: " + String(Kd));
  } else if (input.startsWith("M")) {
    turnErrorMargin = input.substring(1).toDouble();
    Serial.print("Turn error Margin Set to: ");
    Serial.println(turnErrorMargin);
    WebPrintln("Turn error Margin Set to: " + String(turnErrorMargin));
  } else if (input.startsWith("T")) {
    //Stops before turning
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    //Reset Yaw and Set turnTargetYaw
    turnTargetYaw = input.substring(1).toDouble();
    yaw = 0;
    //turn(); //No need. Will be called in the main loop
    turning = 1;
  } else if (input.startsWith("R")) {
    //Encoder
    encoder1Count = 0;
    encoder2Count = 0;
    lastEncoder1Count = 0;
    lastEncoder2Count = 0;
    //PID
    yaw = 0;
    integral = 0;
    lastError = 0;
  } else if (input.startsWith("DA")) {
    reverseA = !reverseA; // Toggle direction for Motor A
    Serial.print("Direction of Motor A set to: ");
    Serial.println(reverseA ? "Reverse" : "Forward");
  } else if (input.startsWith("DB")) {
    reverseB = !reverseB; // Toggle direction for Motor B
    Serial.print("Direction of Motor B set to: ");
    Serial.println(reverseB ? "Reverse" : "Forward");
  }

  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}
