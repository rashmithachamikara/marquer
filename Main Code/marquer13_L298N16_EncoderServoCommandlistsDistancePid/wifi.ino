void handleWifiRemoteInput(String input){
  WebPrintln(input);

  if (input.startsWith("forward_start")){
    //Set direction
    reverseA = false;
    reverseB = false;
    //Reset PID
    yaw = 0;
    integral = 0;
    lastError = 0;
    //Run forward
    handleInput("C"+String(wifiMoveSpeed));
  } else if (input.startsWith("backward_start")){
    //Set direction
    reverseA = true;
    reverseB = true;
    //Reset PID
    yaw = 0;
    integral = 0;
    lastError = 0;
    //Run forward
    handleInput("C"+String(wifiMoveSpeed));
  } else if(input.startsWith("left_start")){
    manualTurnDirection = 1;
    manualTurning = 1;
  } else if(input.startsWith("right_start")){
    manualTurnDirection = 0;
    manualTurning = 1;
  } else if(input.startsWith("stop")){
    manualTurning = 0;
    handleInput("C0");
  }
}