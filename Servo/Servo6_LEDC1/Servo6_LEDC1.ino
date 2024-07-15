#include "esp32-hal-ledc.h"

#define SERVO_PIN 32

// Define the servo motor settings
const int pwmFreq = 50; // Standard servo frequency of 50Hz
const int pwmResolution = 16; // 16-bit resolution
const int pwmChannel = 0; // LEDC channel

// Servo parameters
const int minPulseWidth = 500;  // Minimum pulse width in microseconds
const int maxPulseWidth = 2500; // Maximum pulse width in microseconds

void setup() {
  Serial.begin(115200);

  // Set up the LEDC PWM for the servo
  bool attached = ledcAttachChannel(SERVO_PIN, pwmFreq, pwmResolution, pwmChannel);
  if (!attached) {
    Serial.println("Failed to attach LEDC channel");
    while (1);
  }

  // Move servo to 90 degrees as a test
  turnServoToAngle(0);
  delay(1000);
}

void loop() {
  // Example: Sweep the servo from 0 to 180 degrees and back using the function
  for (int angle = 0; angle <= 60; angle += 10) {
    turnServoToAngle(angle);
    delay(500);
  }

  delay(1000);

  for (int angle = 60; angle >= 0; angle -= 10) {
    turnServoToAngle(angle);
    delay(500);
  }

  delay(1000);
}

// Function to turn the servo to a given angle
void turnServoToAngle(int angle) {
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
