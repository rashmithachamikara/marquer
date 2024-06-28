const int sensorPin = 35;  // Pin connected to the OUT of the sensor
volatile int rotationCount = 0;

void IRAM_ATTR handleInterrupt() {
  rotationCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, RISING);
}

void loop() {
  // Print the count to the Serial Monitor
  Serial.print("Wheel Rotations: ");
  Serial.println(rotationCount);

  Serial.println(rotationCount);
  
  // Add a delay to control the print frequency
  delay(1000);
}
