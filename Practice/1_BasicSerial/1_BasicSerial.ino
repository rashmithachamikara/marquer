int count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello World!");
  Serial.println("Hello World!");
}

void loop() {
  // put your main code here, to run repeatedly:
  count += 1;
  Serial.print("Looped times: ");
  Serial.println(count);
  delay(200);
}
