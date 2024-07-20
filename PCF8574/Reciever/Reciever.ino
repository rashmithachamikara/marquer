#include <Wire.h>
#include <PCF8574.h>

// Define I2C address for the PCF8574
#define PCF8574_ADDRESS 0x21

// Create a PCF8574 instance
PCF8574 pcf8574_reciever(PCF8574_ADDRESS);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin();
  
  // Initialize the PCF8574
  pcf8574.begin();
  

// Define pin numbers for the second PCF8574 (receiver)
byte receiverPins[4] = {0, 1, 2, 3}; // P0 to P3 of PCF8574 #2
  
  Serial.println("Receiver Test Initialized");
}

void loop() {
  // Read values from P0 to P3 (receiver channels)
  int d0 = pcf8574_reciever.read(0);
  int d1 = pcf8574_reciever.read(1);
  int d2 = pcf8574_reciever.read(2);
  int d3 = pcf8574_reciever.read(3);
  
  // Print values to the Serial Monitor
  Serial.print("D0 (B): ");
  Serial.print(d0);
  Serial.print("  D1 (D): ");
  Serial.print(d1);
  Serial.print("  D2 (A): ");
  Serial.print(d2);
  Serial.print("  D3 (C): ");
  Serial.println(d3);
  
  // Plot values on Serial Plotter (requires Arduino IDE)
  Serial.print(d0);
  Serial.print(",");
  Serial.print(d1);
  Serial.print(",");
  Serial.print(d2);
  Serial.print(",");
  Serial.println(d3);
  
  delay(100); // Delay for stability
}
