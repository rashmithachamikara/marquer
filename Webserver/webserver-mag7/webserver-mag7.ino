//MPU9250
#include "mpu9250.h"
//Wifi and Web
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

/*=========== Pins ===========
VCC -> 3.3v
GND -> gnd
SCL -> D22
SDA -> D21
*/

//=========== MPU9250 init stuff ===========
/* Mpu9250 object */
bfs::Mpu9250 imu;

float x,y,z;
int iHeading;


//=========== Webserver init stuff ===========
AsyncWebServer server(80);

const char* ssid = "SLT-Fiber-3F7F";
const char* password = "A09@3f7f";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}



void setup() {
  /* Serial to display data */
  Serial.begin(115200);
  while(!Serial) {}

  //=========== MPU9250 setup stuff ===========
  /* Start the I2C bus */
  Wire.begin();
  Wire.setClock(400000);
  /* I2C bus,  0x68 address */
  imu.Config(&Wire, bfs::Mpu9250::I2C_ADDR_PRIM);
  /* Initialize and configure IMU */
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while(1) {}
  }
  /* Set the sample rate divider */
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while(1) {}
  }

  //=========== Webserver setup stuff ===========
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print local IP address
  Serial.println(WiFi.localIP());

  // Define the web server route for the main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>ESP32 Web Server</h1>";
    html += "<p>Variable value: <span id='variable'></span></p>";
    html += "<script>";
    html += "setInterval(function() {";
    html += "  fetch('/variable').then(response => response.text()).then(data => {";
    html += "    document.getElementById('variable').innerText = data;";
    html += "  });";
    html += "}, 1000);";  // Update every second
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Define the web server route for the variable with CORS headers
  server.on("/variable", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", String(iHeading));
    request->send(response);
  });

  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
}

void loop() {
  /* Check if data read */
  if (imu.Read()) {

    x = imu.mag_x_ut();
    y = imu.mag_y_ut();
    z = imu.mag_z_ut();


    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.print(z);
    Serial.print("\t");

    // Get magnetometer heading in degrees
    float heading = atan2(y, x) * RAD_TO_DEG;
  
    // Correct for negative heading
    if (heading < 0) {
      heading += 360.0;
    }
  
    iHeading = heading;

    // Print heading
    Serial.print("\tHeading: ");
    Serial.print(heading);
    Serial.print("\tHeading: ");
    Serial.println(iHeading);
  
    // Delay before next reading
    delay(100);
  }
}
