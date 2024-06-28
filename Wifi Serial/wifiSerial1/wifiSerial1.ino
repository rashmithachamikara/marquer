/*
  WebSerialLite Demo AP
  ------
  This example code works for both ESP8266 & ESP32 Microcontrollers
  WebSerial is accessible at 192.168.4.1/webserial URL.

  Author: HomeboyC
*/
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerialLite.h>

AsyncWebServer server(80);

const char* ssid = "Marquer"; // Your WiFi AP SSID 
const char* password = "123123123"; // Your WiFi Password


/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  Serial.print(d);
}

void setup() {
    Serial.begin(9600);
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server);
    /* Attach Message Callback */
    WebSerial.onMessage(recvMsg);
    server.begin();
}

void loop() {
    delay(2000);
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP().toString());
    WebSerial.printf("Millis=%lu\n", millis());
    WebSerial.printf("Free heap=[%u]\n", ESP.getFreeHeap());
}