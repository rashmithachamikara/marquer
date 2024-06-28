#include <Wire.h>
#include <MPU9250.h>

MPU9250 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.setup(0x68)) {  // Change to your MPU9250's I2C address if needed
    Serial.println("MPU9250 connection failed.");
    while (1) {}
  }
  
  delay(1000); // Wait for sensor to stabilize
}

void loop() {
  if (mpu.update()) {
    float yaw = mpu.getYaw();
    float pitch = mpu.getPitch();
    float roll = mpu.getRoll();
    
    Serial.print("Yaw: ");
    Serial.print(yaw);
    Serial.print(", Pitch: ");
    Serial.print(pitch);
    Serial.print(", Roll: ");
    Serial.println(roll);
  }
}
