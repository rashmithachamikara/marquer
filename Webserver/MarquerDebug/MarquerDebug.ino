#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>  // Include for LittleFS on ESP32

const char* ssid = "Marquer2";      // SSID (name) of the ESP32 AP
const char* password = "123123123";  // Password for the ESP32 AP

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // Create a WebSocket object

// Function to handle WebSocket events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  }
}

// Function to send messages via WebSocket
void WebPrint(String message) {
  ws.textAll(message);  // Send the message to all connected WebSocket clients
}

void WebPrintln(String message) {
  ws.textAll(message + "\n");  // Send the message with a newline to all connected WebSocket clients
}

void setup() {
  // Serial port initialization
  Serial.begin(115200);

  // Start WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  // Print ESP32's IP address
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start the LittleFS file system
  if (!LittleFS.begin(true)) {
    Serial.println("An error occurred while mounting LittleFS");
    return;
  } else {
    Serial.println("LittleFS mounted successfully");
  }

  // WebSocket event handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Route to serve the server1.html file when /marquer is accessed
  server.on("/marquer", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /marquer");
    if (LittleFS.exists("/controlApp.html")) {
      request->send(LittleFS, "/controlApp.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to handle incoming GET requests
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String message = "Hello from ESP32 AP!";
    request->send(200, "text/plain", message);
    Serial.println("Received GET request");
  });

  // Route to handle incoming POST requests
  server.on("/post_command", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("message", true)) {
      String message = request->getParam("message", true)->value();
      Serial.print("Received POST message: ");
      Serial.println(message);
      WebPrintln("Received POST message: " + message);  // Send message to WebSocket clients
      request->send(200, "text/plain", "Message received");
    } else {
      Serial.println("Received POST request with no message");
      request->send(400, "text/plain", "No message parameter");
    }
  });

  // Start server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // This is optional, you can add other tasks here if needed
  ws.cleanupClients();  // Cleanup disconnected WebSocket clients
  WebPrintln("Helllooooooo54321");
  delay(100);
}
