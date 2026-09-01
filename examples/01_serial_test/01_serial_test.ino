/*
  ESP8266 NodeMCU - Example 01
  Basic Serial Test
*/

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("     ESP8266 NODEMCU TEST");
  Serial.println("==============================");
  Serial.println("NodeMCU started!");
}

void loop() {
  Serial.println("ESP8266 is running...");
  delay(1000);
}
