#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 12
#define ENB 13

#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

int speedA = 0; // Speed for motor A (0 to 255)
int speedB = 0; // Speed for motor B (0 to 255)
volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

void IRAM_ATTR handleEncoder1Interrupt() {
  encoder1Count++;
}

void IRAM_ATTR handleEncoder2Interrupt() {
  encoder2Count++;
}

void setup() {
  // Set all motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Start Serial communication at a baud rate of 9600 for motor control
  Serial.begin(9600);

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

  // Start Serial communication at a baud rate of 115200 for encoders
  Serial.begin(115200);
}

void loop() {
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

  // Print the encoder counts in a format suitable for the Serial Plotter
  Serial.print("Encoder1:");
  Serial.print(encoder1Count/20);
  Serial.print(" Encoder2:");
  Serial.println(encoder2Count/20);

  delay(100); // Short delay to avoid overloading the Serial communication
}
