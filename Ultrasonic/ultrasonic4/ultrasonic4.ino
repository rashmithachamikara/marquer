#include <Ticker.h>

//Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 19

Ticker ultrasonicTicker;
volatile double distance;
volatile long startTime;
volatile long echoTime;
volatile bool measuring = false;

void IRAM_ATTR echoISR() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    startTime = micros();
  } else {
    echoTime = micros() - startTime;
    measuring = false;
  }
}

void measureDistance() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  measuring = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  // Set up the ultrasonicTicker to call measureDistance every second
  ultrasonicTicker.attach(1, measureDistance);
}

void loop() {
  if (!measuring) {
    distance = echoTime * 0.034 / 2; // Duration * SoundSpeed / 2

    // Print the distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(1000);  // Wait for a second before next print
  }
}
