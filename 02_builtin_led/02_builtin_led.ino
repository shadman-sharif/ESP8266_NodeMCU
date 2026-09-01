/*
  ESP8266 NodeMCU - Example 02
  Built-in LED Test

  Many NodeMCU boards use an active-low LED.
*/

#define LED_PIN LED_BUILTIN

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  digitalWrite(LED_PIN, LOW);   // ON
  delay(1000);

  digitalWrite(LED_PIN, HIGH);  // OFF
  delay(1000);
}
