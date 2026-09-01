/*
  ESP8266 NodeMCU - Example 03
  External GPIO test

  Connect:
  D1 -> suitable resistor -> LED -> GND

  Verify your component and wiring before powering the circuit.
*/

const int LED_PIN = D1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("D1 HIGH");
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  Serial.println("D1 LOW");
  delay(1000);
}
