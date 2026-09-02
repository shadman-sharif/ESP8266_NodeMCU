# 📶 ESP8266 NodeMCU — The Complete Guide

<div align="center">

[![Board](https://img.shields.io/badge/Board-NodeMCU%20v1.0%20(ESP--12E)-blue?logo=espressif&logoColor=white)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![Filesystem](https://img.shields.io/badge/Storage-LittleFS-orange)](docs/software.md#4-littlefs-vs-spiffs)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

**A zero-to-working guide for the ESP8266 NodeMCU** — hardware, setup, a live Wi-Fi dashboard, flash-based data logging, and a motion-alert project. Written to include the errors and gotchas that most tutorials leave out.

</div>

> [!NOTE]
> ESP8266 has **no camera support** and **no SD card slot**. Need a camera? Use an ESP32-CAM instead. This guide covers what NodeMCU is actually great at: Wi-Fi, GPIO, and always-on sensor nodes. Storage examples here use the chip's **internal flash (LittleFS)**.

> [!TIP]
> **Target board:** all examples target the common **NodeMCU v1.0 (ESP-12E, Micro-USB)**. Wemos D1 Mini / NodeMCU v3 use the same chip but different silkscreen — check [`docs/hardware.md §2`](docs/hardware.md#2-which-nodemcu-do-you-have) first.

---

## 📖 Table of Contents

- [What is ESP8266 NodeMCU?](#-what-is-esp8266-nodemcu)
- [NodeMCU vs. Alternatives](#-nodemcu-vs-alternatives)
- [Repository Structure](#️-repository-structure)
- [What You Need](#-what-you-need)
- [Quick Start](#-quick-start)
- [Pin Naming — read before wiring](#-pin-naming--read-this-before-wiring-anything)
- [Boot-Cause Cheat Sheet](#-boot-cause-cheat-sheet)
- [Learning Path](#-learning-path)
- [Security & Privacy](#-security--privacy)
- [Next Project Ideas](#-next-project-ideas)
- [FAQ](#-faq)
- [License](#-license)

---

## 📚 What is ESP8266 NodeMCU?

A low-cost Wi-Fi development board built around the **ESP8266** chip, with USB-to-serial already built in (no separate programmer needed, unlike a bare ESP32-CAM module).

```text
   Sensors/GPIO → ESP8266 (Wi-Fi + CPU) → Wi-Fi → Phone/PC browser
                        │
                        └──→ Internal Flash (LittleFS) → saved logs/settings
```

**Under the hood** (rarely mentioned, actually useful to know):
- The chip is a single-core **Tensilica L106** running at 80/160 MHz — there is no true RTOS multitasking; `server.handleClient()`-style libraries just poll fast inside `loop()`.
- **64 KB** of instruction RAM (IRAM) and **96 KB** of data RAM total — large sketches with heavy `String` concatenation can fragment this heap and crash hours later, not immediately (see [`troubleshooting.md §9`](docs/troubleshooting.md#9-random-crashes-after-hoursdays-not-at-boot)).
- Only **one** analog pin (A0), 0–1 V natively at the chip, stepped up to 0–3.3 V on NodeMCU boards via an onboard resistor divider — this is why raw ESP-12 modules and NodeMCU dev boards give different `analogRead()` ranges for the same voltage.

---
## NodeMCU ESP8266

### NodeMCU Board

![NodeMCU Board](images/nodecmcu-board.jpg)

### NodeMCU Pinout

![NodeMCU Pinout](images/nodecmcu-pinout.png)

---

## ⚖️ NodeMCU vs. Alternatives

A comparison that's genuinely hard to find in one place:

| | **NodeMCU (ESP8266)** | **Wemos D1 Mini** | **ESP32 Dev Board** | **ESP32-CAM** |
|---|---|---|---|---|
| Wi-Fi | ✅ 2.4GHz only | ✅ 2.4GHz only | ✅ 2.4GHz | ✅ 2.4GHz |
| Bluetooth | ❌ | ❌ | ✅ BLE + Classic | ✅ BLE |
| Cores | 1 | 1 | 2 | 2 |
| Camera | ❌ | ❌ | ❌ (needs add-on) | ✅ built in |
| Onboard USB-serial | ✅ | ✅ | ✅ (most boards) | ❌ (needs FTDI) |
| Usable GPIOs | ~9 | ~9 | ~25 | ~9 (many shared w/ camera) |
| True analog pins | 1 (shared, 3.3V range) | 1 | 18 | limited |
| Good first board? | ✅ Yes | ✅ Yes (smaller) | ✅ Yes | ⚠️ Only if you want the camera |

**Rule of thumb:** start on NodeMCU or D1 Mini for anything Wi-Fi + sensors. Move to ESP32 once you need Bluetooth, more GPIOs, or heavier processing (e.g., running two things at once with FreeRTOS tasks).

---

## 🗂️ Repository Structure

```text
ESP8266-NodeMCU-Guide/
├── README.md
├── hardware.md         Board parts, variants, power, boot pins (read this first)
├── software.md         Arduino IDE setup, Tools menu settings, debugging tools
├── GPIO.md             Dx-to-GPIO pin map + which pins are actually safe to use
├── troubleshooting.md  Real error messages, decoded exceptions, and their fixes
└── examples/
    ├── 01_serial_test/            Confirm the board boots
    ├── 02_led_blink/              Onboard LED (active-LOW gotcha)
    ├── 03_wifi_web_server/        Live status dashboard in a browser
    ├── 04_data_logging_littlefs/  Log sensor data to internal flash
    └── 05_pir_motion_alert/       PIR sensor triggers logged alert + live page
```

---

## 🧰 What You Need

**For the basic examples (01–03):**
- NodeMCU v1.0 (ESP8266) board
- Micro-USB **data** cable (not charge-only — this is the #1 "board not detected" cause)
- Wi-Fi network, **2.4GHz only** (ESP8266 cannot see 5GHz SSIDs at all, even if you type the password correctly)

**Extra for example 05:**
- HC-SR501 PIR motion sensor

**Nice to have but not required:**
- A **100–470 µF** electrolytic capacitor across 3.3V/GND if you'll be adding sensors — smooths the current spike when Wi-Fi transmits and prevents the mystery reboot in [`troubleshooting.md §2`](docs/troubleshooting.md#2-board-resets-randomly-especially-right-when-wi-fi-connects)
- A logic-level shifter or a simple two-resistor voltage divider for any 5V sensor

No SD card, no USB-TTL adapter, and no camera module needed for anything in this guide.

---

## 🚀 Quick Start

1. **Read [`docs/hardware.md`](docs/hardware.md)** first — especially the Dx-vs-GPIO pin naming, which trips up almost every beginner.
2. Set up Arduino IDE using [`docs/software.md`](docs/software.md) — board package URL, board selection, and the Flash Size setting you'll need later for LittleFS.
3. Upload `examples/01_serial_test` — confirms the board and upload process work.
4. Upload `examples/02_led_blink` — note the onboard LED is **active-LOW** (`LOW` = on).
5. Upload `examples/03_wifi_web_server` — set your Wi-Fi name/password first, then open the printed IP in a browser on the same 2.4GHz network.
6. Try `examples/04_data_logging_littlefs` and `examples/05_pir_motion_alert` once the basics work.

If anything fails, check [`docs/troubleshooting.md`](docs/troubleshooting.md) — real error text and fixes, not just "check your wiring."

---

## 🔌 Pin Naming — read this before wiring anything

The pins printed on the board (`D0`, `D1`, ... `D8`) are **not** the GPIO numbers you use in raw pin-number code — e.g. `D2` is actually `GPIO4`. Full mapping table: [`docs/GPIO.md`](docs/GPIO.md#1-silkscreen-dx-to-code-gpio-mapping).

```text
D3 (GPIO0) and D4 (GPIO2) and D8 (GPIO15) affect boot mode -
avoid wiring anything that holds them in the wrong state at power-on.
```

---

## 🩺 Boot-Cause Cheat Sheet

The very first lines the board prints on **Serial Monitor at 74880 baud** (not 115200!) tell you *why* it just booted — almost no beginner guide explains how to read this:

| You see | Meaning |
|---|---|
| `rst cause:1, boot mode:(3,6)` | Normal power-on reset |
| `rst cause:2, boot mode:(3,6)` | External reset (button/RST pin pulled low) |
| `rst cause:4, boot mode:(3,6)` | Watchdog reset — your `loop()` blocked too long |
| `rst cause:2, boot mode:(1,6)` | Boot mode `1` = **flash/UART download mode** — a boot pin (D3/D4/D8) was in the wrong state, or FLASH button was held |
| `Fatal exception (28)` | Illegal memory access — see the exception decoder in [`software.md §9`](docs/software.md#9-decoding-crash-dumps-the-tool-almost-nobody-mentions) |
| Garbled symbols, then clean text | Normal — the ROM bootloader prints at 74880 baud, your sketch prints at 115200. Switch the monitor baud rate to 74880 just to read this part. |

---

## 🧩 Learning Path

```text
1. Serial Test → 2. LED Blink → 3. Wi-Fi Web Server →
4. Log Data to Flash → 5. Motion-Triggered Alert →
6. Add more sensors / build your own IoT project
```

---

## 🔐 Security & Privacy

- Don't expose a web dashboard directly to the public internet without authentication — `ESP8266WebServer` has no built-in auth; add `server.authenticate()` (HTTP Basic Auth) at minimum if you port-forward it.
- Never commit real Wi-Fi passwords — use a local `secrets.h` (already in `.gitignore`).
- `WiFi.persistent(false)` before `WiFi.begin()` avoids wearing out the flash sector that stores Wi-Fi credentials if your code reconnects often — flash has a finite (~100,000) write-cycle limit per sector.
- Be mindful of where sensor data is sent if you extend these examples to cloud services.

---

## 📌 Next Project Ideas

**Beginner:** serial test → LED blink → Wi-Fi web page → log a sensor reading (all in this repo).

**Intermediate:** DHT11/DHT22 temperature-humidity dashboard, button-controlled relay over Wi-Fi, motion-triggered alert with a buzzer (example 05 is a working starting point), swap `ESP8266WebServer` for **ESPAsyncWebServer** once you need the page to stay responsive while doing other work.

**Advanced:** deep-sleep battery-powered sensor node (see [`docs/GPIO.md §4`](docs/GPIO.md#4-deep-sleep-wiring-for-battery-projects) for the required D0-to-RST wiring — and note the ~71-minute maximum single sleep duration, an undocumented 32-bit timer limit), MQTT integration with Home Assistant, OTA (over-the-air) firmware updates via `ArduinoOTA` so you never need the USB cable again after the first flash.

---

## ❓ FAQ

<details>
<summary><b>Can I power this from a phone charger?</b></summary>

Yes, if it's a genuine 5V/1A+ USB charger with a **data-capable** cable for the first upload. After that, any decent 5V USB power source works — the onboard AMS1117 regulator steps it down to 3.3V.
</details>

<details>
<summary><b>Why does my analogRead() never go above ~1023 no matter what?</b></summary>

That's correct and expected — the ADC is 10-bit, so its range is 0–1023 regardless of input voltage. What differs is the *voltage* mapped to that range: 1.0V on a bare ESP-12 module, ~3.2V on a NodeMCU dev board (because of the onboard divider). Don't feed A0 more than 3.3V on NodeMCU or more than 1V on a bare module.
</details>

<details>
<summary><b>My board shows up as a generic "USB-SERIAL CH340" with no NodeMCU name — is that normal?</b></summary>

Yes. The USB-to-serial chip (CP2102 or CH340) is a separate chip from the ESP8266 itself, and it only identifies itself, not the board. As long as a COM/tty port appears and you can select it in Arduino IDE, you're fine.
</details>

<details>
<summary><b>Can two NodeMCUs talk to each other without a router?</b></summary>

Yes — either put one in **AP mode** (`WiFi.softAP(...)`) and have the other connect to it directly, or use **ESP-NOW**, a connectionless peer-to-peer protocol built into the ESP8266 SDK that needs no Wi-Fi network at all and has much lower latency than a normal TCP request.
</details>

---

## 📜 License

MIT — see [`LICENSE`](LICENSE).
