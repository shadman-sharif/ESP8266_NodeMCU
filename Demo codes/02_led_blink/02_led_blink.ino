/*
  ESP8266 NodeMCU - Example 02
  Onboard LED Blink

  IMPORTANT:
  The onboard LED on D4 (GPIO2) is ACTIVE LOW.
  LOW = LED ON, HIGH = LED OFF. This is intentional, not a bug.
  See docs/GPIO.md section 3.
*/

#define LED_PIN 2  // D4 on the board silkscreen

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED blink test started.");
}

void loop() {
  digitalWrite(LED_PIN, LOW);   // LED ON
  Serial.println("LED ON");
  delay(500);

  digitalWrite(LED_PIN, HIGH);  // LED OFF
  Serial.println("LED OFF");
  delay(500);
}
