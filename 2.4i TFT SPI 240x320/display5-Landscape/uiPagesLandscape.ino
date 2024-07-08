/*

This page contains
- Functions for each UI page
- Helper functions to draw extras, animations, etc...

Note that
- Static content (that only shows once) is wrapped in "if(!staticContentDrawn){}" to avoid flickering
- Animated content should be outside "if(!staticContentDrawn){}" to be drawn each time the screen updates

*/

void drawGrid() {
  tft.setTextDatum(BL_DATUM);  // Set text datum

  // Set the color for the grid lines and text
  uint16_t gridColor = TFT_GREY;
  uint16_t textColor = TFT_BLUE;

  // Set the font size (optional)
  tft.setTextFont(1);  // Set font to font 2
  tft.setTextSize(1); // This sets the text size to the smallest

  // Draw vertical lines and numbers
  for (int x = 0; x < 320; x += 10) {
    tft.drawLine(x, 0, x, 240, gridColor);
    if (x % 50 == 0) { // Number every 50 pixels
      tft.setCursor(x, 0); // Adjust the position slightly for better alignment
      tft.setTextColor(TFT_BLUE); // Set text color 
      tft.print(x);
    }
  }

  // Draw horizontal lines and numbers
  for (int y = 0; y < 240; y += 10) {
    tft.drawLine(0, y, 320, y, gridColor);
    if (y % 50 == 0) { // Number every 50 pixels
      tft.setCursor(0, y); // Adjust the position slightly for better alignment
      tft.setTextColor(TFT_RED); // Set text color
      tft.print(y);
    }
  }
}

void ui_drawHomeScreen() {
  // === Static Content ===
  if (!staticContentDrawn) {

    tft.fillScreen(TFT_WHITE); // Fill the screen with white color

    // Draw Heading
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Marquer", 160, 20);  // Draw string in the middle of the screen

    // Draw "Select mode"
    tft.setTextDatum(TL_DATUM);  // Set text datum
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans18pt7b);  // Set font to a larger font
    tft.drawString("Select option", 10, 60);  // Draw string in the middle of the screen

    // Draw mode options
    tft.setFreeFont(&FreeSans12pt7b);  // Set font to a larger font
    tft.drawString("1. Manual mode", 10, 100);  // Draw string
    tft.drawString("2. Presets", 10, 130);  // Draw string
    tft.drawString("3. Settings", 10, 160);  // Draw string

    // Draw App connection status
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a larger font
    bool connected = false;  // Change this to true if connected
    if (connected) {
      tft.setTextColor(TFT_GREEN, TFT_WHITE);  // Set text color to green with white background
      tft.drawString("App connection: Connected", 160, 200, 2);  // Draw string
    } else {
      tft.setTextColor(TFT_RED, TFT_WHITE);  // Set text color to red with white background
      tft.drawString("App connection: Not connected", 160, 200, 2);  // Draw string
    }

    // Draw IP Address
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.drawString("IP Address: 192.168.5.5", 160, 220);  // Draw string

    // Draw small gray text
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to gray with white background
    tft.drawString("Marquer Drawing Systems v1.0", 160, 250, 2);  // Draw string
  }
}

void ui_drawManualModePage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE); // Fill the screen with white color

    // Draw Heading
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Manual Mode", 160, 20);  // Draw string in the middle of the screen

    // Draw instructions
    tft.setFreeFont(&FreeSans12pt7b);
    tft.setTextColor(TFT_BLACK);  // Set text color to black with white background
    tft.drawString("Instructions", 160, 60);  // Draw string
    tft.setTextDatum(ML_DATUM);  // Set text datum to mid left
    tft.fillCircle(30, 100, 18, TFT_ORANGE);  // Draw filled circle for A
    tft.drawString("A    Forward", 22, 100);  // Draw string
    tft.fillCircle(30, 150, 18, TFT_ORANGE);  // Draw filled circle for B
    tft.drawString("B    Back", 22, 150);  // Draw string
    tft.fillCircle(190, 100, 18, TFT_ORANGE);  // Draw filled circle for C
    tft.drawString("C    Left", 182, 100);  // Draw string
    tft.fillCircle(190, 150, 18, TFT_ORANGE);  // Draw filled circle for D
    tft.drawString("D    Right", 182, 150);  // Draw string

    // Draw "Press * to go back" (small grey text)
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setTextColor(TFT_GREY);  // Set text color to gray with white background
    tft.drawString("Press * to go back", 160, 230, 2);  // Draw string

    staticContentDrawn = true;  // Set static content drawn to true
  }

  //=== Animations ===
  // Draw Distance
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.setFreeFont(&FreeSans12pt7b);
  int distance = 100;  // Example value for distance
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.drawString("Distance - " + String(distance), 160, 190);  // Draw string with distance variable
}

void ui_drawPresetsPage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Presets"
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Presets", 160, 20);  // Draw string in the middle of the screen

    // Initialize the array of example preset strings for testing
    String presets[9];
    for (int i = 0; i < 9; i++) {
      presets[i] = "Empty";
    }

    // Draw the list of presets
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setTextFont(2);  // Set font to font 2
    tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
    for (int i = 0; i < 9; i++) {
      tft.drawString(String(i + 1) + " " + presets[i], 15, 50 + i * 20);  // Print each preset with numbering
    }

    // Draw selected preset
    int selectedPreset = 2;  // Example value for selected preset
    tft.drawString("Selected preset: " + String(selectedPreset), 180, 100);  // Draw selected preset

    // Draw "Press # to start"
    tft.setTextColor(TFT_GREEN, TFT_WHITE);  // Set text color to grey with white background
    tft.drawString("Press # to start", 180, 120);  // Draw string

    // Draw "Press * to go back" (small grey text)
    tft.setTextColor(TFT_GREY);  // Set text color to gray with white background
    tft.drawString("Press * to go back", 180, 140, 2);  // Draw string

    staticContentDrawn = true;  // Set static content drawn to true
  }
}

void ui_drawPreparingPage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Preparing" header
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Preparing", 160, 20);  // Draw string in the middle of the screen

    // Draw Current preset and Preparing
    int currentPreset = 2;  // Example value for current preset
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans12pt7b);  // Set font to a bit large font
    tft.drawString("Current preset: " + String(currentPreset), 160, 80);  // Draw Current preset in the middle
    tft.drawString("Preparing equipment", 160, 110);  // Draw Preparing in the middle

    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.drawString("Progress", 160, 160);  // Draw string in the middle

    // Draw "Press # to cancel"
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.drawString("Press # to cancel", 160, 230, 2);  // Draw string in the middle bottom

    staticContentDrawn = true;  // Set static content drawn to true
  }

  //=== Animations ===
  // Draw progress bar
  static int progress = 0;  // Example value for progress out of 100
  if(progress>=100){
    progress = 0;
  } else {
    progress++;
  }

  tft.fillRect(50, 180, 220, 20, TFT_WHITE);  // Draw rectangle in the bottom 
  tft.drawRect(50, 180, 220, 20, TFT_BLACK);  // Draw rectangle in the bottom 
  tft.fillRect(50, 180, progress * 2.2, 20, TFT_BLUE);  // Fill screen with yellow
}

void drawSpeedometer(int x, int y, int radius, int speed) {
  tft.fillCircle(x, y, radius, TFT_WHITE);  // Draw background fill. Clear last needle
  tft.drawCircle(x, y, radius, TFT_BLACK);  // Draw outer circle
  tft.fillCircle(x, y, 2, TFT_RED);  // Draw center dot

  // Draw speed marks and labels
  tft.setTextFont(1);  // Set font to font 2
  tft.setTextSize(1); // This sets the text size to the smallest
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  for (int i = 0; i <= 100; i += 10) {
    int angle = map(i, 0, 100, -120, 120);
    int x1 = x + (radius - 10) * cos(radians(angle));
    int y1 = y + (radius - 10) * sin(radians(angle));
    int x2 = x + radius * cos(radians(angle));
    int y2 = y + radius * sin(radians(angle));
    tft.drawLine(x1, y1, x2, y2, TFT_BLACK);

    // Draw labels only for 0 and 100
    if (i == 0 || i == 50 || i == 100) {
      int labelX = x + (radius - 12) * cos(radians(angle));
      int labelY = y + (radius - 12) * sin(radians(angle));
      char label[4];  // Buffer to hold the label string
      sprintf(label, "%d", i);  // Convert the speed value to a string
      tft.drawString(label, labelX, labelY);  // Draw the speed label
    }
  }

  int angle = map(speed, 0, 100, -120, 120);  // Map speed to angle
  int x1 = x + (radius-4) * cos(radians(angle));
  int y1 = y + (radius-4) * sin(radians(angle));
  tft.drawLine(x, y, x1, y1, TFT_RED);  // Draw needle
  tft.drawCircle(x, y, radius, TFT_BLACK);  // Draw outer circle again to cover needle hits
}

void drawRotatingAnimation(int x, int y, int radius, int angle) {
  tft.fillCircle(x, y, radius, TFT_WHITE);  // Clear previous
  tft.drawCircle(x, y, radius, TFT_BLACK);  // Draw outer circle
  tft.fillCircle(x, y, 2, TFT_RED);  // Draw center dot
  
  // Draw angle marks
  for (int i = 0; i < 360; i += 30) {
    int angleMark = i - 90;
    int x1 = x + (radius - 10) * cos(radians(angleMark));
    int y1 = y + (radius - 10) * sin(radians(angleMark));
    int x2 = x + radius * cos(radians(angleMark));
    int y2 = y + radius * sin(radians(angleMark));
    tft.drawLine(x1, y1, x2, y2, TFT_BLACK);
  }
  
  // Draw rotating line
  int x1 = x + (radius-4) * cos(radians(angle - 90));
  int y1 = y + (radius-4) * sin(radians(angle - 90));
  tft.drawLine(x, y, x1, y1, TFT_RED);  // Draw rotating line
}

void ui_drawProcessingPage() {
  // Variables
  String presetName = "My Preset 1";  // Example preset name
  int currentStep = 3;  // Example current step
  int totalSteps = 5;  // Example total steps
  String currentStepAction = "Moving 100cm";  // Example current step action
  static int speed = 30;  // Speed in cm/s
  int distanceCovered = 20;  // Distance covered in cm
  int distanceTarget = 50;  // Distance target in cm
  String direction = "left";  // Direction for rotating
  String penStatus = "drawing";  // Pen status

  static int angle = 0;
  static int state = 0;

  // === Static Content ===
  if (staticContentDrawn==false){
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Processing" header
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Processing", 160, 20);  // Draw string in the middle of the screen

    // Draw preset name
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color
    tft.setTextFont(2);  // Set font to font 2
    tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
    tft.drawString("Preset name: " + presetName, 10, 50);  // Draw preset name

    // Draw "Press # to abort"
    tft.setTextDatum(MC_DATUM);  // Set text datum to top-left corner
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    //tft.setTextDatum(TL_DATUM);  // Reset text datum to top-left corner
    tft.drawString("Press # to abort", 160, 230, 2);  // Draw string at the bottom
  }
  
  //=== Animations ===
  //Adjust example variables
  if (state == 0) {
    speed += random(-6,6);  // Increment angle for rotating animation
    if (speed < 0) speed = 0;  // Reset angle after a full rotation
    if (speed > 90) speed = 90;  // Reset angle after a full rotation
  }
  if (state == 1) {
    angle += 2;  // Increment angle for rotating animation
    if (angle > 90) angle = 0;  // Reset angle after a full rotation
  }

  // Draw current step information
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Current step: " + String(currentStep) + "/" + String(totalSteps) + "         ", 10, 70);  // Draw current step info
  tft.drawString("Active task:  " + currentStepAction + "         ", 10, 90);  // Draw current step action

  if (state == 0) {
    // Draw Straight motion information
    tft.setTextColor(TFT_MAROON, TFT_WHITE);  // Set text color
    tft.drawString("Straight motion", 120, 125);  // Draw Straight motion label
    // Draw Speedometer
    drawSpeedometer(50, 162, 40, speed);  // drawSpeedometer(x, y, r, z) Draws a speedometer at (x, y) with radius r. needle at z

    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
    tft.drawString("Speed: " + String(speed) + "cm/s      ", 120, 145);  // Draw speed in text
    // Draw Distance
    tft.drawString("Distance: " + String(distanceCovered) + "/" + String(distanceTarget) + " cm      ", 120, 165);  // Draw distance
    tft.drawString("Pen Status: " + penStatus, 120, 185);  // Draw pen status
  } else if (state == 1) {
    // Draw Rotating information
    tft.setTextColor(TFT_MAROON, TFT_WHITE);  // Set text color
    tft.drawString("Rotating", 120, 125);  // Draw Rotating label
    //Draw rotation meter
    drawRotatingAnimation(50, 162, 40, angle);  // Draw rotating animation at (x, y)

    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
    tft.drawString("Angle: " + String(angle) + " degrees", 120, 145);  // Draw angle
    tft.drawString("Direction: " + direction, 120, 165);  // Draw direction
    tft.drawString("Pen Status: " + penStatus, 120, 185);  // Draw pen status
  }

}

void ui_drawTaskReportPage() {
  // Example Variables
  String presetName = "Moving 100cm";  // Example preset name
  String timeTaken = "12 min 30 sec";  // Example time taken
  String distanceTravelled = "150 cm";  // Example distance travelled
  String avgSpeed = "12.5 cm/s";  // Example average speed

  // === Static Content ===
  if (staticContentDrawn==false){
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Task Report" header
    tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.setFreeFont(&FreeSansBold18pt7b);  // Set font to a large bold font
    tft.drawString("Task report", 160, 20);  // Draw string in the middle of the screen

    // Draw preset name
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to blue with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
    tft.drawString("Preset name: " + presetName, 10, 60);  // Draw preset name

    // Draw time taken
    tft.drawString("Time taken: " + timeTaken, 10, 80);  // Draw time taken

    // Draw distance travelled
    tft.drawString("Distance travelled: " + distanceTravelled, 10, 100);  // Draw distance travelled

    // Draw average speed
    tft.drawString("Avg. Speed: " + avgSpeed, 10, 120);  // Draw average speed

    // Draw "Press * to return to menu"
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setTextDatum(MC_DATUM);  // Set text datum to top-left corner
    tft.drawString("Press * to return to main menu", 160, 220, 2);  // Draw string at the bottom
  }
  
}
