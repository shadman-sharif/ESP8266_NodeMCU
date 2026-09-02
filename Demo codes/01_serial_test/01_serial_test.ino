/*
  ESP8266 NodeMCU - Example 01
  Basic Serial Test

  Purpose:
  - Check that the board boots and Arduino IDE upload works
  - Check Serial Monitor communication (115200 baud)
*/

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==========================");
  Serial.println("   NodeMCU (ESP8266) TEST");
  Serial.println("==========================");
  Serial.println("Board started!");
}

void loop() {
  Serial.println("NodeMCU is running...");
  delay(1000);
}
