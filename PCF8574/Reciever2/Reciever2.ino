#include <Wire.h>
#include <PCF8574.h>

// Define I2C address for the PCF8574
#define PCF8574_ADDRESS 0x21

// Create a PCF8574 instance
PCF8574 pcf8574_reciever(PCF8574_ADDRESS);

// Define pin numbers for the PCF8574 (receiver)
byte receiverPins[4] = {0, 1, 2, 3}; // P0 to P3 of PCF8574

// Variables to store inputs
char recieverKeys[4] = {'B', 'D', 'A', 'C'}; // Assuming LOW is the default state when nothing is pressed

// Variables to store previous states
int prevStates[4] = {LOW, LOW, LOW, LOW}; // Assuming LOW is the default state when nothing is pressed

// Debounce time
const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;

// Function to get wireless key
char getWirelessKey() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < 4; i++) {
    int currentState = pcf8574_reciever.read(receiverPins[i]);
    
    // If the state has changed and debounce time has passed
    if (currentState != prevStates[i] && (currentTime - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = currentTime;
      
      // Update the previous state
      prevStates[i] = currentState;
      
      // Return corresponding key for the pin
      if (currentState == HIGH) { //HIGH indicates a key press
        return recieverKeys[i]; // Returns 'A', 'B', 'C', or 'D' based on the pin
      }
    }
  }
  return '\0'; // No key pressed
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize I2C communication
  Wire.begin();
  
  // Initialize the PCF8574
  pcf8574_reciever.begin();
  
  Serial.println("Receiver Test Initialized");
}

void loop() {
  char wirelessKey = getWirelessKey();
  if (wirelessKey) {
    Serial.print("Key pressed: ");
    Serial.println(wirelessKey);
    // LOGIC
  }

}
