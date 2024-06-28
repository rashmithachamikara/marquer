// ================== WEBSERVER ==================
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
  changeSpeed(d);
}
// ================== WEBSERVER END ==================

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12
#define ENB 13

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

// Constants for encoder resolution
const float ENCODER_RESOLUTION = 140; // counts per rotation

int speedA = 180; // Speed for motor A (0 to 255)
int speedB = 170; // Speed for motor B (0 to 255)
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

int lastEncoder1Count = 0;
int lastEncoder2Count = 0;
unsigned long lastTime = 0;

void IRAM_ATTR handleEncoder1Interrupt() {
  encoder1Count++;
}

void IRAM_ATTR handleEncoder2Interrupt() {
  encoder2Count++;
}

void setup() {
  // ================== WEBSERVER ==================
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  /* Attach Message Callback */
  WebSerial.onMessage(recvMsg);
  server.begin();
  // ================== WEBSERVER END ==================

  // Set all motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Start Serial communication at a baud rate of 115200 for motor control and encoders
  Serial.begin(115200);

  // Enable the motors
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Initialize motors in stopped state
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Configure encoder pins as inputs
  pinMode(ENCODER1_PIN, INPUT);
  pinMode(ENCODER2_PIN, INPUT);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(ENCODER1_PIN), handleEncoder1Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2_PIN), handleEncoder2Interrupt, RISING);

  // Wait a moment for serial communication to stabilize
  delay(100);
}

void loop() {
  unsigned long currentTime = millis();

  // Run motors forward at speed 80 for 5 seconds
  if (currentTime < 6000) {
    // speedA = 80;
    // speedB = 80;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    Stop motors
    speedA = 0;
    speedB = 0;
    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Calculate and print the speed of the encoders every 100ms
  if (currentTime - lastTime >= 100) {
    float encoder1Speed = (encoder1Count - lastEncoder1Count) * (1000.0 / ENCODER_RESOLUTION); // Rotations per second
    float encoder2Speed = (encoder2Count - lastEncoder2Count) * (1000.0 / ENCODER_RESOLUTION); // Rotations per second

    // Send data to Serial Plotter in a single line with comma-separated values
    Serial.print("Encoder1Speed:");
    Serial.print(encoder1Speed);
    Serial.print(",");
    Serial.print("Encoder2Speed:");
    Serial.print(encoder2Speed);

    lastEncoder1Count = encoder1Count;
    lastEncoder2Count = encoder2Count;
    lastTime = currentTime;
  }

  // Print the encoder counts every second
  Serial.print(" Encoder1LCount:");
  Serial.print(encoder1Count / ENCODER_RESOLUTION);
  Serial.print(" Encoder2RCount:");
  Serial.println(encoder2Count / ENCODER_RESOLUTION);
  lastTime = currentTime;

  WebSerial.println(encoder2Count / ENCODER_RESOLUTION);

  delay(100); // Short delay to avoid overloading the Serial communication
}
