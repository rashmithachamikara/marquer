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

//This is called per interval
void calculateEncoderSpeed(){
    // Ensure time interval is in seconds for correct RPS calculation
    float timeInterval = encoderSpeedInterval / 1000.0; // Convert milliseconds to seconds

    encoder1Rps = (encoder1Count - lastEncoder1Count) / (ENCODER_RESOLUTION * timeInterval); // Rotations per second
    encoder2Rps = (encoder2Count - lastEncoder2Count) / (ENCODER_RESOLUTION * timeInterval); // Rotations per second

    wheel1Speed = encoder1Rps * PI * wheelDiameterInm;
    wheel2Speed = encoder2Rps * PI * wheelDiameterInm;

    wheelSpeed = (wheel1Speed+wheel2Speed)/2;

    lastEncoder1Count = encoder1Count;
    lastEncoder2Count = encoder2Count;
}