#include <ESP32Servo.h>

// Define the servo pin
const int servoPin = 32;

// Create a Servo object
Servo myServo;

void setup() {
  // Attach the servo to the servo pin
  myServo.attach(servoPin);

  // Initialize serial communication
  Serial.begin(115200);
}

void loop() {
  // Move the servo to 0 degrees
  Serial.println("Moving to 0 degrees");
  myServo.write(0);
  delay(1000);

  // Move the servo to 90 degrees
  Serial.println("Moving to 90 degrees");
  myServo.write(90);
  delay(1000);

  // Move the servo to 180 degrees
  Serial.println("Moving to 180 degrees");
  myServo.write(180);
  delay(1000);

  // Move the servo back to 90 degrees
  Serial.println("Moving back to 90 degrees");
  myServo.write(90);
  delay(1000);
}
