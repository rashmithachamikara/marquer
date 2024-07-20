#define TRIG_PIN 19
#define ECHO_PIN 5

const unsigned long measurementInterval = 1000; // 1 second
unsigned long trigTime = 0;
unsigned long echoStartTime = 0;
unsigned long echoEndTime = 0;
bool isWaitingForEcho = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  static unsigned long lastMeasurementTime = 0;

  // Check if it's time to start a new measurement
  if (millis() - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = millis();
    triggerSensor();
  }

  // Check if we are waiting for the echo and process it if it has been received
  if (isWaitingForEcho) {
    processEcho();
  }
}

void triggerSensor() {
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Set the trigger pin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Record the time when the trigger was sent
  trigTime = millis();
  echoStartTime = 0;
  echoEndTime = 0;
  isWaitingForEcho = true;
}

void processEcho() {
  if (echoStartTime == 0 && digitalRead(ECHO_PIN) == HIGH) {
    // Echo signal started
    echoStartTime = micros();
  } else if (echoStartTime != 0 && digitalRead(ECHO_PIN) == LOW) {
    // Echo signal ended
    echoEndTime = micros();
    isWaitingForEcho = false;
    calculateDistance();
  }
}

void calculateDistance() {
  if (echoStartTime != 0 && echoEndTime != 0) {
    // Calculate the distance
    unsigned long duration = echoEndTime - echoStartTime;
    float distance = duration * 0.034 / 2;

    // Print the distance
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  } else {
    Serial.println("Failed to receive echo signal.");
  }
}