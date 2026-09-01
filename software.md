# 💻 Software Guide

## Recommended setup

Use **Arduino IDE + ESP8266 Arduino core** for beginner projects.

Other options include PlatformIO and ESP8266 SDK-based development.

## Workflow

```text
Install Arduino IDE
      ↓
Install ESP8266 board support
      ↓
Select board
      ↓
Select serial port
      ↓
Open example
      ↓
Compile
      ↓
Upload
      ↓
Serial Monitor
```

## Serial Monitor

Most examples use:

```text
115200 baud
```

Example:

```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 started!");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
```

## Wi-Fi library

Arduino ESP8266 programs commonly use:

```cpp
#include <ESP8266WiFi.h>
```

## Web server library

This repository's simple web server uses:

```cpp
#include <ESP8266WebServer.h>
```

## Compile vs upload

```text
Source code
    ↓
Compiler
    ↓
Firmware
    ↓
USB/UART
    ↓
ESP8266 flash
```

## Debugging

Use:

```text
Write → Compile → Upload → Run → Serial Monitor → Debug
```

Never publish real Wi-Fi credentials in a public repository.
