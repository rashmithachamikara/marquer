// Vector to hold commandList
std::vector<String> commandList;
int currentStep = 0;
int nextStep = 0;
int totalSteps = 0; // Variable to store the total number of steps

// Function to set the command list from a given command string.
void setCommandList(const String& cmd) {
  commandList.clear(); // Clear the current vector
  int start = 0;
  int end = cmd.indexOf(' ');
  while (end != -1) {
      commandList.push_back(cmd.substring(start, end));
      start = end + 1;
      end = cmd.indexOf(' ', start);
  }
  commandList.push_back(cmd.substring(start));
  nextStep = 0; // Reset the step counter
  totalSteps = commandList.size(); // Update the total steps
  Serial.println("New command set: " + cmd);
  WebPrintln("New command set: " + cmd);
}

// Function to execute the next command.
void nextCommand() {
  if (nextStep < commandList.size()) {
      String word = commandList[nextStep];
      // Process the word
      Serial.println(word);
      ticker.once_ms(2000, std::bind(runCurrentCommand, word));
      nextStep++;
      currentStep = (nextStep > 0) ? nextStep-1 : nextStep;
  } else {
      commandListEndTime = millis();
      commandListExecutionTime = commandListStartTime-commandListEndTime;
      executingCommandList = false;
      Serial.println("Commandlist complete! No more steps.");
      //Change UI Page
      uiPage = 6;
      staticContentDrawn = false;
      ui_drawTaskReportPage();
  }
}

void runCurrentCommand(String command){
  ledBlink(3);
  Serial.println("Command [" + String(nextStep) +"]:'" + command + "'passed to input handler!");
  WebPrintln("Command [" + String(nextStep) +"]:'" + command + "'passed to input handler!");
  handleInput(command);
}