/*

Copied from: L298NserialSpeedAdjust2

*/

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12
#define ENB 13

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

// Constants for encoder resolution
const float ENCODER_1_RESOLUTION = 40; // counts per rotation
const float ENCODER_2_RESOLUTION = 40; // counts per rotation

int speedA = 0; // Speed for motor A (0 to 255)
int speedB = 0; // Speed for motor B (0 to 255)
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

int lastEncoder1Count = 0;
int lastEncoder2Count = 0;
unsigned long lastTime = 0;

unsigned long lastInterruptTime1 = 0;
unsigned long lastInterruptTime2 = 0;

void IRAM_ATTR handleEncoder1Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime1 > 5){
    encoder1Count++;
    lastInterruptTime1 = interruptTime;
  }
}

void IRAM_ATTR handleEncoder2Interrupt() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime2 > 5){
    encoder2Count++;
    lastInterruptTime2 = interruptTime;
  }
}

void setup() {
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
  
  // Check if data is available on the Serial port
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); // Read input until newline
    input.trim(); // Remove any extra whitespace

    if (input.startsWith("a")) {
      speedA = input.substring(1).toInt();
      speedA = constrain(speedA, 0, 255); // Constrain speed to 0-255
      analogWrite(ENA, speedA);
      Serial.print("Speed of Motor A set to: ");
      Serial.println(speedA);
    } else if (input.startsWith("b")) {
      speedB = input.substring(1).toInt();
      speedB = constrain(speedB, 0, 255); // Constrain speed to 0-255
      analogWrite(ENB, speedB);
      Serial.print("Speed of Motor B set to: ");
      Serial.println(speedB);
    } else if (input.startsWith("c")) {
      int speed = input.substring(1).toInt();
      speed = constrain(speed, 0, 255); // Constrain speed to 0-255
      speedA = speed;
      speedB = speed;
      analogWrite(ENA, speedA);
      analogWrite(ENB, speedB);
      Serial.print("Speed of both Motors A and B set to: ");
      Serial.println(speed);
    } else if (input.startsWith("r")) {
      encoder1Count = 0;
      encoder2Count = 0;
      lastEncoder1Count = 0;
      lastEncoder2Count = 0;
    }
  }

  // Example: Drive motor A forward and motor B backward with set speeds
  if (speedA > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  if (speedB > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  // Calculate and print the speed of the encoders every 100ms
  if (currentTime - lastTime >= 100) {
    float encoder1Speed = (encoder1Count - lastEncoder1Count); // * (1000.0 / ENCODER_RESOLUTION); // Rotations per second
    float encoder2Speed = (encoder2Count - lastEncoder2Count); // * (1000.0 / ENCODER_RESOLUTION); // Rotations per second

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
  Serial.print(encoder1Count/ENCODER_1_RESOLUTION);
  Serial.print(" Encoder2RCount:");
  Serial.println(encoder2Count/ENCODER_2_RESOLUTION);
  lastTime = currentTime;


  delay(100); // Short delay to avoid overloading the Serial communication
}
