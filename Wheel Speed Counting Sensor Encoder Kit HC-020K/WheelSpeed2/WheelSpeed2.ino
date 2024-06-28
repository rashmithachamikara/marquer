#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

volatile int encoder1Count = 0;
volatile int encoder2Count = 0;

void IRAM_ATTR handleEncoder1Interrupt() {
  encoder1Count++;
}

void IRAM_ATTR handleEncoder2Interrupt() {
  encoder2Count++;
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Configure encoder pins as inputs
  pinMode(ENCODER1_PIN, INPUT);
  pinMode(ENCODER2_PIN, INPUT);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(ENCODER1_PIN), handleEncoder1Interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER2_PIN), handleEncoder2Interrupt, RISING);
}

void loop() {
  // Print the encoder counts in a format suitable for the Serial Plotter
  Serial.print("Encoder1L:");
  Serial.print(encoder1Count);
  Serial.print(" Encoder2R:");
  Serial.println(encoder2Count);

  delay(100);
}
