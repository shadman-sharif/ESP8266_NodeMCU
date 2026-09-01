/*
  ESP8266 NodeMCU - Example 04
  Wi-Fi Test

  Replace the placeholder credentials before uploading.
*/

#include <ESP8266WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("      ESP8266 WI-FI TEST");
  Serial.println("==============================");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Signal RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}

void loop() {
  delay(1000);
}
