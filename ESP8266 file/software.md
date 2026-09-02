# 💻 ESP8266 NodeMCU Software Guide

## 1. Arduino IDE Setup

```text
Install Arduino IDE (2.x)
       ↓
File → Preferences → Additional Board URLs:
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
       ↓
Tools → Board Manager → search "esp8266" → Install (by ESP8266 Community)
       ↓
Tools → Board → "NodeMCU 1.0 (ESP-12E Module)"
       ↓
Select the correct serial port
       ↓
Upload → Open Serial Monitor at 115200 baud
```

## 2. Tools menu settings that actually matter

| Setting | Value | Why |
|---|---|---|
| Board | NodeMCU 1.0 (ESP-12E Module) | Correct pin mapping and flash layout |
| Flash Size | 4MB (FS: 1MB, OTA: ~1019KB) or similar | Needs enough space set aside if you use LittleFS (example 04) — too small a filesystem partition means file writes silently fail |
| Upload Speed | Start at 115200 | 921600 is faster but fails more often on cheap USB-serial chips or long cables; drop it if uploads are unreliable |
| CPU Frequency | 80 MHz (default is fine) | 160MHz only needed for heavier processing; uses more power |
| Erase Flash | "Only Sketch" (default) | Switch to **"All Flash Contents"** once, if a board is behaving strangely after many different sketches — this wipes stale LittleFS data, old Wi-Fi credentials, and EEPROM leftovers from previous projects that "Only Sketch" leaves untouched |

## 3. Libraries

```cpp
#include <ESP8266WiFi.h>      // Wi-Fi — built into the board package
#include <ESP8266WebServer.h> // simple HTTP server — built in
#include <LittleFS.h>         // internal flash filesystem — built in
```

You do not need to install any of these separately from the Library Manager — installing a duplicate is a common source of "multiple definition" compile errors, same trap as on ESP32.

**Worth installing once you outgrow the basics:**
- **ESPAsyncWebServer** + **ESPAsyncTCP** — a non-blocking replacement for `ESP8266WebServer`. The built-in server processes one request at a time inside `server.handleClient()`; the async version can serve pages, WebSockets, and Server-Sent Events without stalling your other code. Worth the switch the moment your `loop()` needs to do real work besides serving pages.
- **ArduinoOTA** — built into the board package already; lets you upload new firmware over Wi-Fi instead of USB after the first flash. Genuinely useful once a board is mounted somewhere inconvenient to reach.

## 4. LittleFS vs SPIFFS

Older ESP8266 tutorials use `SPIFFS`. **Use `LittleFS` instead** — it's the currently maintained filesystem, more reliable with power loss during writes, and SPIFFS is deprecated in newer board package versions. Example 04 in this repo uses LittleFS.

> [!WARNING]
> **Flash has a finite write-cycle life — usually rated around 100,000 erase/write cycles per sector.** Logging a sensor reading every second via LittleFS sounds harmless but adds up: at 1 write/sec that's ~86,400 writes/day. LittleFS spreads writes across sectors (wear-leveling) so it's far more forgiving than writing to the *same* address repeatedly, but for high-frequency logging, buffer several readings in RAM and flush to flash every minute or so instead of every reading.

## 5. Serial Monitor basics

```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("NodeMCU started!");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
```

## 6. Wi-Fi credentials — keep them out of Git

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

Put these in a `secrets.h` file (already `.gitignore`d) instead of hardcoding in a file you'll commit.

**Extra, rarely mentioned:**
```cpp
WiFi.persistent(false);   // call once, before WiFi.begin()
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
```
By default, `WiFi.begin()` writes the SSID/password to flash *every single time it's called*, even if they haven't changed. If your code ever calls `WiFi.begin()` repeatedly (e.g., inside a reconnect-retry loop), this silently wears down the same flash sector. `WiFi.persistent(false)` stops that — the credentials are only saved once, via the normal SDK behavior, not re-written on every reconnect attempt.

## 7. Don't block `loop()` for too long

The ESP8266 has a background watchdog timer. If `loop()` (or any function it calls) runs too long without returning — for example a `while` loop waiting on something that never happens — the chip will reset itself with a watchdog error. Use `delay()` or `yield()` inside long loops instead of empty busy-waits. In truly unavoidable long blocking sections, call `ESP.wdtFeed()` directly to reset the countdown without yielding control.

## 8. Static IP (skip the DHCP wait, and avoid a subtle timing bug)

```cpp
IPAddress local_IP(192, 168, 1, 184);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFi.config(local_IP, gateway, subnet); // call BEFORE WiFi.begin()
WiFi.begin(ssid, password);
```
This isn't just a convenience — it also sidesteps a real, documented ESP8266 quirk where calling `WiFi.config()` *after* `WiFi.begin()` on some SDK versions silently fails to apply. Always configure the static IP first, then begin the connection.

## 9. Decoding crash dumps — the tool almost nobody mentions

When your board prints something like:

```text
Exception (28):
epc1=0x4020104d epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000
>>>stack>>>
3ffffcb0:  40202461 3ffee6f0 3ffffcd0 40202105
...
<<<stack<<<
```

that raw hex is nearly useless by eye, but it is **fully decodable** back into the actual line of your code that crashed:

1. In Arduino IDE, open **Tools → ESP Exception Decoder** (installed automatically with the ESP8266 board package on recent versions; if missing, search "ESP8266 Exception Decoder" in Library/Board Manager).
2. Copy the *entire* crash block from Serial Monitor (from `Exception` down through `<<<stack<<<`).
3. Paste it into the Exception Decoder panel.
4. It prints the actual function names and line numbers from your sketch that were on the call stack when it crashed.

This single tool turns "my board just crashes sometimes" into an actual stack trace pointing at the offending line — treat it as step one for any crash that isn't an obvious watchdog timeout.

## 10. Development loop

```text
Write → Compile → Upload → Run → Read Serial Monitor → Debug → Improve → Repeat
```

See `docs/troubleshooting.md` for specific error messages and fixes.
