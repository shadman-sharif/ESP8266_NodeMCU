/*
  ESP8266 NodeMCU - Example 05
  Simple Wi-Fi Web Server

  Features:
  - Connects to Wi-Fi
  - Starts an HTTP server
  - Provides LED ON/OFF controls

  Replace Wi-Fi credentials before uploading.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);

#define LED_PIN LED_BUILTIN

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP8266 NodeMCU</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background: #111;
      color: white;
      padding: 30px;
    }
    .card {
      max-width: 500px;
      margin: auto;
      padding: 25px;
      background: #222;
      border-radius: 12px;
    }
    a {
      display: inline-block;
      margin: 8px;
      padding: 12px 20px;
      border-radius: 8px;
      text-decoration: none;
      color: white;
      background: #444;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>ESP8266 NodeMCU</h1>
    <p>Wi-Fi Web Server</p>
    <a href="/led/on">LED ON</a>
    <a href="/led/off">LED OFF</a>
  </div>
</body>
</html>
)rawliteral";

  return html;
}

void handleRoot() {
  server.send(200, "text/html", makePage());
}

void handleLedOn() {
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/html", makePage());
  Serial.println("LED ON");
}

void handleLedOff() {
  digitalWrite(LED_PIN, HIGH);
  server.send(200, "text/html", makePage());
  Serial.println("LED OFF");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led/on", handleLedOn);
  server.on("/led/off", handleLedOff);

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
}
