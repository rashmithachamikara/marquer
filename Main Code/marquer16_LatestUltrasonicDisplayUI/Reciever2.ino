// Function to get wireless key
char getWirelessKey() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < 4; i++) {
    int currentState = pcf8574_reciever.read(receiverPins[i]);
    
    // If the state has changed and debounce time has passed
    if (currentState != prevStates[i] && (currentTime - lastRecieverDebounceTime) > recieverDebounceDelay) {
      lastRecieverDebounceTime = currentTime;
      
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

void recieverSetup() {
  // Initialize the PCF8574
  pcf8574_reciever.begin();
  
  Serial.println("Receiver Initialized");
}

void recieverLoop() {
  char wirelessKey = getWirelessKey();

  if (wirelessKey) {
    Serial.print("Key pressed: ");
    Serial.println(wirelessKey);

    switch(wirelessKey){
      case 'A':
        break;
      case 'B':
        handleInput("BREAKALL");
        break;
      case 'C':
        break;
      case 'D':
        break;
      default:
        break;
    }
  }

}
