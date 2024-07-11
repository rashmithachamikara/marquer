#include <Wire.h>
#include <PCF8574.h>

// Define I2C addresses for the two PCF8574 expanders
#define KEYPAD_ADDRESS_1 0x20 
#define RECEIVER_ADDRESS_2 0x21

PCF8574 pcf1(KEYPAD_ADDRESS_1);
PCF8574 pcf2(RECEIVER_ADDRESS_2);

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Define pin numbers for the first PCF8574 (keypad)
byte rowPins1[ROWS] = {0, 1, 2, 3}; // P0 to P3 of PCF8574 #1
byte colPins1[COLS] = {4, 5, 6, 7}; // P4 to P7 of PCF8574 #1

// Define pin numbers for the second PCF8574 (receiver)
byte receiverPins[4] = {0, 1, 2, 3}; // P0 to P3 of PCF8574 #2

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pcf1.begin();
  pcf2.begin();
  
  // Initialize the row pins as INPUT and column pins as OUTPUT for PCF8574 #1
  for (byte i = 0; i < ROWS; i++) {
    pcf1.write(rowPins1[i], HIGH); // Set rows HIGH (inactive state)
  }
  for (byte i = 0; i < COLS; i++) {
    pcf1.write(colPins1[i], HIGH); // Set columns HIGH (inactive state)
  }

  // Initialize the receiver pins as INPUT for PCF8574 #2
  for (byte i = 0; i < 4; i++) {
    pcf2.write(receiverPins[i], HIGH); // Set receiver pins HIGH (inactive state)
  }
}

char getKey() {
  for (byte c = 0; c < COLS; c++) {
    pcf1.write(colPins1[c], LOW); // Set the current column to LOW
    for (byte r = 0; r < ROWS; r++) {
      if (pcf1.read(rowPins1[r]) == LOW) {
        while (pcf1.read(rowPins1[r]) == LOW); // Wait for key release
        pcf1.write(colPins1[c], HIGH); // Reset the column to HIGH
        return keys[r][c];
      }
    }
    pcf1.write(colPins1[c], HIGH); // Reset the column to HIGH
  }
  return '\0'; // No key pressed
}

void checkReceiver() {
  for (byte i = 0; i < 4; i++) {
    if (pcf2.read(receiverPins[i]) == LOW) {
      Serial.print("Receiver channel ");
      Serial.print(i);
      Serial.println(" is LOW (not activated).");
    } else {
      Serial.print("Receiver channel ");
      Serial.print(i);
      Serial.println(" is HIGH (activated).");
    }
  }
}

void loop() {
  char key = getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }

  checkReceiver();
  delay(1000); // Check receiver state every second
}
