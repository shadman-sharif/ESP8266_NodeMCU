# 🔌 ESP8266 NodeMCU GPIO Guide

> Pins below are for the common **NodeMCU v1.0 (ESP-12E)** board. Silkscreen labels (`D0`, `D1`...) do **not** match the actual GPIO numbers you use in code — this mismatch is the #1 beginner confusion on this board.

## 1. Silkscreen (Dx) to Code (GPIO) Mapping

| Board label | GPIO (use this number in code) | Notes |
|---|---:|---|
| D0 | 16 | No PWM, no interrupt support, and uses a **different internal register set (RTC domain)** from every other GPIO — this is why it's the only pin that can wake the chip from deep sleep. Used for deep-sleep wake (see §4) |
| D1 | 5 | Commonly used for I2C SCL |
| D2 | 4 | Commonly used for I2C SDA |
| D3 | 0 | Boot mode pin — see `docs/hardware.md` §5 |
| D4 | 2 | Boot mode pin + **onboard LED, active LOW** |
| D5 | 14 | SPI CLK |
| D6 | 12 | SPI MISO |
| D7 | 13 | SPI MOSI |
| D8 | 15 | Boot mode pin, must be LOW at boot — has onboard pull-down |
| D9 | 3 | Serial RX — avoid using while `Serial` is active |
| D10 | 1 | Serial TX — avoid using while `Serial` is active |
| A0 | — | The only analog input. 0–3.3V range on NodeMCU boards (onboard divider); raw ESP8266 chip is 0–1V only |

In Arduino code you write `digitalWrite(2, HIGH)` or `digitalWrite(D2, HIGH)` — the `D2` macro is defined by the board package and equals `4`. Mixing up the label and the number (e.g. assuming "D2" means GPIO2) is the most common wiring bug on this board.

## 2. GPIOs to avoid entirely

| GPIO | Reason |
|---|---|
| 6, 7, 8, 9, 10, 11 | Connected internally to the chip's own flash memory — not broken out on NodeMCU, but never usable even if you see them exposed on a bare ESP-12 module |
| 0, 2, 15 (D3, D4, D8) | Boot-mode pins — usable for your own I/O *after* boot, but don't let external circuitry hold them in the wrong state during power-up (see hardware.md) |

## 3. Onboard LED (D4 / GPIO2) — active LOW

```cpp
#define LED_PIN 2  // same as D4

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, LOW);   // LED ON  (inverted!)
  delay(500);
  digitalWrite(LED_PIN, HIGH);  // LED OFF
  delay(500);
}
```

This trips up almost everyone the first time — `HIGH` turns the onboard LED **off**, not on.

## 4. Deep Sleep wiring (for battery projects)

To use `ESP.deepSleep()` and have the board wake itself back up, you must physically connect:

```text
D0 (GPIO16)  →  RST pin
```

Without this wire, the chip will go to sleep and never wake up on its own — a very common "my deep sleep code doesn't work" report that's actually a missing wire, not a code bug.

> [!NOTE]
> A single `ESP.deepSleep(us)` call is reliable only up to roughly **71 minutes** (the internal timer is 32-bit, counted in microseconds, and wraps around 2^32 µs ≈ 71.58 minutes). Undocumented in most tutorials — if you need a longer interval, loop through several shorter sleeps instead of one long one.

## 5. PWM — shared frequency, not per-pin

`analogWrite()` on ESP8266 gives you **10-bit** PWM (range 0–1023, not the 0–255 you may know from classic AVR Arduino boards) — remember to scale accordingly if porting old sketches. More importantly: on the stock ESP8266 Arduino core, **all PWM pins share one global frequency** (default ~1kHz, changeable with `analogWriteFreq()`), rather than each pin having its own independent frequency like on some other microcontrollers. If you need two PWM signals at genuinely different frequencies simultaneously, you'll need software bit-banging or a dedicated PWM driver chip — the built-in `analogWrite()` can't do it.

## 6. I2C pins can be remapped

D1/D2 (GPIO5/GPIO4) are the *conventional* I2C pins on NodeMCU, but they are not hardwired — the ESP8266's I2C is bit-banged in software, so any two free GPIOs work:

```cpp
Wire.begin(4, 5); // SDA, SCL — you can pick different pins if D1/D2 are in use
```

This is worth knowing if D1/D2 are already committed to something else in a bigger project — you are not stuck using them for I2C.

## 7. Planning Your Pins

```text
Identify board variant → Note D3/D4/D8 boot constraints →
Reserve D0 if using deep sleep → Reserve D9/D10 if using Serial →
Only then assign remaining pins to sensors/actuators
```

**Realistically free GPIOs for general use:** D1, D2, D5, D6, D7, and A0 (analog). D0 if you don't need deep sleep.
