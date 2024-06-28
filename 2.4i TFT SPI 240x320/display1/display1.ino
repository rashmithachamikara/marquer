/*  
 Test the tft.print() viz. embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */


#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
//#include <Arduino.h> 

#define TFT_GREY 0x5AEB // New colour
#define TFT_ORANGE 0xFBE0 // New colour
#define TFT_WHITE 0xFFFF // New colour
#define TFT_PURPLE 0x209c // New colour

TFT_eSPI tft = TFT_eSPI();  // Invoke library


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
}

void loop() {
  //exampleLoop();
  //homeScreen();
  //homeScreen1();
  //homeScreen3();
  drawManualModePage();

  Serial.println("Pass");

  if (0 && Serial.available() > 0) {
    // Read the incoming byte from the Serial Monitor
    char input = Serial.read();

    // Convert char to integer
    int number = input - '0'; // Convert ASCII to integer value

    Serial.println("Invalid input. Enter 1, 2, or 3.");

    // Execute function based on the input number
    switch (number) {
      case 1:
        drawManualModePage();
        break;
      case 2:
        homeScreen3();
        break;
      case 3:
        exampleLoop();
        break;
      default:
        Serial.println("Invalid input. Enter 1, 2, or 3.");
        break;
    }
  }

  delay(1000);
}


//==================== Loop content in the example moved to here ====================
void exampleLoop(){
  // Fill screen with grey so we can see the effect of printing with and without 
  // a background colour defined
  tft.fillScreen(TFT_GREY);
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0, 2);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  // We can now plot text on screen using the "print" class
  tft.println("Hello World!");
  
  // Set the font colour to be yellow with no background, set to font 7
  tft.setTextColor(TFT_YELLOW); tft.setTextFont(7);
  tft.println(1234.56);
  
  // Set the font colour to be red with black background, set to font 4
  tft.setTextColor(TFT_RED,TFT_BLACK);    tft.setTextFont(4);
  //tft.println(3735928559L, HEX); // Should print DEADBEEF

  // Set the font colour to be green with black background, set to font 4
  tft.setTextColor(TFT_GREEN,TFT_BLACK);
  tft.setTextFont(4);
  tft.println("Groop");
  tft.println("I implore thee,");

  // Change to font 2
  tft.setTextFont(2);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  // This next line is deliberately made too long for the display width to test
  // automatic text wrapping onto the next line
  tft.println("Or I will rend thee in the gobberwarts with my blurglecruncheon, see if I don't!");
  
  // Test some print formatting functions
  float fnumber = 123.45;
   // Set the font colour to be blue with no background, set to font 4
  tft.setTextColor(TFT_BLUE);    tft.setTextFont(4);
  tft.print("Float = "); tft.println(fnumber); // Print floating point number
  tft.print("Binary = "); tft.println((int)fnumber, BIN); // Print as integer value in binary
  tft.print("Hexadecimal = "); tft.println((int)fnumber, HEX); // Print as integer number in Hexadecimal
}

void homeScreen(){
  // Fill screen with a color so we can see the effect of printing with and without 
  // a background colour defined
  tft.fillScreen(TFT_WHITE);
  
  // Set "cursor" at top left corner of display (0,0) and select font 2
  // (cursor will move to next line automatically during printing with 'tft.println'
  //  or stay on the line is there is room for the text with tft.print)
  tft.setCursor(0, 0, 2);

  tft.setTextFont(1);
  tft.setTextColor(TFT_BLACK); tft.setTextSize(2);
  // We can now plot text on screen using the "print" class
  //tft.println("Hello World!");

  tft.setTextFont(2);
  tft.setTextColor(TFT_BLACK); tft.setTextSize(2);
  // We can now plot text on screen using the "print" class
  tft.println("Hello World!");

}

// Function to display the UI page
void homeScreen1() {
  tft.fillScreen(TFT_WHITE); // Fill the screen with white color

  // Display "Greetings"
  tft.setTextColor(TFT_BLACK, TFT_WHITE); // Black text, white background
  tft.setTextDatum(MC_DATUM);             // Middle center datum
  tft.drawString("Greetings", 120, 20, 4); // Text, x, y, font

  // Display "Select mode"
  tft.drawString("Select mode", 120, 60, 4); // Text, x, y, font

  // Display "1. Manual mode"
  tft.drawString("1. Manual mode", 120, 100, 4); // Text, x, y, font

  // Display "2. Preset mode"
  tft.drawString("2. Preset mode", 120, 140, 4); // Text, x, y, font

  // Display connection status
  bool isConnected = false; // Change to true if connected
  if (isConnected) {
    tft.setTextColor(TFT_GREEN, TFT_WHITE); // Green text if connected
    tft.drawString("App connection: Connected", 120, 180, 4); // Text, x, y, font
  } else {
    tft.setTextColor(TFT_RED, TFT_WHITE); // Red text if not connected
    tft.drawString("App connection: Not connected", 120, 180, 4); // Text, x, y, font
  }

  // Display IP Address
  tft.setTextColor(TFT_BLACK, TFT_WHITE); // Black text, white background
  tft.drawString("IP Address: 192.168.5.5", 120, 220, 4); // Text, x, y, font

  // Display "Marquer Drawing Systems v1.0" in gray small letters
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE); // Dark gray text, white background
  tft.drawString("Marquer Drawing Systems v1.0", 120, 300, 2); // Text, x, y, font
}

void homeScreen2() {
  tft.fillScreen(TFT_WHITE); // Fill the screen with white color

  // Display "Greetings" (larger, bold, purple)
  tft.setTextColor(TFT_PURPLE, TFT_WHITE); // Purple text, white background
  tft.setTextDatum(MC_DATUM);              // Middle center datum
  tft.drawString("Greetings", 120, 20, 4); // Text, x, y, font (7 is a larger font size)

  // Display "Select mode"
  tft.setTextColor(TFT_BLACK, TFT_WHITE); // Black text, white background
  tft.drawString("Select mode", 120, 90, 3); // Text, x, y, font

  // Display "1. Manual mode" (smaller)
  tft.drawString("1. Manual mode", 120, 140, 2); // Text, x, y, font (2 is a smaller font size)

  // Display "2. Preset mode" (smaller)
  tft.drawString("2. Preset mode", 120, 160, 2); // Text, x, y, font (2 is a smaller font size)

  // Display connection status (smaller)
  bool isConnected = false; // Change to true if connected
  if (isConnected) {
    tft.setTextColor(TFT_GREEN, TFT_WHITE); // Green text if connected
    tft.drawString("App connection: Connected", 120, 220, 2); // Text, x, y, font (2 is a smaller font size)
  } else {
    tft.setTextColor(TFT_RED, TFT_WHITE); // Red text if not connected
    tft.drawString("App connection: Not connected", 120, 220, 2); // Text, x, y, font (2 is a smaller font size)
  }

  // Display IP Address (smaller)
  tft.setTextColor(TFT_BLACK, TFT_WHITE); // Black text, white background
  tft.drawString("IP Address: 192.168.5.5", 120, 240, 2); // Text, x, y, font (2 is a smaller font size)

  // Display "Marquer Drawing Systems v1.0" in gray small letters
  tft.setTextColor(TFT_DARKGREY, TFT_WHITE); // Dark gray text, white background
  tft.drawString("Marquer Drawing Systems v1.0", 120, 300, 2); // Text, x, y, font
}

void homeScreen3() {
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

  if (0 && Serial.available() > 0) {
    // Read the incoming byte from the Serial Monitor
    char input = Serial.read();

    // Convert char to integer
    int number = input - '0'; // Convert ASCII to integer value

    Serial.println("Invalid input. Enter 1, 2, or 3.");

    // Execute function based on the input number
    switch (number) {
      case 1:
        drawManualModePage();
        break;
      default:
        Serial.println("Invalid input. Enter 1, 2, or 3.");
        break;
    }
  }

}

void drawManualModePage() {
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

  if (0 && Serial.available() > 0) {
    // Read the incoming byte from the Serial Monitor
    char input = Serial.read();

    // Convert char to integer
    int number = input - '0'; // Convert ASCII to integer value

    Serial.println("Invalid input. Enter 1, 2, or 3.");

    // Execute function based on the input number
    switch (number) {
      case 0:
        homeScreen3();
        break;
      default:
        Serial.println("Invalid input. Enter 1, 2, or 3.");
        break;
    }
  }

}