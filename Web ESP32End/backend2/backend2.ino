//#include <vector>
//#include <string>

//#include <vector>
//#include <string>

// This will hold the split command words.
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
    Serial.println("New command set");
}

// Function to execute the next step.
void nextStep() {
    if (currentStep < commandList.size()) {
        String word = commandList[currentStep];
        // Process the word
        Serial.println(word);
        currentStep++;
    } else {
        Serial.println("No more steps.");
    }
}

void setup() {
    Serial.begin(115200);
    String initialCommand = "forward 200 left 100 left 200 left 100";
    setCommandList(initialCommand);
}

void loop() {
  // Call nextStep() as needed, for example, based on a button press or a timer.
  // if (/* condition to call nextStep */) {
  //     nextStep();
  // }

  if (Serial.available() > 0) {  // Check if there is any serial input available
    String input = Serial.readStringUntil('\n');  // Read the input until newline character
    if (input == "i") {
      nextStep();
    } else if (input == "a"){
      setCommandList("Little Brown Fox Jump Over Lazy Dog 100 200");
    }
  }

}
