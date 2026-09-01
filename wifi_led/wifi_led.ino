/*
  ESP8266 NodeMCU Project
  Wi-Fi LED Controller

  Replace credentials before uploading.
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

ESP8266WebServer server(80);
const int LED_PIN = LED_BUILTIN;

const char PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Wi-Fi LED</title>
</head>
<body style="font-family:Arial;text-align:center;padding:30px">
  <h1>ESP8266 Wi-Fi LED</h1>
  <p><a href="/on">Turn ON</a></p>
  <p><a href="/off">Turn OFF</a></p>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Open: http://");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", PAGE);
  });

  server.on("/on", []() {
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", PAGE);
  });

  server.on("/off", []() {
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/html", PAGE);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
