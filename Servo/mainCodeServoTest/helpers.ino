//Print debug infomration to Serial monitor and the Marquer web
// void debugPrints(){
  
// }


//Change speed based on input
void handleInput(String input) {
  input.trim(); // Remove any extra whitespace
  input.toUpperCase();

  if (input.startsWith("A")) {
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
  } else if (input.startsWith("C")) {
    int speed = input.substring(1).toInt();
    speed = constrain(speed, 0, 255); // Constrain speed to 0-255
    speedA = speed;
    speedB = speed;
    Serial.print("Speed of both Motors A and B set to: ");
    Serial.println(speed);
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
  } else if (input.startsWith("DA")) {
    reverseA = !reverseA; // Toggle direction for Motor A
    Serial.print("Direction of Motor A set to: ");
    Serial.println(reverseA ? "Reverse" : "Forward");
  } else if (input.startsWith("DB")) {
    reverseB = !reverseB; // Toggle direction for Motor B
    Serial.print("Direction of Motor B set to: ");
    Serial.println(reverseB ? "Reverse" : "Forward");
  } else if (input.startsWith("G")) {
    targetDistance = input.substring(1).toDouble();
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
  } else if (input.startsWith("S")) {
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
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
  }
  delay(100);
  digitalWrite(LED,LOW);
}
