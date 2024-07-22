void IRAM_ATTR echoISR() {
  if (digitalRead(ECHO_PIN) == HIGH) {
    startTime = micros();
  } else {
    echoTime = micros() - startTime;
    measuring = false;
  }
}

void measureDistance() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  measuring = true;
}

void ultrasonicSetup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  // Set up the ultrasonicTicker to call measureDistance every second
  ultrasonicTicker.attach(1, measureDistance);
}

void ultrasonicLoop() {
  if (!measuring) {
    ultrasonicDistance = echoTime * 0.034 / 2; // Duration * SoundSpeed / 2
  }

  if(ultrasonicDistance<20 && ultrasonicDistance>1){
    if(!obstructionWarned){
      Serial.println("Obstruction detected!");
      obstructionWarned = true;
      handleInput("BREAKALL");
    }
  }
  
  if(obstructionWarned){
    if(ultrasonicDistance>20){
      obstructionWarned = false;
    }
  }

}