#include <Ticker.h>

#define TRIG_PIN 19
#define ECHO_PIN 5

Ticker ultrasonicTicker;
volatile double distance;

void measureDistance() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance
  distance = duration * 0.034 / 2; //Duration*SounSpeed/2
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Set up the ultrasonicTicker to call measureDistance every second
  ultrasonicTicker.attach(1, measureDistance);
}

void loop() {
  // Print the distance
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);  // Wait for a second before next print
}
