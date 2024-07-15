// Vector to hold commandList
std::vector<String> commandList;
int currentStep = 0;
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
    currentStep = 0; // Reset the step counter
    totalSteps = commandList.size(); // Update the total steps
    Serial.println("New command set: " + cmd);
    WebPrintln("New command set: " + cmd);
}

// Function to execute the next command.
void nextCommand() {
    if (currentStep < commandList.size()) {
        String word = commandList[currentStep];
        // Process the word
        Serial.println(word);
        runCurrentCommand(word);
        currentStep++;
    } else {
        Serial.println("Commandlist complete! No more steps.");
        WebPrintln("Commandlist complete! No more steps.");
        executingCommandList = false;
    }
}

void runCurrentCommand(String command){
  delay(3000);
  Serial.println("Command:'" + command + "'passed to input handler!");
  WebPrintln("Command:'" + command + "'passed to input handler!");
  handleInput(command);
}