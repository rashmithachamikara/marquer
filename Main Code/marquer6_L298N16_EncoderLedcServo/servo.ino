#define SERVO_PIN 32

// Define the servo motor settings
const int pwmFreq = 50; // Standard servo frequency of 50Hz
const int pwmResolution = 16; // 16-bit resolution
const int pwmChannel = 15; // LEDC channel

// Servo parameters
const int minPulseWidth = 500;  // Minimum pulse width in microseconds
const int maxPulseWidth = 2500; // Maximum pulse width in microseconds

void servoSetup() {
  // Set up the LEDC PWM for the servo
  bool attached = ledcAttachChannel(SERVO_PIN, pwmFreq, pwmResolution, pwmChannel);
  if (!attached) {
    Serial.println("Failed to attach LEDC channel to Servo pin");
    while (1);
  }

  // Move servo to 90 degrees as a test
  moveServoToAngle(0);
  delay(100);
}

void penUp() {
  Serial.println("Pen up!");
  moveServoToAngle(penLiftedAngle);
}

void penDown() {
  Serial.println("Pen down!");
  moveServoToAngle(penDroppedAngle);
}

void servoUpDown(){
  for (int pos = 0; pos <= 60; pos++) {  // Move the servo from 0 to 180 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
  for (int pos = 60; pos >= 0; pos--) {  // Move the servo back from 180 to 0 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
}

// Function to turn the servo to a given angle
void moveServoToAngle(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  int pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
  uint32_t dutyCycle = pulseWidthToDutyCycle(pulseWidth);
  ledcWrite(SERVO_PIN, dutyCycle);
}

// Helper function to convert pulse width in microseconds to duty cycle
uint32_t pulseWidthToDutyCycle(int pulseWidth) {
  // Calculate duty cycle for 16-bit resolution
  return ((1 << pwmResolution) * pulseWidth) / (1000000 / pwmFreq);
}