#include <ESP32Servo.h>

Servo myServo;  // Create a servo object

void setup() {
  Serial.begin(115200);
  myServo.attach(32);  // Attach the servo on pin 32
  Serial.println("Servo attached to pin 32");
}

void loop() {
  for (int pos = 0; pos <= 60; pos++) {  // Move the servo from 0 to 180 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
  for (int pos = 60; pos >= 0; pos--) {  // Move the servo back from 180 to 0 degrees
    myServo.write(pos);              // Tell servo to go to position in variable 'pos'
    delay(15);                       // Wait 15 ms for the servo to reach the position
  }
}
