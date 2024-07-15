//Sample command list
String command = "forward 200 left 100 left 200 left 100";
String lastWord;

int startIndex = 0;
int spaceIndex = command.indexOf(' ');

void setup(){
  Serial.begin(115200);
  // Wait a moment for serial communication to stabilize
  delay(100);
}

void loop(){
  // Iterate through the string and print each word
  if (spaceIndex != -1) {
    String word = command.substring(startIndex, spaceIndex);
    //Serial.println(word);

    // Print the last word
    lastWord = command.substring(startIndex);
    //Serial.println(lastWord);
  }

  if (Serial.available() > 0) {  // Check if there is any serial input available
    String input = Serial.readStringUntil('\n');  // Read the input until newline character
    if (input = "i") {
      // Move to the next word
      startIndex = spaceIndex + 1;
      spaceIndex = command.indexOf(' ', startIndex);
    }
  }

  Serial.println(word);

  delay(100);
}