// Define constants
int penLiftedAngle = 0;
int penDroppedAngle = 60;

// Create a Servo object
Servo myServo;

void servoSetup(){
  // Attach the servo to the servo pin
  myServo.attach(SERVO_PIN);
  servoTurnTo(0);
}

//Turn servo to a given angle
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

//Move pen holder up
void penUp() {
  Serial.println("Pen up!");
  servoTurnTo(penLiftedAngle);
}

//Move pen holder down
void penDown() {
  Serial.println("Pen down!");
  servoTurnTo(penDroppedAngle);
}