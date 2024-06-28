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

//variables
int uiPage = 1;
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
  
  homeScreen();
}

void loop() {
  //exampleLoop();
  //homeScreen();
  //homeScreen1();
  //homeScreen3();
  //drawManualModePage();

  Serial.println("Pass");
  drawUi();

  delay(100);
}

void drawUi(){
  unsigned int uiFrequency = 10000; //How often should UI update 
  static unsigned long lastUpdate = 0; //A static variable inside a function retains its value between function calls
  unsigned long currentMillis = millis(); //Milliseconds spent since the boot of device

  //Serial.println("Last Update - "+String(lastUpdate));
  //Serial.println("current Millis - "+String(currentMillis));
  //Debug line below
  if(old_uiPage != uiPage){
    Serial.println("Page difference!");
  }

  //=========== UI Page Index ===========
  

  //Update UI Page either when interval (uiFrequency) reached or uiPage changed
  if (currentMillis - lastUpdate >= uiFrequency || old_uiPage != uiPage) {  // Update every second
    lastUpdate = currentMillis; //Change last update
    //Draw UI page
    switch (uiPage) {
        case 1: 
          homeScreen();
          break;
          //Switch for page 1
        case 2:
          drawManualModePage();
          break;
        default:
          uiPage = 1;
          Serial.println("No page changes");
          Serial.println("UI Page = " + String(uiPage));
          break;
    }
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
            drawManualModePage();
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
            homeScreen();
            break;
          default:
            Serial.println("Invalid input. Enter 1, 2, or 3.");
            break;
        }
        break;
    }    
  }
}
