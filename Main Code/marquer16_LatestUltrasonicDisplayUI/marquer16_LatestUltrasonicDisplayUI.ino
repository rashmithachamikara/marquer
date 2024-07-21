/*

================= Marquer Main Code =========================

Originally copied from: Components/L298N/"L298NserialSpeedAdjust16_MarkurWeb_Gyro_PID_Turn_Overshootv2"

L298N Motor Control
Display encoder feedback input
PID for moving straight
PID for turning
Truning continously till turned 
Tuned for turning
Recover from stalling
Add turn speed value as a global variable
Changed PID system to run in realtime with 50ms dt clock
Count Distance from wheel encoders
Introuced a 50ms delay in main loop to avoid crashes and wifi usage induced power failures
Send data over wifi in a 500ms delta clock interval
Made the robot move a specified distance then stop
Created a context system
Calibrated the distance moving system. Added proportional control

===============================================================

*/

//Libraries
#include "marquerWeb.h"
#include "mpu9250.h" //By brain taylor 
#include <ESP32Servo.h>
#include "Ticker.h"
#include <PCF8574.h>

//L298N Pins
// #define IN1 25
// #define IN2 26
// #define IN3 27
// #define IN4 14
// #define ENA 12 //Right Motor Speed
// #define ENB 13 //Left Motor Speed

//Alternative pin configuration 1 for motors
// #define ENA 25 //Right Motor Speed
// #define IN1 26
// #define IN2 27
// #define IN3 14
// #define IN4 12
// #define ENB 13 //Left Motor Speed

//Alternative pin configuration 2 for motors
#define ENA 13 //Right Motor Speed
#define IN1 12
#define IN2 14
#define IN3 27
#define IN4 26
#define ENB 25 //Left Motor Speed

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

//Built-in LED
#define LED 2

#define SERVO_PIN 32

//Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 19

// ======== L298N =========
int speedA = 0; // Speed for motor A (0 to 255) Right
int speedB = 0; // Speed for motor B (0 to 255) Left

bool reverseA = false; // Direction flag for motor A
bool reverseB = false; // Direction flag for motor B

unsigned long currentTime; //Time since boot
unsigned long lastSerialUpdateTime = 0;
//===========================

// ========= Ultrasonic =========
Ticker ultrasonicTicker;
volatile double ultrasonicDistance;
volatile long startTime;
volatile long echoTime;
volatile bool measuring = false;

//Hoisted for callbacks
void IRAM_ATTR echoISR();
//===========================

//====== Display ======
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include <Arduino_JSON.h>
#include <Preferences.h>
//#include <Arduino.h> 

//Define custom colors
#define TFT_GREY 0x5AEB // New colour
#define TFT_ORANGE 0xFBE0 // New colour
#define TFT_WHITE 0xFFFF // New colour
#define TFT_PURPLE 0x209c // New colour

TFT_eSPI tft = TFT_eSPI();  // Invoke library

//variables
int uiPage = 1;
unsigned int old_uiPage = uiPage;
bool staticContentDrawn = false;
bool uiDebug = false;
bool manualMode = false;

//UIPages variables
int selectedPreset = 1;  // Current preset
bool connected = false;  // Change this to true if app is connected
static int preparingProgress = 0;  // Example value for progress out of 100
//======================================

// ========= Encoders =========

// Constants for encoder resolution
const float ENCODER_RESOLUTION = 40; // counts per rotation
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;
int lastEncoder1Count = 0;
int lastEncoder2Count = 0;
unsigned long lastInterruptTime1 = 0;
unsigned long lastInterruptTime2 = 0;
const double wheelDiameter = 21.25; //Whell diameter is measured to be 21.25cm
const double wheelDiameterInm = 21.25/100; //Whell diameter in meters
const unsigned long encoderSpeedInterval = 100;
double wheel1Rotations = 0;
double wheel2Rotations = 0;
double wheel1Distance = 0; //Distance in cm
double wheel2Distance = 0; //Distance in cm
double wheelDistance = 0;
double encoder1Rps = 0;
double encoder2Rps = 0;
double wheel1Speed = 0;
double wheel2Speed = 0;
double wheelSpeed = 0;

//Hoisted for callbacks
void calculateEncoderSpeed();
void handleEncoder1Interrupt();
void handleEncoder2Interrupt();

Ticker encoderTicker;
//===========================

// ========= Reciever =========
// Define I2C address for the PCF8574
#define PCF8574_ADDRESS 0x21

// Create a PCF8574 instance
PCF8574 pcf8574_reciever(PCF8574_ADDRESS);

// Define pin numbers for the PCF8574 (receiver)
byte receiverPins[4] = {0, 1, 2, 3}; // P0 to P3 of PCF8574

// Variables to store inputs
char recieverKeys[4] = {'B', 'D', 'A', 'C'}; // Reciever Keypad

// Variables to store previous states
int prevStates[4] = {LOW, LOW, LOW, LOW}; // Assuming LOW is the default state when nothing is pressed

// Debounce time
const unsigned long recieverDebounceDelay = 50;
unsigned long lastRecieverDebounceTime = 0;

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
double Kp = 0.6; //2.5;
double Ki = 0.6; //0.5;
double Kd = 0.6; //1;
double integral = 0;
double lastError = 0;
double dt = 0.1; // Time interval for PID calculation
unsigned long lastTime = 0;
unsigned long lastPidUpdateTime = 0;
int minPidSpeed = 50; //60;
int maxPidSpeed = 170; //180;

//Turning Variables
double turnTargetYaw = 0.00;
int turnDirection;
//PID constants for turning.
double turnKp = 0.4;
double turnKi = 0.4;
double turnKd = 1.5;
int minTurnSpeed = 60;
int maxTurnSpeed = 150;
int turning = 0; //Context switch. Change to more sophisticated method later
//int turnSpeedA = 0; //Caclulation variables. Probably unnecessary
//int turnSpeedB = 0; //Caclulation variables. Probably unnecessary
int turnBaseSpeed = 60; //Speed for turning. 100 for 7v+ (2x 3.7v). 60 for 12v+ (3x 3.7v). 30 with PCB as low wire resistance
double turnError;
double turnDerivative;
double turnIntegral = 0;
double lastTurnError = 0;
double turnPidOutput;
bool overshootMode = false;
//unsigned long lastTimeTurned = 0; //Probably unnecessary
unsigned long lastTurnPidTime = 0;
double turnErrorMargin = 1;
unsigned long turnCompletionTime = 0;
bool checkingOvershoot = false;
double overshootMargin = 2;
//===========================

// ========= Distance moving =========
//Distance Moving Variables
bool distanceMoving = false; //Context switch flag.
bool correctingTurnGap = false; //Cotenxt swith flag.
bool correctingStopRotation = false; //Cotenxt swith flag.
double targetDistance = 0;
double distanceMoveOvershoot = 3; //Found thorugh repeated experiment
double remaigningDistance = 0;
int movingBaseSpeed = 90;
//===========================

// ========= Command List (Context switches etc) =========
Ticker ticker; //Ticker instance for timed calls
bool executingCommandList = false;
unsigned long commandListStartTime = 0;
unsigned long commandListEndTime = 0;
unsigned long commandListExecutionTime = 0;

//Hosited for callbacks
void runCurrentCommand(String command);
void handleInput(String input);
//===========================

// ========= Wifi remote control =========
int wifiMoveSpeed = 100;
bool manualTurning = false;
int manualTurnDirection = 0; //0 right. 1 left
int manualTurnSpeed = 100;
//===========================

//============ Presets ============
struct Preset {
  String name;
  String instructions;
};

Preset presets[9]; // Maximum 9 presets
//=================================


void setup() {
  Serial.begin(115200);
  // Wait a moment for serial communication to stabilize
  delay(100);

  //Initial commandList
  String initialCommand = "T90 T-90 G50";
  setCommandList(initialCommand);

  //Assign PWM channel to ENA, ENB by using analogWrite().
  //Do this before attaching PWM to servo.
  //Attaching servo a PWM channel before analogWrite() might cause analogWrite() to attach the same PWM channel and clock used by servo to ENA,ENB pins when it is called later in the program.
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  //Call servoSetup after attaching PWM to ENA, ENB pins
  servoSetup();

  //pinMode(LED,OUTPUT);
  delay(500);
  //digitalWrite(LED,HIGH);

  // =============== Marquer WEB ===============
  setupMarquerWeb();

  delay(500);
  //digitalWrite(LED,LOW);

  // Set the callback function to handle POST messages
  setWebMessageCallback([](String message) {
    Serial.println("Received message via POST:");
    Serial.println(message);
    // Input Handling
    if (message.startsWith("PRESETS")) {
      savePresets(message); //Preset commands
    } else if(message.startsWith("Remote")){
      handleWifiRemoteInput(message.substring(7)); //Remote commands
    } else if(message.startsWith("Keypad")){
      handleUiInputs(message.charAt(7)); //UI commands
    } else {
      handleInput(message);//Global commands. Commands will be capitalized
    }

    
    
  });
  // =============== Marquer WEB END ===============

  displayUiSetup(); //Also setup keypad and presets

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

  // Set up the encoderTicker to call calculateEncoderSpeed at interval
  encoderTicker.attach_ms(encoderSpeedInterval, calculateEncoderSpeed);
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

  //Reciever
  recieverSetup();
  //Ultrasonic
  ultrasonicSetup();

  ledBlink(5);
}

void loop() {
  currentTime = millis();

  //========= MPU 9250 =========
  calculateYaw();
  //===========================

  //========= Reciever =========
  recieverLoop();
  //===========================

  //========= Ultrasonic =========
  ultrasonicLoop();
  //===========================

  //=========== PID ===========
  //Do PID per 50ms
  if (currentTime - lastPidUpdateTime >= 50){
    moveStraightPid();
  }
  //===========================

  //========= Turn =========
  if (turning){
    turn();
  }
  if (manualTurning){
    manualTurn();
  }
  //===========================

  //========= Distance moving =========
  if (distanceMoving){
    distanceMove();
  }
  if (correctingTurnGap && !turning){
    correctTurnGap();
  }
  //===========================

  //========= L298N =========
  // Control motor direction and speed
  setSpeedAndDir();
  //===========================

  //========= Encoder =========
  encoderCaclculations();
  //===========================

  //========= Display UI =========
  displayUiLoop();
  //===========================

  // Once every 200ms stuff
  if (currentTime - lastSerialUpdateTime >= 300) {
    loopMarquerWeb();

    //calculateEncoderSpeed(); //Now called by ticker

    // Send data to Serial Plotter in a single line with comma-separated values
    Serial.print("Ultrasonic:"); Serial.print(ultrasonicDistance);Serial.print(", ");
    Serial.print("Wheel Speed mps:"); Serial.print(wheelSpeed);Serial.print(", ");
    Serial.print("Wheel Speed cms:"); Serial.print(wheelSpeed*100);Serial.print(", ");
    Serial.print("Wheel Speed kmh:"); Serial.print(wheelSpeed*3.6);Serial.print(", ");
    Serial.print("Yaw: "); Serial.print(yaw,6);
    Serial.print("\n");

    lastSerialUpdateTime = currentTime;

    //WebPrint("sR:"+String(speedA));
    //WebPrint(", sL:"+String(speedB));
    //WebPrint(", Te:"+String(turnError));
    //WebPrint(", Ti:"+String(turnIntegral));
    //WebPrint(", Td:"+String(turnDerivative));
    //WebPrintln(", tPid:"+String(turnPidOutput));
    //WebPrint(", EncL:"+String(wheel1Rotations));
    //WebPrint(", WncR:"+String(wheel2Rotations));
    // WebPrint(", D1:"+String(wheel1Distance));
    // WebPrint(", D2:"+String(wheel2Distance));
    // WebPrint(", DT:"+String(wheelDistance));
    // WebPrint(", yaw:"+String(yaw));

    WebPrintln(
                "sL:"+String(speedB)+
                ", sR:"+String(speedA)+
                ", DT:"+String(wheelDistance)+
                ", UD:"+String(ultrasonicDistance)+
                ", yaw:"+String(yaw)
              );
  }

  //Check for serial input
  if (Serial.available() > 0) {  // Check if there is any serial input available
    String input = Serial.readStringUntil('\n');  // Read the input until newline character
    handleInput(input);  // Pass the input to the handleInput function
  }

  //delay(1); // Delays are not used anymore!
}

void distanceMove(){
  int slowingDistance=10; //Slow down from last 20cm

  remaigningDistance = targetDistance-wheelDistance;

  if (wheelDistance < targetDistance){
    if (speedA < minPidSpeed){
      speedA = movingBaseSpeed; //Make speed a bit more than pid baseline
    }
    if (speedB < minPidSpeed){
      speedB = movingBaseSpeed; //Make speed a bit more than pid baseline
    }

    if (remaigningDistance<slowingDistance){
      //Limit speed proportionately to remaigning distance at last 20cm.
      double speedAMultiplier = constrain(remaigningDistance/slowingDistance, 0.8, 1);
      double speedBMultiplier = constrain(remaigningDistance/slowingDistance, 0.8, 1);
      
      //Don't let speed get too low
      speedA = constrain(speedA*speedAMultiplier, minPidSpeed , maxPidSpeed);
      speedB = constrain(speedB*speedBMultiplier, minPidSpeed , maxPidSpeed);
    }

  } else {
    //Active break
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    delay(10);
    //Halt after moving
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    reverseA = false;
    reverseB = false;
    delay(100);
    //Print debug stuff then set flag
    WebPrint("Moved a distance of "+String(wheelDistance)+"cm.");
    distanceMoving = false;
    
    //Enable turn gap correcting mode
    correctingStopRotation = true; //For correcting stop rotation
  
    //reset distance
    encoder1Count = 0;
    encoder2Count = 0;
    lastEncoder1Count = 0;
    lastEncoder2Count = 0;
    wheelDistance = 0;
    //Set new target
    targetDistance = 11;

    //Lift pen
    penUp();

    //Correct turn error
    WebPrintln("Correcting stop rotation of " + String(yaw));
    double yawCorrection = 0; //yaw * -1;
    //handleInput("T"+String(yawCorrection));
    String argument = "T"+String(yawCorrection);
    ticker.once_ms(500, std::bind(handleInput, argument)); //Wait till pen is up

    //No need to execute next command as turning system does it after correcting turn gap, then correcting yaw
    // if (executingCommandList) {
    //   nextCommand();
    // }
  }
}

//Reverse distance move for clearing gap
void correctTurnGap(){
  int slowingDistance=10; //Slow down from last 20cm

  reverseA = true;
  reverseB = true;

  remaigningDistance = targetDistance-wheelDistance;

  if (wheelDistance < targetDistance){
    if (speedA < minPidSpeed){
      speedA = movingBaseSpeed; //Make speed a bit more than pid baseline
    }
    if (speedB < minPidSpeed){
      speedB = movingBaseSpeed; //Make speed a bit more than pid baseline
    }

    if (remaigningDistance<5){
      //Limit speed proportionately to remaigning distance at last 20cm.
      double speedAMultiplier = constrain(remaigningDistance/5, 0.8, 1);
      double speedBMultiplier = constrain(remaigningDistance/5, 0.8, 1);
      
      //Don't let speed get too low
      speedA = constrain(speedA*speedAMultiplier, minPidSpeed , maxPidSpeed);
      speedB = constrain(speedB*speedBMultiplier, minPidSpeed , maxPidSpeed);
    }

  } else {
    //Active break
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    delay(10);
    //Halt after moving
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    reverseA = false;
    reverseB = false;
    delay(100);
    //Print debug stuff then set flag
    WebPrintln("Turn gap compensation made. Moved a distance of "+String(wheelDistance)+"cm.");
    correctingTurnGap = false;

    //Correct turn error
    WebPrintln("Correcting stop rotation of " + String(yaw));
    double yawCorrection = 0; //yaw*-1;
    handleInput("T"+String(yawCorrection));

    //No need to execute next command as turning system does it after correcting yaw
    // if (executingCommandList) {
    //   nextCommand();
    // }
  }
}
