//Encoder1 interrupts
void IRAM_ATTR handleEncoder1Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > 5){
    //Reduce count if reversing
    if(reverseA && reverseB){
      encoder1Count--;
    } else {
      encoder1Count++;
    }
    lastInterruptTime1 = interruptTime;
  }

}

//Encoder2 interrupts
void IRAM_ATTR handleEncoder2Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > 5){
    //Reduce count if reversing
    if(reverseA && reverseB){
      encoder2Count--;
    } else {
      encoder2Count++;
    }
    lastInterruptTime2 = interruptTime;
  }
}

//Do encoder caluclations including rotations and distances
void encoderCaclculations(){
  wheel1Rotations = encoder1Count/ENCODER_RESOLUTION;
  wheel2Rotations = encoder2Count/ENCODER_RESOLUTION;
  wheel1Distance = wheel1Rotations*wheelDiameter;
  wheel2Distance = wheel2Rotations*wheelDiameter;
  wheelDistance = (wheel1Distance+wheel2Distance)/2;
}