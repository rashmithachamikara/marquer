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

Todo
- Calibrate distance moving system. Probably add proportional control
- Create a context system

===============================================================

*/

//Libraries
#include "marquerWeb.h"
#include "mpu9250.h" //By brain taylor 
#include <ESP32Servo.h>
#include "Ticker.h"
#include <Arduino_JSON.h>
#include <Preferences.h>

//Pins
#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12 //Right Motor Speed
#define ENB 13 //Left Motor Speed

//Alternative pin configuration for motors
// #define ENA 25 //Right Motor Speed
// #define IN1 26
// #define IN2 27
// #define IN3 14
// #define IN4 12
// #define ENB 13 //Left Motor Speed

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

#define LED 2

#define SERVO_PIN 32

// ======== L298N =========
int speedA = 0; // Speed for motor A (0 to 255) Right
int speedB = 0; // Speed for motor B (0 to 255) Left

bool reverseA = false; // Direction flag for motor A
bool reverseB = false; // Direction flag for motor B

unsigned long currentTime; //Time since boot
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
double wheelDiameter = 21.25; //Whell diameter is measured to be 21.25cm
double wheel1Rotations = 0;
double wheel2Rotations = 0;
double wheel1Distance = 0; //Distance in cm
double wheel2Distance = 0; //Distance in cm
double wheelDistance = 0;

//Hoisted for callbacks
void handleEncoder1Interrupt();
void handleEncoder2Interrupt();

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
double turnErrorMargin = 2;
unsigned long turnCompletionTime = 0;
bool checkingOvershoot = false;
double overshootMargin = 3;
//===========================

// ========= Distance moving =========
//Distance Moving Variables
bool distanceMoving = false; //Context switch. Change to more sophisticated method later
double targetDistance = 0;
int movingBaseSpeed = 90;
//===========================

// ========= Command List (Context switches etc) =========
Ticker ticker; //Ticker instance for timed calls
bool executingCommandList = false;

//Hosited for callbacks
void runCurrentCommand(String command);
//===========================

//============ Presets ============
struct Preset {
  String name;
  String instructions;
};

Preset presets[9]; // Maximum 9 presets. This line is probably unnecessary
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
    if (message.startsWith("PRESETS")) {
      savePresets(message);
    } else {
      handleInput(message);
    }

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
  currentTime = millis();

  //========= MPU 9250 =========
  calculateYaw();
  //===========================


  //=========== PID ===========
  //Do PID per 50ms
  if (currentTime - lastPidUpdateTime >= 50){
    moveStraightPid();
  }
  //===========================

  //========= Turn =========
  if (turning == 1){
    //Serial.println("Hit MainLoop");
    turn();
  }
  //===========================

  //========= Distance moving =========
  if (distanceMoving){
    //Serial.println("Hit MainLoop");
    distanceMove();
  }
  //===========================

  //========= L298N =========
  // Control motor direction and speed
  setSpeedAndDir();
  //===========================


  //========= Encoder =========
  encoderCaclculations();
  //===========================

  // Once every 200ms stuff
  if (currentTime - lastSerialUpdateTime >= 500) {
    loopMarquerWeb();

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
  if (wheelDistance < targetDistance){
    if (speedA < minPidSpeed){
      speedA = movingBaseSpeed; //Make speed a bit more than pid baseline
    }
    if (speedB < minPidSpeed){
      speedB = movingBaseSpeed; //Make speed a bit more than pid baseline
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
    if (executingCommandList) {
      nextCommand();
    }
  }
}
