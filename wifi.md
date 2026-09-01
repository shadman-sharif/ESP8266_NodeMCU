# 📶 Wi-Fi Guide

## Station mode

The ESP8266 joins an existing Wi-Fi network:

```text
ESP8266
   │
 Wi-Fi
   │
 Router
 ├── Phone
 └── PC
```

Example:

```cpp
#include <ESP8266WiFi.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");
  Serial.println(WiFi.localIP());
}

void loop() {}
```

## Access Point mode

The ESP8266 can create its own local Wi-Fi network:

```text
        ESP8266
           │
     Wi-Fi network
       /           Phone     Laptop
```

## Web server

A web server allows a browser to control hardware or read sensor data:

```text
Browser
   ↓ HTTP
ESP8266
   ├── GPIO
   ├── Sensors
   └── LEDs
```

## Security

A simple HTTP server is suitable for learning on a private local network. Real deployments should use a secure architecture, authentication, and appropriate network protection.
