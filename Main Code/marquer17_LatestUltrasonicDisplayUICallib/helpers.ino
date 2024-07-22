//Print debug infomration to Serial monitor and the Marquer web
// void debugPrints(){
  
// }


//Change speed based on input
void handleInput(String input) {
  input.trim(); // Remove any extra whitespace
  input.toUpperCase();

  if (input.startsWith("BREAKALL")) {
    Serial.println("Breaking All. Aborting All");
    WebPrintln("Breaking All. Aborting All");
    //Active break
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    delay(50);
    //Halt after turning
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    reverseA = false;
    reverseB = false;
    executingCommandList = false;
    turning = 0;
    checkingOvershoot = false;
    distanceMoving = 0;
    correctingTurnGap = false; 
    correctingStopRotation = false; 
    penUp();
  } else if (input.startsWith("A")) {
    speedA = input.substring(1).toInt();
    speedA = constrain(speedA, 0, 255); // Constrain speed to 0-255
    //turnSpeedA = speedA;
    Serial.print("Speed of Motor A set to: ");
    Serial.println(speedA);
  } else if (input.startsWith("B")) {
    speedB = input.substring(1).toInt();
    speedB = constrain(speedB, 0, 255); // Constrain speed to 0-255
    //turnSpeedB = speedB;
    Serial.print("Speed of Motor B set to: ");
    Serial.println(speedB);
  } else if (input.startsWith("CMDR")) {
    Serial.print("CommandList Execution Started!");
    WebPrintln("CommandList Execution Started!");
    commandListStartTime = millis();
    executingCommandList = true;
    nextCommand();
  } else if (input.startsWith("CMDH")) {
    executingCommandList = false;
    Serial.println("CommandList Execution Halted!");
    WebPrintln("CommandList Execution Halted!");
  } else if (input.startsWith("CMDN")) {
    Serial.println("Force Next Command");
    WebPrintln("Force Next Command");
    handleInput("BREAKALL");
    nextCommand();
  } else if (input.startsWith("CMD")) {
    String newCommand = input.substring(3);
    Serial.println("New command recieved");
    WebPrintln("New command recieved");
    setCommandList(newCommand);
  }  else if (input.startsWith("C")) {
    int speed = input.substring(1).toInt();
    speed = constrain(speed, 0, 255); // Constrain speed to 0-255
    speedA = speed;
    speedB = speed;
    Serial.print("Speed of both Motors A and B set to: ");
    Serial.println(speed);
  } else if (input.startsWith("DA")) {
    reverseA = !reverseA; // Toggle direction for Motor A
    Serial.print("Direction of Motor A set to: ");
    Serial.println(reverseA ? "Reverse" : "Forward");
  } else if (input.startsWith("DB")) {
    reverseB = !reverseB; // Toggle direction for Motor B
    Serial.print("Direction of Motor B set to: ");
    Serial.println(reverseB ? "Reverse" : "Forward");
  } else if (input.startsWith("P")) {
    Kp = input.substring(1).toDouble();
    turnKp = input.substring(1).toDouble();
    Serial.print("Kp/turn Set to: ");
    Serial.println(Kp);
    WebPrintln("Kp/turnKp Set to: " + String(Kp));
  } else if (input.startsWith("I")) {
    Ki = input.substring(1).toDouble();
    turnKi = input.substring(1).toDouble();
    Serial.print("Ki Set to: ");
    Serial.println(Ki);
    WebPrintln("Ki/turnKi Set to: " + String(Ki));
  } else if (input.startsWith("D")) {
    Kd = input.substring(1).toDouble();
    turnKd = input.substring(1).toDouble();
    Serial.print("Kd Set to: ");
    Serial.println(Kd);
    WebPrintln("Kd/turnKd Set to: " + String(Kd));
  } else if (input.startsWith("M")) {
    turnErrorMargin = input.substring(1).toDouble();
    Serial.print("Turn error Margin Set to: ");
    Serial.println(turnErrorMargin);
    WebPrintln("Turn error Margin Set to: " + String(turnErrorMargin));
  } else if (input.startsWith("T")) {
    //Stops before turning
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    //Reset Yaw and Set turnTargetYaw
    turnTargetYaw = input.substring(1).toDouble();
    yaw = 0;
    //turn(); //No need. Will be called in the main loop
    if(executingCommandList && !correctingStopRotation){
      turnTargetYaw += yawCorrection;
      yawCorrection = 0;
    }
    turning = 1;
  } else if (input.startsWith("R")) {
    //Encoder
    encoder1Count = 0;
    encoder2Count = 0;
    lastEncoder1Count = 0;
    lastEncoder2Count = 0;
    //PID
    yaw = 0;
    integral = 0;
    lastError = 0;
  } else if (input.startsWith("G")) {
    targetDistance = input.substring(1).toDouble();
    targetDistance -= distanceMoveOvershoot;
    //Reset stuff
    encoder1Count = 0;
    encoder2Count = 0;
    lastEncoder1Count = 0;
    lastEncoder2Count = 0;
    yaw = 0;
    integral = 0;
    lastError = 0;
    wheelDistance = 0;
    WebPrintln("Start moving a distance of "+String(targetDistance)+"cm");
    distanceMoving = true;
  } else if (input.startsWith("SU")) {
    WebPrintln("SU Hit");
    penUp();
  } else if (input.startsWith("SD")) {
    penDown();
  }else if (input.startsWith("S")) {
    int angle = input.substring(1).toInt();
    servoTurnTo(angle);
    Serial.println("servoTurnTo() called with " + String(angle));
  } 

  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}


//Blink built-in LED a given amount of times
void ledBlink(int times){
  for (int i=0; i<=times; i++){
    delay(100);
    //digitalWrite(LED,LOW);
    delay(100);
    //digitalWrite(LED,HIGH);
  }
  delay(100);
  //digitalWrite(LED,LOW);
}

//Calibration

//Callibratable values
/*
PID
double Kp = 0.6; //2.5;
double Ki = 0.6; //0.5;
double Kd = 0.6; //1;
int minPidSpeed = 50; //60;
int maxPidSpeed = 170; //180;

//Turning.
double turnKp = 0.4;
double turnKi = 0.4;
double turnKd = 1.5;
int minTurnSpeed = 60;
int maxTurnSpeed = 150;
int turnBaseSpeed = 60;
double turnErrorMargin = 1;
double overshootMargin = 2;

//DistanceMove
double distanceMoveOvershoot = 3; //Found thorugh repeated experiment
int movingBaseSpeed = 90;

//Remote
int manualTurnSpeed = 100;
int wifiMoveSpeed = 100;

MPU9250
double gyroBiasZ;
*/

void handleCallib(String input){
  if(input.startsWith("Get")){
    WebPrintln("Callib_data:" +
          String(Kp) + "," + String(Ki) + "," +
          String(Kd) + "," + String(minPidSpeed) + "," +
          String(maxPidSpeed) + "," + String(turnKp) + "," +
          String(turnKi) + "," + String(turnKd) + "," +
          String(minTurnSpeed) + "," + String(maxTurnSpeed) + "," +
          String(turnBaseSpeed) + "," + String(turnErrorMargin) + "," +
          String(overshootMargin) + "," + String(distanceMoveOvershoot) + "," +
          String(movingBaseSpeed) + "," + String(manualTurnSpeed) + "," +
          String(wifiMoveSpeed) + "," + String(gyroBiasZ) + "," + String(pidInterval));
  }
  if(input.startsWith("Set:")){
    Serial.println(input.substring(4));

    String inputString = input.substring(4);

    // Split the string
    int index = 0;
    String values[19]; // Array to hold the split values
    while (inputString.length() > 0) {
      int commaIndex = inputString.indexOf(',');
      if (commaIndex == -1) { // Last value
        values[index++] = inputString;
        break;
      }
      values[index++] = inputString.substring(0, commaIndex);
      inputString = inputString.substring(commaIndex + 1);
    }

    // Assign values to variables
    Kp = values[0].toDouble();
    Ki = values[1].toDouble();
    Kd = values[2].toDouble();
    minPidSpeed = values[3].toInt();
    maxPidSpeed = values[4].toInt();
    
    turnKp = values[5].toDouble();
    turnKi = values[6].toDouble();
    turnKd = values[7].toDouble();
    minTurnSpeed = values[8].toInt();
    maxTurnSpeed = values[9].toInt();
    turnBaseSpeed = values[10].toInt();
    turnErrorMargin = values[11].toDouble();
    overshootMargin = values[12].toDouble();
    pidInterval = values[18].toDouble();
    
    distanceMoveOvershoot = values[13].toDouble();
    movingBaseSpeed = values[14].toInt();
    
    manualTurnSpeed = values[15].toInt();
    wifiMoveSpeed = values[16].toInt();
    
    gyroBiasZ = values[17].toDouble();

    // Print values to verify
    Serial.println(Kp);
    Serial.println(Ki);
    Serial.println(Kd);
    Serial.println(minPidSpeed);
    Serial.println(maxPidSpeed);
    Serial.println(turnKp);
    Serial.println(turnKi);
    Serial.println(turnKd);
    Serial.println(minTurnSpeed);
    Serial.println(maxTurnSpeed);
    Serial.println("Turn Base Speed "+ String(turnBaseSpeed));
    Serial.println(turnErrorMargin);
    Serial.println(overshootMargin);
    Serial.println(distanceMoveOvershoot);
    Serial.println(movingBaseSpeed);
    Serial.println(manualTurnSpeed);
    Serial.println(wifiMoveSpeed);
    Serial.println(gyroBiasZ);
    Serial.println(pidInterval);
  }
}
