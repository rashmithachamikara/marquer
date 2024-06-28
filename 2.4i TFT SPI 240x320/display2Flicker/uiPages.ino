void homeScreen() {
  tft.fillScreen(TFT_WHITE); // Fill the screen with white color

  // Draw "Greetings"
  tft.setTextColor(TFT_BLUE, TFT_WHITE);  // Set text color to blue with white background
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.setFreeFont(&FreeSansBold24pt7b);  // Set font to a large bold font
  tft.drawString("Greetings", 120, 40);  // Draw string in the middle of the screen

  // Draw "Select mode"
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to a larger font
  tft.drawString("Select mode", 120, 100);  // Draw string in the middle of the screen

  // Draw mode options
  tft.setFreeFont(&FreeSans12pt7b);  // Set font to a larger font
  tft.drawString("1 Manual mode", 120, 140);  // Draw string
  tft.drawString("2 Preset mode", 120, 180);  // Draw string

  // Draw App connection status
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a larger font
  bool connected = false;  // Change this to true if connected
  if (connected) {
    tft.setTextColor(TFT_GREEN, TFT_WHITE);  // Set text color to green with white background
    tft.drawString("App connection: Connected", 120, 230, 2);  // Draw string
  } else {
    tft.setTextColor(TFT_RED, TFT_WHITE);  // Set text color to red with white background
    tft.drawString("App connection: Not connected", 120, 230, 2);  // Draw string
  }

  // Draw IP Address
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.drawString("IP Address: 192.168.5.5", 120, 250);  // Draw string

  // Draw small gray text
  tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to gray with white background
  tft.drawString("Marquer Drawing Systems v1.0", 120, 300, 2);  // Draw string

}

void drawManualModePage() {
  tft.startWrite();  // Begin a write transaction
  // Draw "Manual Mode"
  tft.fillScreen(TFT_WHITE); // Fill the screen with white color

  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as Select mode
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Manual Mode", 120, 40);  // Draw string in the middle of the screen

  // Draw instructions
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextColor(TFT_BLACK);  // Set text color to black with white background
  tft.drawString("Instructions", 120, 80);  // Draw string
  tft.setTextDatum(ML_DATUM);  // Set text datum to mid left
  tft.fillCircle(60, 120, 18, TFT_ORANGE);  // Draw filled circle for A
  tft.drawString("A    Forward", 50, 120);  // Draw string
  tft.fillCircle(60, 160, 18, TFT_ORANGE);  // Draw filled circle for B
  tft.drawString("B    Back", 50, 160);  // Draw string
  tft.fillCircle(60, 200, 18, TFT_ORANGE);  // Draw filled circle for C
  tft.drawString("C    Left", 50, 200);  // Draw string
  tft.fillCircle(60, 240, 18, TFT_ORANGE);  // Draw filled circle for D
  tft.drawString("D    Right", 50, 240);  // Draw string

  // Draw Distance
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  int distance = 100;  // Example value for distance
  tft.setTextColor(TFT_BLACK);  // Set text color to black with white background
  tft.drawString("Distance - " + String(distance), 120, 280);  // Draw string with distance variable

  // Draw "Press * to go back" (small grey text)
  tft.setTextColor(TFT_GREY);  // Set text color to gray with white background
  tft.drawString("Press * to go back", 120, 300, 2);  // Draw string

  tft.endWrite();  // End the write transaction
}