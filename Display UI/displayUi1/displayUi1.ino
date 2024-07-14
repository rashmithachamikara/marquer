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
//#include <Arduino.h> 

//Define custom colors
#define TFT_GREY 0x5AEB // New colour
#define TFT_ORANGE 0xFBE0 // New colour
#define TFT_WHITE 0xFFFF // New colour
#define TFT_PURPLE 0x209c // New colour

TFT_eSPI tft = TFT_eSPI();  // Invoke library

//variables
int uiPage = 6;
unsigned int old_uiPage = uiPage;
bool staticContentDrawn = false;
bool uiDebug = true;

//UIPages variables
int selectedPreset = 1;  // Current preset


//====== Keypad ======
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
byte rowPins[ROWS] = {0, 1, 2, 3}; // P0 to P3
byte colPins[COLS] = {4, 5, 6};    // P4 to P6

// Variables for debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // 50 milliseconds debounce delay

// Store the previous state of each key
bool previousKeyState[ROWS][COLS] = {false};


void setup(void) {
  // Start serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  keypadSetup();
  Serial.println("Enter a number (1, 2, or 3) to execute a function:");

  //Display init
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  ui_drawTaskReportPage();
  
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
          //Switch for page 1
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

void uiEngine2(){
  static unsigned int uiFrequency = 1; //How often should UI update 
  static unsigned long lastUpdate = 0; //A static variable inside a function retains its value between function calls
  unsigned long currentMillis = millis(); //Milliseconds spent since the boot of device

  //Serial.println("Last Update - "+String(lastUpdate));
  //Serial.println("current Millis - "+String(currentMillis));
  //Debug line below
  if(old_uiPage != uiPage){
    Serial.println("Page difference!");
  }


  /*=========== UI Page Index ===========

  1 - Homepage
  2 - Manual Mode
  3 - Preset Mode
  4 - Preapring
  5 - In Progress
  6 - Task report


  */

  // ========== Draw ========= Update UI Page either when interval (uiFrequency) reached or uiPage changed
  if (currentMillis - lastUpdate >= uiFrequency || old_uiPage != uiPage) {  // Update every second
    lastUpdate = currentMillis; //Change last update
    //Draw UI page
    tft.startWrite();
    switch (uiPage) {
        case 1: 
          ui_drawHomeScreen();
          break;
          //Switch for page 1
        case 2:
          ui_drawManualModePage();
          break;
        case 3:
          ui_drawPresetsPage();
          break;
        case 4:
          ui_drawPreparingPage();
          break;
        case 5:
          ui_drawProcessingPage();
          break;
        case 6:
          ui_drawTaskReportPage();
          break;
        default:
          uiPage = 1;
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

    switch (number) {
          case 9:
            uiFrequency++;
            break;
          case 8:
            uiFrequency--;
    }
    


    // ========== Inputs ========= Execute function based on the input number
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

  Serial.println(uiFrequency);
}

void handleUiInputs(char input){
  // Convert char to integer
  int number = input - '0'; // Convert ASCII to integer value

  Serial.println("Invalid input. Enter 1, 2, or 3.");

  //Set static update to 0
  staticContentDrawn = false;

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
      Serial.println("Not valid command. Checking for UI page changes");
      break;
  }

  //Execute function based on the input number and page
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
          break;
        case '#':
          uiPage = 4;
          ui_drawPreparingPage();
          break;
        case '*':
          uiPage = 1;
          ui_drawHomeScreen();
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