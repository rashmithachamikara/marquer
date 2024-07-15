#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

//Hoisted for callbacks
void handleEncoder1Interrupt();
void handleEncoder2Interrupt();

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

// ========= Distance moving =========
//Distance Moving Variables
bool distanceMoving = false; //Context switch. Change to more sophisticated method later
double targetDistance = 0;
int movingBaseSpeed = 100;

void setup(){
  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(ENCODER1_PIN), handleEncoder1Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2_PIN), handleEncoder2Interrupt, RISING);
}

void loop(){
  Serial.print(encoder1Count);
  Serial.print("\t");
  Serial.print(encoder2Count);
  Serial.print("\t");
  Serial.print(wheel1Rotations);
  Serial.print("\t");
  Serial.print(wheel2Rotations);
  Serial.print("\t");
  Serial.print(wheel1Distance);
  Serial.print("\t");
  Serial.print(wheel2Distance);
  Serial.print("\t");
  Serial.print(wheelDistance);
  Serial.print("\n");
  delay(100); //delay to avoid serial overload
  
}

//Encoder1 interrupts
void IRAM_ATTR handleEncoder1Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > 5){
    encoder1Count++;
    lastInterruptTime1 = interruptTime;
  }
}

//Encoder2 interrupts
void IRAM_ATTR handleEncoder2Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > 5){
    encoder2Count++;
    lastInterruptTime2 = interruptTime;
  }
}

//Do encoder caluclations including rotations and distances
void encoderCaclculations(){
  wheel1Rotations = encoder1Count/ENCODER_RESOLUTION;
  wheel2Rotations = encoder2Count/ENCODER_RESOLUTION;
  wheel1Distance = wheel1Rotations*wheelDiameter;
  wheel2Distance = wheel2Rotations*wheelDiameter;
  wheelDistance = (wheel1Distance+wheel2Distance)/2;
}