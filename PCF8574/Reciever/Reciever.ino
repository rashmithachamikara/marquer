#include <Wire.h>
#include <PCF8574.h>

// Define I2C address for the PCF8574
#define PCF8574_ADDRESS 0x21

// Create a PCF8574 instance
PCF8574 pcf8574(PCF8574_ADDRESS);

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
  int d0 = pcf8574.read(0);
  int d1 = pcf8574.read(1);
  int d2 = pcf8574.read(2);
  int d3 = pcf8574.read(3);
  int ut = pcf8574.read(4);
  
  // Print values to the Serial Monitor
  Serial.print("D0 (B): ");
  Serial.print(d0);
  Serial.print("  D1 (D): ");
  Serial.print(d1);
  Serial.print("  D2 (A): ");
  Serial.print(d2);
  Serial.print("  D3 (C): ");
  Serial.print(d3);
  Serial.print("  UT: ");
  Serial.println(ut);
  
  // Plot values on Serial Plotter (requires Arduino IDE)
  Serial.print(d0);
  Serial.print(",");
  Serial.print(d1);
  Serial.print(",");
  Serial.print(d2);
  Serial.print(",");
  Serial.print(d3);
  Serial.print(",");
  Serial.println(ut);
  
  delay(100); // Delay for stability
}
