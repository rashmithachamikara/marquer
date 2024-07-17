//Scan keypad Columns and Rows to get any avialble input from keypad

void keypadSetup() {
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

//Print keypad input if available
void printKeyLoop() {
  char key = getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
  }
}
