/*  
 The Display and UI for Marquer

 =========== Library information ===========

 Test the tft.print() viz. embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

//====== Display ======
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "marquerWeb.h"
#include <Arduino_JSON.h>
#include <Preferences.h>
//#include <Arduino.h> 

//Define custom colors
#define TFT_GREY 0x5AEB // New colour
#define TFT_ORANGE 0xFBE0 // New colour
#define TFT_WHITE 0xFFFF // New colour
#define TFT_PURPLE 0x209c // New colour

TFT_eSPI tft = TFT_eSPI();  // Invoke library

//variables
int uiPage = 1;
unsigned int old_uiPage = uiPage;
bool staticContentDrawn = false;
bool uiDebug = false;

//UIPages variables
double wheelDistance = 120; //Comment when merge As in main code
int selectedPreset = 1;  // Current preset
bool connected = false;  // Change this to true if app is connected

//============ Keypad ============
#include <Wire.h>
#include <PCF8574.h>

#define KEYPAD_ADDRESS 0x20

PCF8574 keypad(KEYPAD_ADDRESS);

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Define pin numbers corresponding to PCF8574T pins
byte rowPins[ROWS] = {1, 6, 5, 3};  //{0, 1, 2, 3}; // P0 to P3
byte colPins[COLS] = {2, 0, 4};     //{4, 5, 6};    // P4 to P6

// Variables for debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50 milliseconds debounce delay

// Store the previous state of each key
bool previousKeyState[ROWS][COLS] = {false};
//================================================

//Presets

struct Preset {
  String name;
  String instructions;
};

Preset presets[9]; // Maximum 9 presets
//=================================

void setup(void) {
  // Start serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  setupMarquerWeb();
  loadPresets();

  // Set the callback function to handle POST messages
  setWebMessageCallback([](String message) {
    Serial.print("Received message via POST:");
    Serial.println(message);
    // Input Handling
    if (message.startsWith("PRESETS")) {
      savePresets(message);
    } else {
      webInput(message);
    }
    
  });

  keypadSetup();
  Serial.println("Enter a number (1, 2, or 3) to execute a function:");

  //Display init
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);
  ui_drawHomeScreen();
  
}

void loop() {
  uiEngine();

  loopMarquerWeb();
  //Serial.println("Pass");

  delay(100);
}

void uiEngine(){
  static unsigned int uiFrequency = 35; //How often should UI update 
  static unsigned long lastUpdate = 0; //A static variable inside a function retains its value between function calls
  unsigned long currentMillis = millis(); //Milliseconds spent since the boot of device

  //Serial.println("Last Update - "+String(lastUpdate));
  //Serial.println("current Millis - "+String(currentMillis));
  //Debug line below
  if(old_uiPage != uiPage){
    Serial.println("Page difference!");
  }


  /*
  =========== UI Page Index ===========

  1 - Homepage
  2 - Manual Mode
  3 - Preset Mode
  4 - Preapring
  5 - In Progress
  6 - Task report
  */

  //Serial.println("Condition 1: " + String(currentMillis - lastUpdate >= uiFrequency) + ", Condition 2: " + String(old_uiPage != uiPage));
  // ========== Draw ========= Update UI Page either when interval (uiFrequency) reached or uiPage changed
  if (currentMillis - lastUpdate >= uiFrequency || old_uiPage != uiPage) {  // Update every second
    lastUpdate = currentMillis; //Change last update
    //Draw UI page
    tft.startWrite();
    switch (uiPage) {
        case 1: 
          uiFrequency = 60;
          ui_drawHomeScreen();
          //staticContentDrawn = true;
          break;
        case 2:
          uiFrequency = 60;
          ui_drawManualModePage();
          //staticContentDrawn = true;
          break;
        case 3:
          uiFrequency = 60;
          ui_drawPresetsPage();
          //staticContentDrawn = true;
          break;
        case 4:
          uiFrequency = 60;
          ui_drawPreparingPage();
          //staticContentDrawn = true;
          break;
        case 5:
          uiFrequency = 60;
          ui_drawProcessingPage();
          //staticContentDrawn = true;
          break;
        case 6:
          uiFrequency = 60;
          ui_drawTaskReportPage();
          //staticContentDrawn = true;
          break;
        default:
          uiPage = 1;
          uiFrequency = 60;
          Serial.println("No page changes");
          Serial.println("UI Page = " + String(uiPage));
          break;
    }
    if(uiDebug){
      drawGrid();
    }
    staticContentDrawn = true;
    tft.endWrite();  // End the write transaction
  }
  old_uiPage = uiPage;

  //Check serial inputs
  if (Serial.available() > 0) {
    // Read the incoming byte from the Serial Monitor
    char input = Serial.read();
    handleUiInputs(input);
  }

  //Check keypad inputs
  char key = getKey();
  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);
    handleUiInputs(key);
  }
}

void handleUiInputs(char input){
  // Convert char to integer
  int number = input - '0'; // Convert ASCII to integer value ('0' has an ASCII value of 48)

  Serial.println("Input: " + String(input) + "; Handling UI Input... " );

  // ========== Inputs =========
  //Execute global input regardless of current UI page
  switch(input){
    //Debug mode toggle
    case 'g':
    case 'd':
      if(uiDebug){
        uiDebug = false;
        Serial.println("UI Debug mode turned off!");
      } else {
        uiDebug = true;
        Serial.println("UI Debug mode turned on!");
      }
      break;
    default:
      Serial.println("Not a valid global command. Checking for UI page change commands.UI page: "+String(uiPage));
      break;
  }

  //Execute function based on the input number and page
  switch (uiPage){
    //Homepage
    case 1:
      switch (input) {
        case '1':
          uiPage = 2;
          staticContentDrawn = false;
          ui_drawManualModePage();
          break;
        case '2':
          uiPage = 3;
          selectedPreset = 0;
          staticContentDrawn = false;
          ui_drawPresetsPage();
          break;
        default:
          Serial.println("Invalid input. UI page 1");
          break;
      }
      break;
    //Manual Mode Page
    case 2:
      switch (input) {
        case '*':
          uiPage = 1;
          staticContentDrawn = false;
          ui_drawHomeScreen();
          break;
        default:
          Serial.println("Invalid input 2. UI page 2");
          break;
      }
      break;
    //Presets Page
    case 3:
      Serial.println("Hit UI page 3");
      switch (input) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          selectedPreset = number; //number is i
          Serial.println("Hit presets number");
          break;
        case '#':
          uiPage = 4;
          staticContentDrawn = false;
          ui_drawPreparingPage();
          break;
        case '*':
          uiPage = 1;
          staticContentDrawn = false;
          ui_drawHomeScreen();
          break;
        default:
          Serial.println(String(input) + " is an invalid input. UI page 3");
          break;
      }
      break;
    //Preapring page
    case 4:
      switch (input) {
        case '*':
          uiPage = 3;
          staticContentDrawn = false;
          ui_drawPresetsPage();
          break;
        case '#':
          uiPage = 5;
          staticContentDrawn = false;
          ui_drawProcessingPage();
          break;
        default:
          Serial.println("Invalid input. UI page 4");
          break;
      }
      break;
    //Processing page
    case 5:
      switch (input) {
        case '*':
          uiPage = 3;
          staticContentDrawn = false;
          ui_drawPresetsPage();
          break;
        case '2':
        case '#':
          uiPage = 6;
          staticContentDrawn = false;
          ui_drawTaskReportPage();
          break;
        default:
          Serial.println("Invalid input. UI page 5");
          break;
      }
      break;
    //Task report page
    case 6:
      switch (input) {
        case '1':
        case '*':
          uiPage = 1;
          staticContentDrawn = false;
          ui_drawHomeScreen();
          break;
        default:
          Serial.println("Invalid input. UI page 6");
          break;
      }
    break;
  }    
}

void webInput(String input){
  input.trim(); // Remove any extra whitespace
  input.toUpperCase();
}
