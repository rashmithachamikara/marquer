//Preferences
Preferences preferences;

void savePresets(String input){
  String jsonInput = input.substring(7);
  Serial.println("========== Saving process being ==========\nJson String: " + jsonInput);

  // Open Preferences with a namespace, e.g., "marquer"
  // NB: Namespace name is limited to 15 chars
  preferences.begin("marquer", false);

  // Parse JSON string
  JSONVar myArray = JSON.parse(jsonInput);

  // Check if the parsing succeeded
  if (JSON.typeof(myArray) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

  // Iterate over the JSON array
  for (int i = 0; i < myArray.length(); i++) {
    JSONVar item = myArray[i];
    
    // Extract values
    String name = (const char*)item["name"];
    String instructions = (const char*)item["instructions"];

    Serial.println(i);
    Serial.println(name);
    Serial.println(instructions);

    // Store values in Preferences
    String nameKey = "name" + String(i);
    String instructionsKey = "instructions" + String(i);

    preferences.putString(nameKey.c_str(), name);
    preferences.putString(instructionsKey.c_str(), instructions);
  }

  preferences.end();
  Serial.println("Data stored in preferences.");
  staticContentDrawn = false;

  delay(100);
  //loadPresets();
}

Preset* loadPresets() {
  // Open Preferences with a namespace, e.g., "marquer"
  // NB: Namespace name is limited to 15 chars
  // Set readonly to true
  preferences.begin("marquer", true);

  // Read stored values
  for (int i = 0; i < 9; i++) {
    String nameKey = "name" + String(i);
    String instructionsKey = "instructions" + String(i);

    String name = preferences.getString(nameKey.c_str(), "None");
    String instructions = preferences.getString(instructionsKey.c_str(), "S0");
    
    Serial.println("Name: " + name);
    Serial.println("Instructions: " + instructions);

    // Store in the presets array
    presets[i].name = name;
    presets[i].instructions = instructions;
  }

  preferences.end();

  return presets;
}

void setCommandListFromPreset(int presetNo){
  Preset* loadedPresets = loadPresets();
  String instructions = loadedPresets[presetNo-1].instructions;
  Serial.println("Setting commandlist with these instructions: " + instructions);
  setCommandList(instructions);
}
