#include "marquerWeb.h"

const char* ssid = "Marquer";      // SSID (name) of the ESP32 AP
const char* password = "123123123";  // Password for the ESP32 AP

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");  // Create a WebSocket object

// Global variable to hold the callback function
WebMessageCallback webMessageCallback = nullptr;

// Function to handle WebSocket events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  }
}

//Run marquer web setup content
void setupMarquerWeb() {
  // Serial port initialization
  if (!Serial) {
    Serial.begin(115200);  // Start Serial communication if not already started
  }

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

  //Allow Cross origin
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

  // WebSocket event handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Route to serve the index.html file when /marquer is accessed
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /index");
    if (LittleFS.exists("/index.html")) {
      request->send(LittleFS, "/index.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to serve the controlApp.html file when /marquer is accessed
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

  // Route to serve the controlAppPid.html file when /marquer is accessed
  server.on("/marquerPid", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /marquerPid");
    if (LittleFS.exists("/controlAppPid.html")) {
      request->send(LittleFS, "/controlAppPid.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to serve the controlAppPid.html file when /marquer is accessed
  server.on("/creator", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /creator.html");
    if (LittleFS.exists("/creator.html")) {
      request->send(LittleFS, "/creator.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to serve the controlAppPid.html file when /marquer is accessed
  server.on("/presets", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /presets.html");
    if (LittleFS.exists("/presets.html")) {
      request->send(LittleFS, "/presets.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to serve the keypad.html file when /marquer is accessed
  server.on("/keypad", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /presets.html");
    if (LittleFS.exists("/keypad.html")) {
      request->send(LittleFS, "/keypad.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Route to serve the chart.js file when /marquer is accessed
  server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /chart.js");
    if (LittleFS.exists("/chart.js")) {
      request->send(LittleFS, "/chart.js", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });


  // Route to handle incoming POST requests
  server.on("/post_command", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("message", true)) {
      String message = request->getParam("message", true)->value();
      Serial.print("Received POST message: ");
      Serial.println(message);
      if (webMessageCallback) {
        webMessageCallback(message);  // Invoke the callback with the received message
      }
      request->send(200, "text/plain", "Message received");
    } else {
      Serial.println("Received POST request with no message");
      request->send(400, "text/plain", "No message parameter");
    }
  });

  // ================ Remote Control Routes ===================
  // Route for root / web page
  server.on("/remote", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Serving /remote.html");
    if (LittleFS.exists("/remote.html")) {
      request->send(LittleFS, "/remote.html", "text/html");
      Serial.println("File served successfully");
    } else {
      request->send(404, "text/plain", "File not found");
      Serial.println("File not found");
    }
  });

  // Start server
  server.begin();
  Serial.println("Server started");
}

//Run marquer web loopable program content
void loopMarquerWeb() {
  // This is optional, you can add other tasks here if needed
  ws.cleanupClients();  // Cleanup disconnected WebSocket clients
}

// Function to set the callback for handling POST messages
void setWebMessageCallback(WebMessageCallback callback) {
  webMessageCallback = callback;
}
