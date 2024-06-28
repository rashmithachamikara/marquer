#include <Wire.h>
#include <PCF8574.h>

#define KEYPAD_ADDRESS 0x20

PCF8574 keypad(KEYPAD_ADDRESS);

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define pin numbers corresponding to PCF8574T pins
byte rowPins[ROWS] = {0, 1, 2, 3}; // P0 to P3
byte colPins[COLS] = {4, 5, 6, 7}; // P4 to P7

void setup() {
  Serial.begin(115200);
  Wire.begin();
  keypad.begin();
  
  // Initialize the row pins as INPUT and column pins as OUTPUT
  for (byte i = 0; i < ROWS; i++) {
    // Pin mode is not set explicitly, just ensure default state is HIGH (not pressed)
    keypad.write(rowPins[i], HIGH);
  }
  for (byte i = 0; i < COLS; i++) {
    // Set columns as HIGH (inactive state)
    keypad.write(colPins[i], HIGH);
  }
}

char getKey() {
  for (byte c = 0; c < COLS; c++) {
    // Set the current column to LOW
    keypad.write(colPins[c], LOW);
    for (byte r = 0; r < ROWS; r++) {
      if (keypad.read(rowPins[r]) == LOW) {
        // Wait for key release
        while (keypad.read(rowPins[r]) == LOW);
        // Reset the column to HIGH
        keypad.write(colPins[c], HIGH);
        return keys[r][c];
      }
    }
    // Reset the column to HIGH
    keypad.write(colPins[c], HIGH);
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
