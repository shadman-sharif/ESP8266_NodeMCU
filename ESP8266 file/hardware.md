# 🔧 ESP8266 NodeMCU Hardware Guide

## 1. Main Parts

```text
┌───────────────────────────────────┐
│           NodeMCU (ESP8266)        │
│                                     │
│   ESP8266 chip (Wi-Fi + CPU)       │
│   Onboard USB-to-serial (CP2102/CH340)
│   Onboard 3.3V regulator (AMS1117) │
│   Onboard blue LED (D4)            │
│   Onboard FLASH + RESET buttons    │
│   30 pins broken out (D0–D10, A0)  │
└───────────────────────────────────┘
```

- **ESP8266 (ESP8266EX)** — the actual chip. Single-core Tensilica L106, Wi-Fi only (no Bluetooth, no camera interface).
- **Onboard USB-serial chip** — this is why NodeMCU is beginner-friendly: no separate USB-TTL adapter needed, unlike a plain AI-Thinker ESP32-CAM.
- **No camera support.** ESP8266 has no camera peripheral (no DVP/CSI interface). If you specifically want a camera project, you need an ESP32-CAM, not NodeMCU. This guide instead covers Wi-Fi, sensors, and onboard flash storage — the things NodeMCU is actually good at.
- **No SD card slot** on standard NodeMCU boards. Instead, this guide uses the chip's **internal flash (LittleFS)** to store data — see example 04.

> [!TIP]
> **Clone boards often report the wrong flash size.** Cheap NodeMCU clones sometimes ship with 4MB flash chips that the bootloader misreports as 32MB or vice-versa. Run `esptool.py flash_id` (installed alongside the ESP8266 board package, usually under your Arduino15 tools folder) to see the *actual* chip ID and cross-reference it, rather than trusting the label on the board.

## 2. Which NodeMCU do you have?

| Variant | Notes |
|---|---|
| **NodeMCU v1.0 (ESP-12E based)** | Most common, wide board, USB-C or Micro-USB |
| **NodeMCU v3 (ESP-12 based)** | Older, narrower board, some pin differences |
| **Wemos D1 Mini** | Smaller, same ESP8266 chip, different pin labels — check silkscreen before wiring |

All examples in this repo target the common **NodeMCU v1.0 (ESP-12E, Micro-USB)** board.

## 3. Power

Much less finicky than ESP32-CAM (no camera + Wi-Fi combined draw), but still real issues exist:

- Powering from a laptop USB port is usually fine for Wi-Fi-only use.
- If you add several sensors, a servo, or relays, **power those from a separate 5V supply**, not the NodeMCU's onboard regulator — it can only supply a limited current (typically ~500mA) before it overheats or the board resets during Wi-Fi transmit.
- **Logic level is 3.3V, and GPIO pins are NOT 5V tolerant.** Connecting a 5V sensor's output pin directly to a NodeMCU GPIO can damage it — use a voltage divider or logic-level shifter for 5V sensors.
- **The "brownout reboot loop" almost nobody explains well:** Wi-Fi transmit draws a current spike of ~300–400mA for a few milliseconds. If your 3.3V rail sags below ~2.9V during that spike (weak USB port, long/thin cable, cheap regulator), the chip's internal brownout detector resets it — right when Wi-Fi connects, over and over. A **100–470 µF electrolytic capacitor** directly across the 3.3V and GND pins (as close to the chip as possible) smooths this out and fixes the majority of "reboots only when Wi-Fi connects" reports.
- **The 3V3 pin can also *source* current**, not just accept it — you can power a low-draw 3.3V sensor directly from the board's 3V3 pin, but remember it's coming from that same current-limited onboard regulator.

### Simple voltage divider for a 5V sensor output

```text
5V sensor OUT ──[10kΩ]──┬──[20kΩ]── GND
                         │
                    to NodeMCU GPIO (reads ~3.3V when sensor outputs 5V)
```

Using a 1:2 ratio (10kΩ over 20kΩ, or any pair in that ratio) safely steps a 5V logic signal down into the 3.3V-safe range for GPIO input. This only works one direction (sensor → board); for two-way (I2C) signals, use a proper bi-directional logic-level shifter module instead of a simple divider.

## 4. USB Connection

Just plug in a Micro-USB cable — no external programmer needed. If the board isn't detected:

- Install the **CP2102** or **CH340** driver depending on which USB chip your board has (check the chip printed on the board near the USB port).
- Try a different USB cable — many phone-charging-only cables have no data lines.
- On Linux, if the port appears but uploads fail with a permissions error, add your user to the `dialout` group (`sudo usermod -a -G dialout $USER`, then log out/in) rather than resorting to `sudo arduino`.

## 5. Boot Mode Pins — the ESP8266 equivalent of ESP32-CAM's IO0

Unlike ESP32-CAM, NodeMCU has an onboard auto-reset circuit, so you normally **never need to manually short any pin** to upload — Arduino IDE handles entering flash mode automatically. But a few pins still matter if you use them for your own wiring:

| Pin | GPIO | Boot behavior |
|---|---|---|
| D3 | GPIO0 | Must be HIGH (or floating) at boot for normal run mode; LOW at boot = flash mode. Don't tie this permanently LOW with your own circuit. |
| D4 | GPIO2 | Must be HIGH at boot. Also drives the onboard LED (active LOW — see `docs/GPIO.md`). |
| D8 | GPIO15 | Must be LOW at boot. Has an onboard pull-down on most NodeMCU boards — don't add your own pull-up here. |
| D9 / D10 | GPIO3 / GPIO1 | Used for Serial (RX/TX) and the boot log. Avoid using these while `Serial` is active. |

If your custom wiring holds any of these pins in the wrong state at power-on, the board will fail to boot properly or get stuck reflashing — this is a very common "why is my board stuck" cause once you start adding your own sensors.

> [!TIP]
> **Reading the boot log requires 74880 baud, not 115200.** The ESP8266's ROM bootloader prints its own diagnostic line (`rst cause`, `boot mode`) at a fixed 74880 baud before your sketch's `Serial.begin(115200)` even runs. If you only ever open the monitor at 115200, you'll see garbled characters for that first line and miss it entirely — switch the monitor to 74880 once, right after pressing reset, to actually read it. See the [README's boot-cause cheat sheet](../README.md#-boot-cause-cheat-sheet) for what the codes mean.

## 6. Antenna & Range Notes (rarely covered)

- The PCB trace antenna on NodeMCU boards is directional-ish and quite sensitive to nearby metal. Mounting the board inside a metal enclosure (a mint tin, a metal project box) can cut Wi-Fi range dramatically or prevent connection entirely — use plastic enclosures, or route the antenna area outside any metal shielding.
- A cluttered 2.4GHz environment (many neighboring Wi-Fi networks, microwave ovens, Bluetooth devices) affects ESP8266 more noticeably than phones/laptops because of its single, non-diversity antenna — if a device seems to randomly drop Wi-Fi, try a different channel on your router (channels 1, 6, and 11 avoid overlap) before assuming it's a hardware fault.

## 7. Hardware Checklist

- [ ] Confirmed board variant (NodeMCU v1.0 vs v3 vs D1 Mini — pin labels differ)
- [ ] Correct USB driver installed (CP2102/CH340)
- [ ] Using a data-capable USB cable
- [ ] No external wiring holding D3/D4/D8 in the wrong state at boot
- [ ] Any 5V sensor uses a level shifter or divider, not a direct GPIO connection
- [ ] Extra current-hungry components (motors, many LEDs, relays) powered separately, not from the board's 3.3V regulator
- [ ] A smoothing capacitor (100–470 µF) across 3.3V/GND if you're seeing Wi-Fi-connect reboots
