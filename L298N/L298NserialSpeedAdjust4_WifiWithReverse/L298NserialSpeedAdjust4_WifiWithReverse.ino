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
const float ENCODER_RESOLUTION = 8; // counts per rotation

int speedA = 180; // Speed for motor A (0 to 255)
int speedB = 170; // Speed for motor B (0 to 255)
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

int lastEncoder1Count = 0;
int lastEncoder2Count = 0;
unsigned long lastTime = 0;

bool reverseA = false; // Direction flag for motor A
bool reverseB = false; // Direction flag for motor B

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

  // Control motor direction and speed
  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
  
  if (reverseA) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  if (reverseB) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
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

void changeSpeed(String input) {
  input.trim(); // Remove any extra whitespace

  if (input.startsWith("A")) {
    speedA = input.substring(1).toInt();
    speedA = constrain(speedA, 0, 255); // Constrain speed to 0-255
    Serial.print("Speed of Motor A set to: ");
    Serial.println(speedA);
  } else if (input.startsWith("B")) {
    speedB = input.substring(1).toInt();
    speedB = constrain(speedB, 0, 255); // Constrain speed to 0-255
    Serial.print("Speed of Motor B set to: ");
    Serial.println(speedB);
  } else if (input.startsWith("C")) {
    int speed = input.substring(1).toInt();
    speed = constrain(speed, 0, 255); // Constrain speed to 0-255
    speedA = speed;
    speedB = speed;
    Serial.print("Speed of both Motors A and B set to: ");
    Serial.println(speed);
  } else if (input.startsWith("R")) {
    encoder1Count = 0;
    encoder2Count = 0;
    lastEncoder1Count = 0;
    lastEncoder2Count = 0;
  } else if (input.startsWith("DA")) {
    reverseA = !reverseA; // Toggle direction for Motor A
    Serial.print("Direction of Motor A set to: ");
    Serial.println(reverseA ? "Reverse" : "Forward");
  } else if (input.startsWith("DB")) {
    reverseB = !reverseB; // Toggle direction for Motor B
    Serial.print("Direction of Motor B set to: ");
    Serial.println(reverseB ? "Reverse" : "Forward");
  }

  analogWrite(ENA, speedA);
  analogWrite(ENB, speedB);
}
