#include <ESP32Servo.h>

// Define the servo pin
const int servoPin = 32;

const int penLiftedAngle = 0;
const int penDroppedAngle = 60;

// Create a Servo object
Servo myServo;

void setup() {
  // Attach the servo to the servo pin
  myServo.attach(servoPin);

  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Servo Control Initialized");
  Serial.println("Enter an angle between 0 and 180 to move the servo:");
}

void loop() {
  // Check if data is available on the serial port
  if (Serial.available() > 0) {
    // Read the input from the serial port
    String input = Serial.readStringUntil('\n');

    // Remove any extra whitespace
    input.trim();

    // Convert the input to an integer
    int servoAngle = input.toInt();

    // Check if the input is a valid angle
    if (servoAngle >= 0 && servoAngle <= 180) {
      // Move the servo to the specified angle
      myServo.write(servoAngle);
      Serial.print("Moving to ");
      Serial.print(servoAngle);
      Serial.println(" degrees");
    } else {
      // If the input is not a valid angle, print an error message
      Serial.println("Invalid input. Please enter a number between 0 and 180.");
    }
  }
}

void penUp(){
  myServo.write(penDroppedAngle);
}

void penDown(){
  myServo.write(penLiftedAngle);
}

void servoTurnTo(int servoAngle){
  // Check if the input is a valid angle
  if (servoAngle >= 0 && servoAngle <= 180) {
    // Move the servo to the specified angle
    myServo.write(servoAngle);
    Serial.print("Moving to ");
    Serial.print(servoAngle);
    Serial.println(" degrees");
  } else {
    // If the input is not a valid angle, print an error message
    Serial.println("Invalid input. Please enter a number between 0 and 180.");
  }
}