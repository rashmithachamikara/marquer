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

void ui_drawInProgressPage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "In Progress" header
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as Preparing header
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.drawString("In Progress", 160, 30);  // Draw string in the middle of the screen

    // Draw Current task and Progress
    int currentTask = 3;  // Example value for current task
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans12pt7b);  // Set font to a bit large font
    tft.drawString("Current task: " + String(currentTask), 160, 130);  // Draw current task in the middle
    tft.drawString("Progress", 160, 170);  // Draw Progress in the middle

    // Draw "Press # to pause"
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.drawString("Press # to pause", 160, 300);  // Draw string in the middle bottom

    staticContentDrawn = true;  // Set static content drawn to true
  }

  //=== Animations ===
  // Draw progress bar
  int progress = 75;  // Example value for progress
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Progress", 160, 200);  // Draw string in the middle
  tft.drawRect(50, 220, 220, 20, TFT_BLACK);  // Draw rectangle in the bottom 
  tft.fillRect(50, 220, progress * 2.2, 20, TFT_BLUE);  // Fill screen with yellow
}

void ui_drawProcessingPage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Processing" header
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as In Progress header
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.drawString("Processing", 160, 30);  // Draw string in the middle of the screen

    // Draw Current step and Progress
    int currentStep = 4;  // Example value for current step
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans12pt7b);  // Set font to a bit large font
    tft.drawString("Current step: " + String(currentStep), 160, 130);  // Draw current step in the middle
    tft.drawString("Progress", 160, 170);  // Draw Progress in the middle

    // Draw "Press # to stop"
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.drawString("Press # to stop", 160, 300);  // Draw string in the middle bottom

    staticContentDrawn = true;  // Set static content drawn to true
  }

  //=== Animations ===
  // Draw progress bar
  int progress = 90;  // Example value for progress
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Progress", 160, 200);  // Draw string in the middle
  tft.drawRect(50, 220, 220, 20, TFT_BLACK);  // Draw rectangle in the bottom 
  tft.fillRect(50, 220, progress * 2.2, 20, TFT_BLUE);  // Fill screen with yellow
}

void ui_drawTaskReportPage() {
  // === Static Content ===
  if (staticContentDrawn == false) {
    tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

    // Draw "Task Report" header
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as Processing header
    tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
    tft.drawString("Task Report", 160, 30);  // Draw string in the middle of the screen

    // Draw Task summary and Report
    int taskSummary = 5;  // Example value for task summary
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
    tft.setFreeFont(&FreeSans12pt7b);  // Set font to a bit large font
    tft.drawString("Task summary: " + String(taskSummary), 160, 130);  // Draw task summary in the middle
    tft.drawString("Report", 160, 170);  // Draw report in the middle

    // Draw "Press * to go back" (small grey text)
    tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
    tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
    tft.drawString("Press * to go back", 160, 300);  // Draw string in the middle bottom

    staticContentDrawn = true;  // Set static content drawn to true
  }

  //=== Animations ===
  // Draw task details
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Task details:", 20, 200);  // Draw string
  tft.drawString("- Detail 1", 40, 230);  // Draw string
  tft.drawString("- Detail 2", 40, 250);  // Draw string
  tft.drawString("- Detail 3", 40, 270);  // Draw string
}
