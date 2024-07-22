/*
Contain all L298n, MPU9250, PID and related functions
*/


// ========== MPU Calculations ==========
//Calculate yaw and store in yaw variable
void calculateYaw(){
  //Get MPU9250 data if new data is available
  if (imu.Read()) {
    //Get gyroscope z-axis data and correct for bias
    double gzRound = round((imu.gyro_z_radps() - gyroBiasZ) * 100.00) / 100.00; //With rounding
    double gz = (gzRound) * (180.0 / PI); //Convert Radians per second to degrees per second

    // Calculate Delta Time (dt)
    unsigned long currentTimeMpu = millis();
    double dtMpu = (currentTimeMpu - lastTimeMpu) / 1000.0;
    lastTimeMpu = currentTimeMpu;

    // Integrate gyroscope data for yaw
    yaw += gz * dtMpu;
  }
}

//Run Move Straight PID algorithm and set global speed values
void moveStraightPid(){
  dt = (currentTime - lastTime) / 1000.0; // Convert to seconds. Comment to use constant time

  // PID calculations for yaw control
  double error = desiredYaw - yaw;
  integral += error * dt;
  double derivative = (error - lastError) / dt;

  double pidOutput = Kp * error + Ki * integral + Kd * derivative;

  //Change PID for reverse
  if (reverseA && reverseB) {
    pidOutput *= -1;
  }

  //Only apply PID if not turning
  if (turning == 0){
    // Adjust motor speeds based on PID output, if speed>20
    if (speedA>20 && speedB>20) {
      speedA = constrain(speedA - pidOutput, minPidSpeed, maxPidSpeed);
      speedB = constrain(speedB + pidOutput, minPidSpeed, maxPidSpeed);
    }
  }

  lastError = error;   
  lastTime = currentTime;
  lastPidUpdateTime = currentTime;
  //WebPrintln("Hit!"+String(currentTime));
}


// ========== Motor applications ==========

// Control motor direction and speed
void setSpeedAndDir(){
  if (reverseA) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  if (reverseB) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}


//Turn the robot to a specific orientation
void turn() {
  bool turned = false;
  //Direction 0 - right. 1 - left
  Serial.println("Hit turn() function"); 

  // For testing, moved to global scope
  // double turnError;
  // double turnDerivative;
  // double turnPidOutput;

  double currentTime = millis();

  if(currentTime-lastTurnPidTime >= 50)
  {
    //Turn PID
    dt = (currentTime - lastTurnPidTime)/1000; //Convert to seconds. Otherwise, values are abnormally large!

    turnError = abs(turnTargetYaw - yaw);
    turnIntegral += turnError * dt; 
    turnDerivative = (turnError - lastTurnError) / dt;

    //Constrain the turn integral
    turnIntegral = constrain(turnIntegral, -200, 200);

    turnPidOutput = (turnKp * turnError) + (turnKi * turnIntegral) + (turnKd * turnDerivative);
    lastTurnError = turnError;

    lastTurnPidTime = currentTime;
  }
  

  //Set turn direction. Yaw should be 0 at the beginning of the turn
  if (turnTargetYaw > yaw) {
    turnDirection = 0; //Right
  } else {
    turnDirection = 1; //Left
  }

  //Anti Motor Stall. Pause and give a massive speed suddenly if integral is too much (which means it stayed in once yaw for long)
  if (0 && turnIntegral >= 180) { //0 Disabled for now
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    WebPrintln("Anti Stall System Activated!");
    turnIntegral = 100; //Reduce the integral a bit
    delay(100); //Wait a bit to recover
  }

  //Turn Right
  if (turnDirection == 0) {
    //Configure right motor to reverse
    reverseA = true;
    reverseB = false;
  }
  //Turn left
  else if (turnDirection == 1) {
    reverseA = false;
    reverseB = true;
  }

  //Apply power
  if(!checkingOvershoot){
    speedA = constrain(turnBaseSpeed + turnPidOutput, minTurnSpeed, maxTurnSpeed);
    speedB = constrain(turnBaseSpeed + turnPidOutput, minTurnSpeed, maxTurnSpeed);
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
  }
  //In overshoot mode, apply a constant power
  if(overshootMode){
    speedA = constrain(turnBaseSpeed*1.4 , 80, 180);
    speedB = constrain(turnBaseSpeed*1.4, 80, 180);
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
  }

  //Check if target Yaw reached
  if (yaw >= (turnTargetYaw - turnErrorMargin) && yaw <= (turnTargetYaw + turnErrorMargin)) {  
    turned = true;
  } else {
    turned = false;
  }

  if (turned && !checkingOvershoot) {
    overshootMode = false; //Set the overshoot mode to false if not set yet
    //Active break
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    delay(10);
    //Halt after turning
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    reverseA = false;
    reverseB = false;
    //yaw = 0;
    turnIntegral = 0;
    lastTurnError = 0;

    //Start checking for overshoot after some time
    turnCompletionTime = millis();
    checkingOvershoot = true;
  }

  if (checkingOvershoot) {
    //Check for overshoot after 1 second
    if (millis() - turnCompletionTime >= 1000) {
      if (yaw > turnTargetYaw + overshootMargin || yaw < turnTargetYaw - overshootMargin) {
        overshootMode = true;
        Serial.println("Overshoot detected!");
        WebPrintln("Target yaw: " + String(turnTargetYaw));
        WebPrintln("Lower margin: " + String(turnTargetYaw-overshootMargin));
        WebPrintln("Upper margin: " + String(turnTargetYaw+overshootMargin));
        WebPrintln("Current yaw: " + String(yaw));
        WebPrintln("Significant Overshoot Detected. Retrying!");
      } else {
        Serial.println("No considerable overshoot.");
        WebPrintln("No considerable overshoot.");
        //Finalize turning
        Serial.println("Turned!");
        WebPrintln("Turned!\nTurned yaw is: " + String(yaw));
        //Halt after turning
        speedA = 0;
        speedB = 0;
        analogWrite(ENA, speedA);
        analogWrite(ENB, speedB);
        //Release any break
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        reverseA = false;
        reverseB = false;
        //Reset Variables
        turnIntegral = 0;
        lastTurnError = 0;
        overshootMode = false;
        turning = 0;
        //Reverse distance moving is set when correcting turn gap
        if (executingCommandList && !correctingStopRotation) {
          nextCommand();
        }
        if (correctingStopRotation) {
          correctingStopRotation = false;
          correctingTurnGap = true;
        }
      }
      checkingOvershoot = false; // Stop checking after the first check
    }
  }
}

void manualTurn() {
  //Turn Right
  if (manualTurnDirection == 0) {
    //Configure right motor to reverse
    reverseA = true;
    reverseB = false;
  }
  //Turn left
  else if (manualTurnDirection == 1) {
    reverseA = false;
    reverseB = true;
  }

  //Apply power
  speedA = manualTurnSpeed;
  speedB = manualTurnSpeed;
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}

