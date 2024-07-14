//Scan keypad Columns and Rows to get any avialble input from keypad
//Made for 3x4 plastic matrix keypad

#include <Wire.h>
#include <PCF8574.h>

#define KEYPAD_ADDRESS 0x20

PCF8574 keypad(KEYPAD_ADDRESS);

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Define pin numbers corresponding to PCF8574T pins
byte rowPins[ROWS] = {1, 6, 5, 3}; // P0 to P3
byte colPins[COLS] = {2, 0, 4};    // P4 to P6

// Variables for debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50 milliseconds debounce delay

// Store the previous state of each key
bool previousKeyState[ROWS][COLS] = {false};

void setup() {
  Serial.begin(115200);
  Wire.begin();
  keypad.begin();
  
  // Initialize the row pins as INPUT and column pins as OUTPUT
  for (byte i = 0; i < ROWS; i++) {
    keypad.write(rowPins[i], HIGH); // Ensure default state is HIGH (not pressed)
  }
  for (byte i = 0; i < COLS; i++) {
    keypad.write(colPins[i], HIGH); // Set columns as HIGH (inactive state)
  }
}

char getKey() {
  unsigned long currentTime = millis();
  
  for (byte c = 0; c < COLS; c++) {
    keypad.write(colPins[c], LOW); // Set the current column to LOW
    
    for (byte r = 0; r < ROWS; r++) {
      bool currentState = (keypad.read(rowPins[r]) == LOW);
      
      if (currentState != previousKeyState[r][c]) { // Key state has changed
        if (currentState) { // Key is pressed
          if ((currentTime - lastDebounceTime) > debounceDelay) { // Debounce
            lastDebounceTime = currentTime;
            previousKeyState[r][c] = currentState; // Update previous key state
            keypad.write(colPins[c], HIGH); // Reset the column to HIGH
            return keys[r][c]; // Return the pressed key
          }
        }
        previousKeyState[r][c] = currentState; // Update previous key state
      }
    }
    keypad.write(colPins[c], HIGH); // Reset the column to HIGH
  }
  return '\0'; // No key pressed
}

void loop() {
  char key = getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }
}
