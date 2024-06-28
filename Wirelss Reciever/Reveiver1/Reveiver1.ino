#include <Arduino.h>

// Define the pins connected to the receiver module
const int RECEIVER_PIN = 19;  // GPIO pin number on ESP32

void setup() {
  Serial.begin(115200);
  pinMode(RECEIVER_PIN, INPUT);
}

void loop() {
  // Read the state of the receiver pin
  int receiverState = digitalRead(RECEIVER_PIN);

  // Output the received state to serial monitor
  Serial.print("Receiver state: ");
  Serial.println(receiverState);

  Serial.println(receiverState);
  

  delay(100);  // Adjust delay as needed for your application
}
