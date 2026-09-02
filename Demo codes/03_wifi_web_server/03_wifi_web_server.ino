/*
  ESP8266 NodeMCU - Example 03
  Simple Wi-Fi Web Server (status dashboard)

  What it does:
  - Connects to your Wi-Fi
  - Serves a web page showing uptime and the A0 analog reading
  - Refreshes automatically every 2 seconds

  Before uploading:
  1. Change WIFI_SSID and WIFI_PASSWORD below
  2. Board: "NodeMCU 1.0 (ESP-12E Module)"
  3. Upload, then open Serial Monitor at 115200 to see the IP address
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);

void handleRoot() {
  int analogValue = analogRead(A0);
  unsigned long uptimeSec = millis() / 1000;

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;background:#111;color:#fff;text-align:center;padding:30px;}";
  html += "h1{color:#4fc3f7;} .box{background:#222;border-radius:10px;padding:20px;display:inline-block;}</style>";
  html += "</head><body><h1>NodeMCU Dashboard</h1><div class='box'>";
  html += "<p>Uptime: " + String(uptimeSec) + " seconds</p>";
  html += "<p>A0 reading: " + String(analogValue) + "</p>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
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
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
}
