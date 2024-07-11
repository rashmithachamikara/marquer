#ifndef MARQUERWEB_H
#define MARQUERWEB_H

#include <Arduino.h>  // Include Arduino core for Serial functions
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>  // Include for LittleFS on ESP32
#include <functional>  // For std::function

extern const char* ssid;
extern const char* password;

extern AsyncWebServer server;
extern AsyncWebSocket ws;

// Callback function type definition
typedef std::function<void(String)> WebMessageCallback;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Template function to send messages via WebSocket
template <typename T>
void WebPrint(T message) {
  ws.textAll(String(message));  // Send the message to all connected WebSocket clients
}

// Template function to send messages with newline via WebSocket
template <typename T>
void WebPrintln(T message) {
  ws.textAll(String(message) + "\n");  // Send the message with a newline to all connected WebSocket clients
}

void setupMarquerWeb();
void loopMarquerWeb();

// Function to set the callback for handling POST messages
void setWebMessageCallback(WebMessageCallback callback);

#endif // MARQUERWEB_H
