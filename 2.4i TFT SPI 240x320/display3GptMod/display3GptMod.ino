#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
//#include <Arduino.h>

#define TFT_GREY 0x5AEB // New colour
#define TFT_ORANGE 0xFBE0 // New colour
#define TFT_WHITE 0xFFFF // New colour
#define TFT_PURPLE 0x209c // New colour

TFT_eSPI tft = TFT_eSPI();  // Invoke library

//variables
int uiPage = 5;
unsigned int old_uiPage = uiPage;


void setup(void) {
  // Start serial communication
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("Enter a number (1, 2, or 3) to execute a function:");

  //Display init
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_WHITE);
  ui_drawProcessingPage();
  
}

void loop() {
  uiEngine();

  Serial.println("Pass");

  delay(35);
}

void uiEngine(){
  static unsigned int uiFrequency = 35; //How often should UI update 
  static unsigned long lastUpdate = 0; //A static variable inside a function retains its value between function calls
  unsigned long currentMillis = millis(); //Milliseconds spent since the boot of device

  if(old_uiPage != uiPage){
    Serial.println("Page difference!");
  }

  // Update UI Page either when interval (uiFrequency) reached or uiPage changed
  if (currentMillis - lastUpdate >= uiFrequency || old_uiPage != uiPage) {
    lastUpdate = currentMillis; //Change last update
    //Draw UI page
    tft.startWrite();
    switch (uiPage) {
        case 1: 
          uiFrequency = 10000;
          ui_drawHomeScreen();
          break;
        case 2:
          uiFrequency = 10000;
          ui_drawManualModePage();
          break;
        case 3:
          uiFrequency = 10000;
          ui_drawPresetsPage();
          break;
        case 4:
          uiFrequency = 100;
          ui_drawPreparingPage();
          break;
        case 5:
          uiFrequency = 1000;
          ui_drawProcessingPage();
          break;
        case 6:
          uiFrequency = 10000;
          ui_drawTaskReportPage();
          break;
        default:
          uiPage = 1;
          uiFrequency = 10000;
          Serial.println("No page changes");
          Serial.println("UI Page = " + String(uiPage));
          break;
    }
    tft.endWrite();  // End the write transaction
  }
  old_uiPage = uiPage;

  //Check input and change UI
  if (Serial.available() > 0) {
    // Read the incoming byte from the Serial Monitor
    char input = Serial.read();

    // Convert char to integer
    int number = input - '0'; // Convert ASCII to integer value

    Serial.println("Invalid input. Enter 1, 2, or 3.");

    // Execute function based on the input number
    switch (uiPage){
      //Homepage
      case 1:
        switch (number) {
          case 1:
            uiPage = 2;
            ui_drawManualModePage();
            break;
          case 2:
            uiPage = 3;
            ui_drawPresetsPage();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
      //Manual Mode Page
      case 2:
        switch (number) {
          case 1:
            uiPage = 1;
            ui_drawHomeScreen();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
      //Presets Page
      case 3:
        switch (number) {
          case 1:
            uiPage = 1;
            ui_drawHomeScreen();
            break;
          case 2:
            uiPage = 4;
            ui_drawPreparingPage();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
      //Preapring page
      case 4:
        switch (number) {
          case 1:
            uiPage = 3;
            ui_drawPresetsPage();
            break;
          case 2:
            uiPage = 5;
            ui_drawProcessingPage();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
      //Processing page
      case 5:
        switch (number) {
          case 1:
            uiPage = 3;
            ui_drawPresetsPage();
            break;
          case 2:
            uiPage = 6;
            ui_drawTaskReportPage();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
      //Task report page
      case 6:
        switch (number) {
          case 1:
            uiPage = 1;
            ui_drawHomeScreen();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
    }    
  }
}

void ui_drawHomeScreen() {
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

void ui_drawManualModePage() {
  tft.startWrite();  // Begin a write transaction
  // Draw "Manual Mode"
  tft.fillScreen(TFT_WHITE); // Fill the screen with white color

  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as Select mode
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Manual Mode", 120, 20);  // Draw string in the middle of the screen

  // Draw instructions
  tft.setFreeFont(&FreeSans12pt7b);
  tft.setTextColor(TFT_BLACK);  // Set text color to black with white background
  tft.drawString("Instructions", 120, 60);  // Draw string
  tft.setTextDatum(ML_DATUM);  // Set text datum to mid left
  tft.fillCircle(60, 100, 18, TFT_ORANGE);  // Draw filled circle for A
  tft.drawString("A    Forward", 50, 100);  // Draw string
  tft.fillCircle(60, 140, 18, TFT_ORANGE);  // Draw filled circle for B
  tft.drawString("B    Back", 50, 140);  // Draw string
  tft.fillCircle(60, 180, 18, TFT_ORANGE);  // Draw filled circle for C
  tft.drawString("C    Left", 50, 180);  // Draw string
  tft.fillCircle(60, 220, 18, TFT_ORANGE);  // Draw filled circle for D
  tft.drawString("D    Right", 50, 220);  // Draw string

  // Draw Distance
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  int distance = 100;  // Example value for distance
  tft.setTextColor(TFT_BLACK);  // Set text color to black with white background
  tft.drawString("Distance - " + String(distance), 120, 260);  // Draw string with distance variable

  // Draw "Press * to go back" (small grey text)
  tft.setTextColor(TFT_GREY);  // Set text color to gray with white background
  tft.drawString("Press * to go back", 120, 300, 2);  // Draw string

  tft.endWrite();  // End the write transaction
}

void ui_drawPresetsPage() {
  tft.fillScreen(TFT_WHITE);  // Fill the screen with white color

  // Draw "Presets"
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans12pt7b);  // Set font to the same as Select mode
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Presets", 120, 20);  // Draw string in the middle of the screen

  // Initialize the array of preset strings
  String presets[9];
  for (int i = 0; i < 9; i++) {
    presets[i] = "Empty";
  }

  // Draw the list of presets
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setTextFont(2);  // Set font to font 2
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  for (int i = 0; i < 9; i++) {
    tft.drawString(String(i + 1) + " " + presets[i], 10, 50 + i * 20);  // Print each preset with numbering
  }

  // Draw selected preset
  int selectedPreset = 1;  // Example value for selected preset
  tft.drawString("Selected preset: " + String(selectedPreset), 10, 240);  // Draw selected preset

  // Draw "Press # to start"
  tft.setTextColor(TFT_GREEN, TFT_WHITE);  // Set text color to grey with white background
  tft.drawString("Press # to start", 10, 260);  // Draw string

  // Draw "Press * to go back" (small grey text)
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle
  tft.setTextColor(TFT_GREY);  // Set text color to gray with white background
  tft.drawString("Press * to go back", 120, 300, 2);  // Draw string
}

void drawLoadingIcon(int x, int y, int radius, int angle) {
  tft.drawCircle(x, y, radius, TFT_GREY);  // Draw outer circle
  int x1 = x + radius * cos(radians(angle));
  int y1 = y + radius * sin(radians(angle));
  int x2 = x + radius * cos(radians(angle + 180));
  int y2 = y + radius * sin(radians(angle + 180));
  tft.drawLine(x1, y1, x2, y2, TFT_BLACK);  // Draw rotating line
}

void ui_drawPreparingPage() {
  static int angle = 0;
  angle += 30;  // Increment angle to create the rotation effect
  if (angle >= 360) angle = 0;  // Reset angle after a full rotation

  tft.fillRect(0, 0, 240, 320, TFT_WHITE);  // Fill the screen with white color

  // Draw "Preparing" header
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as Presets header
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Preparing", 120, 20);  // Draw string in the middle of the screen

  // Draw Current preset and Preparing
  int currentPreset = 2;  // Example value for current preset
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans12pt7b);  // Set font to a bit large font
  tft.drawString("Current preset: " + String(currentPreset), 120, 120);  // Draw Current preset in the middle
  tft.drawString("Preparing", 120, 160);  // Draw Preparing in the middle

  // Draw animated loading icon
  drawLoadingIcon(120, 200, 20, angle);  // Center of the screen at (120, 200), radius of 20

  // Draw "Press # to cancel"
  tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.drawString("Press # to cancel", 120, 300);  // Draw string in the middle bottom
}

void ui_drawInProgressPage() {
  static int angle = 0;
  static int state = 1;

  tft.fillRect(0, 0, 240, 320, TFT_WHITE);  // Fill the screen with white color

  // Variables
  int currentStep = 3;  // Example current step
  int totalSteps = 5;  // Example total steps
  int speed = 5;  // Speed in cm/s
  int distance = 10;  // Distance in cm
  int progress = 40;  // Progress percentage
  String direction = "left";  // Direction for rotating
  String penStatus = "drawing";  // Pen status

  // Draw "In Progress" header
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as previous headers
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("In Progress", 120, 20);  // Draw string in the middle of the screen

  // Draw current step information
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans12pt7b);  // Set font to a larger font
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Current step " + String(currentStep) + "/" + String(totalSteps), 10, 60);  // Draw current step info

  if (state == 0) {
    // Draw Moving information
    tft.drawString("Moving", 10, 100);  // Draw Moving label
    tft.setTextDatum(TL_DATUM);  // Reset text datum to top-left corner
    tft.drawString("- Speed: " + String(speed) + " cm/s", 10, 130);  // Draw speed info
    tft.drawString("- Distance: " + String(distance) + " cm", 10, 160);  // Draw distance info
    tft.drawString("- Progress: " + String(progress) + "%", 10, 190);  // Draw progress info
  } else if (state == 1) {
    // Draw Rotating information
    tft.drawString("Rotating", 10, 100);  // Draw Rotating label
    tft.setTextDatum(TL_DATUM);  // Reset text datum to top-left corner
    tft.drawString("- Direction: " + direction, 10, 130);  // Draw direction info
    tft.drawString("- Angle: " + String(angle) + " degrees", 10, 160);  // Draw angle info
  }

  // Draw Pen Status
  tft.drawString("Pen Status: " + penStatus, 10, 230);  // Draw pen status

  // Draw "Press # to abort"
  tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.setTextDatum(TL_DATUM);  // Reset text datum to top-left corner
  tft.drawString("Press # to abort", 10, 300);  // Draw string at the bottom

  if (state == 1) {
    angle += 5;  // Increment angle for rotating animation
    if (angle > 90) angle = 0;  // Reset angle after a full rotation
  }
}

void drawSpeedometer(int x, int y, int radius, int speed) {
  tft.drawCircle(x, y, radius, TFT_BLACK);  // Draw outer circle
  int angle = map(speed, 0, 100, -120, 120);  // Map speed to angle
  int x1 = x + radius * cos(radians(angle));
  int y1 = y + radius * sin(radians(angle));
  tft.drawLine(x, y, x1, y1, TFT_RED);  // Draw needle
}

void drawRotatingAnimation(int x, int y, int angle) {
  tft.fillCircle(x, y, 30, TFT_WHITE);  // Clear previous
  tft.drawCircle(x, y, 30, TFT_BLACK);  // Draw outer circle
  int x1 = x + 30 * cos(radians(angle - 90));
  int y1 = y + 30 * sin(radians(angle - 90));
  tft.drawLine(x, y, x1, y1, TFT_RED);  // Draw rotating line
}

void ui_drawProcessingPage() {
  static int angle = 0;
  static int state = 1;

  tft.fillRect(0, 0, 240, 320, TFT_WHITE);  // Fill the screen with white color

  // Variables
  String presetName = "My Preset 1";  // Example preset name
  int currentStep = 3;  // Example current step
  int totalSteps = 5;  // Example total steps
  String currentStepAction = "Moving 100cm";  // Example current step action
  int speed = 30;  // Speed in cm/s
  int distanceCovered = 20;  // Distance covered in cm
  int distanceTarget = 50;  // Distance target in cm
  String direction = "left";  // Direction for rotating
  String penStatus = "drawing";  // Pen status

  // Draw "Processing" header
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as previous headers
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Processing", 120, 20);  // Draw string in the middle of the screen

  // Draw preset name
  tft.setTextFont(2);  // Set font to font 2
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Preset name: " + presetName, 10, 50);  // Draw preset name

  // Draw current step information
  tft.drawString("Current step " + String(currentStep) + "/" + String(totalSteps), 10, 70);  // Draw current step info
  tft.drawString("Current step: " + currentStepAction, 10, 90);  // Draw current step action

  tft.setTextDatum(MC_DATUM);  // Set text datum to top-left corner
  if (state == 0) {
    // Draw Straight motion information
    tft.drawString("Straight motion", 120, 130);  // Draw Straight motion label

    // Draw Speedometer
    drawSpeedometer(120, 180, 40, speed);  // Draw speedometer at (60, 170) with radius 40
    tft.drawString("Speed: " + String(speed) + "cm/s", 120, 230);  // Draw speed in text

    // Draw Distance
    tft.drawString("Distance: " + String(distanceCovered) + "/" + String(distanceTarget) + " cm", 120, 250);  // Draw distance
  } else if (state == 1) {
    // Draw Rotating information
    tft.drawString("Rotating", 120, 130);  // Draw Rotating label
    drawRotatingAnimation(120, 180, angle);  // Draw rotating animation at (60, 170)
    tft.drawString("Angle: " + String(angle) + " degrees", 120, 230);  // Draw angle
    tft.drawString("Direction: " + direction, 120, 250);  // Draw direction
  }

  // Draw Pen Status
  tft.drawString("Pen Status: " + penStatus, 120, 270);  // Draw pen status

  // Draw "Press # to abort"
  tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.drawString("Press # to abort", 120, 300);  // Draw string at the bottom

  if (state == 1) {
    angle += 5;  // Increment angle for rotating animation
    if (angle > 90) angle = 0;  // Reset angle after a full rotation
  }
}

void ui_drawTaskReportPage() {
  tft.fillRect(0, 0, 240, 320, TFT_WHITE);  // Fill the screen with white color

  // Variables
  String presetName = "Moving 100cm";  // Example preset name
  String timeTaken = "12 min 30 sec";  // Example time taken
  String distanceTravelled = "150 cm";  // Example distance travelled
  String avgSpeed = "12.5 cm/s";  // Example average speed

  // Draw "Task Report" header
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  // Set text color to black with white background
  tft.setFreeFont(&FreeSans18pt7b);  // Set font to the same as previous headers
  tft.setTextDatum(MC_DATUM);  // Set text datum to middle center
  tft.drawString("Task Report", 120, 20);  // Draw string in the middle of the screen

  // Draw preset name
  tft.setTextFont(2);  // Set font to font 2
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Preset name: " + presetName, 10, 60);  // Draw preset name

  // Draw time taken
  tft.drawString("Time taken: " + timeTaken, 10, 100);  // Draw time taken

  // Draw distance travelled
  tft.drawString("Distance travelled: " + distanceTravelled, 10, 140);  // Draw distance travelled

  // Draw average speed
  tft.drawString("Avg. Speed: " + avgSpeed, 10, 180);  // Draw average speed

  // Draw "Press * to return to menu"
  tft.setTextColor(TFT_GREY, TFT_WHITE);  // Set text color to grey with white background
  tft.setFreeFont(&FreeSans9pt7b);  // Set font to a smaller font
  tft.setTextDatum(TL_DATUM);  // Set text datum to top-left corner
  tft.drawString("Press * to return to menu", 10, 300);  // Draw string at the bottom
}