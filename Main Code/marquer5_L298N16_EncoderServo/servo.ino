void servoSetup() {
  // Attach the servo to the servo pin
  myServo.attach(servoPin);

  // Initialize serial communication if not already initialized
  if (!Serial) {
    Serial.begin(115200);
  }
  Serial.println("Servo Control Initialized");
  myServo.write(penLiftedAngle); // Lift the pen on initialization
}

void penUp() {
  Serial.println("Pen up!");
  moveServoToAngle(penLiftedAngle);
}

void penDown() {
  Serial.println("Pen down!");
  moveServoToAngle(penDroppedAngle);
}

void moveServoToAngle(int angle) {
  // Check if the input is a valid angle
  if (angle >= 0 && angle <= 180) {
    // Move the servo to the specified angle
    myServo.write(angle);
    Serial.print("Moving to ");
    Serial.print(angle);
    Serial.println(" degrees");
    delay(500);
  } else {
    // If the input is not a valid angle, print an error message
    Serial.println("Invalid input. Please enter a number between 0 and 180.");
  }
}

void servoUpDown(){
  Serial.println("Hit");
  for (int pos = 0; pos <= 60; pos++) {  // Move the servo from 0 to 180 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
  for (int pos = 60; pos >= 0; pos--) {  // Move the servo back from 180 to 0 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
}
