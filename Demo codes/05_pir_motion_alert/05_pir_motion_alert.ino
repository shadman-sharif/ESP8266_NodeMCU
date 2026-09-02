/*
  ESP8266 NodeMCU - Example 05
  Motion-Triggered Alert (PIR sensor -> log to flash + live web status)

  Since NodeMCU has no camera, this project logs *when* motion
  happened (instead of taking a photo like the ESP32-CAM version)
  and shows the latest status on a live web page.

  Wiring the PIR sensor (HC-SR501):
    PIR VCC -> NodeMCU 3V3  (NOT 5V - GPIOs are not 5V tolerant,
                              and most PIR modules work fine at 3.3V;
                              double check your specific module)
    PIR GND -> NodeMCU GND
    PIR OUT -> NodeMCU D5 (GPIO14)

  Before uploading:
  1. Change WIFI_SSID / WIFI_PASSWORD below
  2. Board: "NodeMCU 1.0 (ESP-12E Module)"
  3. Flash Size: an option that reserves filesystem space (LittleFS)
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

#define PIR_PIN 14        // D5
#define COOLDOWN_MS 5000  // minimum time between two logged events

ESP8266WebServer server(80);
unsigned long lastEventTime = 0;
unsigned long lastEventUptime = 0;
int eventCount = 0;

void logEvent() {
  eventCount++;
  lastEventUptime = millis() / 1000;

  File file = LittleFS.open("/motion_log.txt", "a");
  if (file) {
    file.println("Motion #" + String(eventCount) + " at " + String(lastEventUptime) + "s uptime");
    file.close();
  }

  Serial.println("Motion detected! Event #" + String(eventCount));
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;background:#111;color:#fff;text-align:center;padding:30px;}";
  html += "h1{color:#ff7043;} .box{background:#222;border-radius:10px;padding:20px;display:inline-block;}</style>";
  html += "</head><body><h1>Motion Sensor Status</h1><div class='box'>";
  html += "<p>Total events: " + String(eventCount) + "</p>";
  html += "<p>Last event: " + String(lastEventUptime) + "s after boot</p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, formatting...");
    LittleFS.format();
    LittleFS.begin();
  }

  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  Serial.println("Give the PIR sensor 30-60 seconds to calibrate before trusting readings.");
  Serial.println("Watching for motion...");
}

void loop() {
  server.handleClient();

  bool motion = digitalRead(PIR_PIN) == HIGH;
  unsigned long now = millis();

  if (motion && (now - lastEventTime > COOLDOWN_MS)) {
    lastEventTime = now;
    logEvent();
  }
}
