#define ENCODER1_PIN 34
#define ENCODER2_PIN 35

volatile int encoder1State = 0;
volatile int encoder2State = 0;

void IRAM_ATTR encoder1ISR() {
  encoder1State = digitalRead(ENCODER1_PIN);
}

void IRAM_ATTR encoder2ISR() {
  encoder2State = digitalRead(ENCODER2_PIN);
}

void setup() {
  Serial.begin(115200);

  pinMode(ENCODER1_PIN, INPUT);
  pinMode(ENCODER2_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER1_PIN), encoder1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER2_PIN), encoder2ISR, CHANGE);
}

void loop() {
  Serial.print("Enc1:");
  Serial.print(encoder1State);
  Serial.print(",");
  Serial.print("Enc2:");
  Serial.println(encoder2State);
  
  delay(100); // Adjust the delay as needed for your application
}
