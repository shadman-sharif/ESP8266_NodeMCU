# 🛠️ ESP8266 NodeMCU Troubleshooting (Real Errors & Fixes)

## 1. "Failed to connect to ESP8266: Timed out waiting for packet header"

- Try a different, known-good **data** USB cable (many cheap cables are charge-only).
- Hold the onboard **FLASH** button while the IDE says "Connecting..." on boards where the auto-reset circuit is flaky — release it once upload starts.
- Lower Upload Speed to 115200 in the Tools menu.
- Make sure no external wiring is holding D3, D4, or D8 in a state that blocks boot (see `docs/hardware.md` §5).
- Install/reinstall the CP2102 or CH340 USB driver for your OS.

## 2. Board resets randomly, especially right when Wi-Fi connects

```text
Soft WDT reset
...
ets Jul 29 2019 ...
```

**Cause:** usually power, not code. Wi-Fi transmit draws a short current spike.

**Fix:**
- Use a good quality USB cable and port (avoid USB hubs without their own power).
- If other components (sensors, relays, motors) are attached, give them a separate power supply — don't draw everything through the NodeMCU's onboard regulator.
- Add a **100–470 µF** capacitor across 3.3V/GND, close to the chip (see `docs/hardware.md` §3) — this fixes the majority of reports that look identical to this one.

## 3. "Soft WDT reset" / "Fatal exception 28" while your code is running (not at boot)

This means the watchdog killed your program because `loop()` didn't return in time — usually a `while(...)` loop waiting on a condition that never becomes true.

**Fix:** add `delay(1)` or `yield()` inside any loop that waits on something, or restructure so `loop()` returns quickly and gets called again. For an unavoidable long blocking section, call `ESP.wdtFeed()` to reset the countdown directly.

**Also try:** paste the full exception block into **Tools → ESP Exception Decoder** in Arduino IDE — it turns the raw hex addresses into the actual function and line number that crashed. See `docs/software.md §9`.

## 4. LittleFS: `LittleFS.begin()` returns false, or files don't save

- The Flash Size setting in Tools menu must reserve space for a filesystem (e.g. "4MB (FS:1MB OTA:~1019KB)"). If the filesystem size shown is 0, no space was reserved — change this setting and re-upload.
- The very first time, `LittleFS.begin(true)` (with `true` to auto-format) is often needed on a brand-new board that has never had a filesystem written to it.
- If it worked before and suddenly stopped after uploading a *different* project to the same board, the new sketch's Flash Size setting may not match — LittleFS partition boundaries are defined by that setting, and switching it between sketches without reformatting can corrupt the filesystem. Do a one-time **"Erase Flash: All Flash Contents"** upload to reset it cleanly (`docs/software.md §2`).

## 5. Onboard LED behaves "backwards"

Not a bug — GPIO2 (D4) drives the LED in **active-LOW** — see `docs/GPIO.md` §3. `digitalWrite(LED_PIN, LOW)` turns it on.

## 6. Web page / dashboard won't load in the browser

- Confirm the IP address printed to Serial Monitor after "WiFi connected."
- Phone/PC must be on the **same 2.4GHz Wi-Fi network** — ESP8266 does not support 5GHz networks at all.
- Router "AP/client isolation" setting can block phone-to-device traffic on the same Wi-Fi — try a mobile hotspot to confirm this is the cause.
- If it loaded fine once and then randomly stops responding after minutes/hours of uptime, you're likely hitting the connection-limit issue in §7 below, or a slow memory leak (§9).

## 7. Web page loads for a while, then stops responding (no crash, no reboot)

`ESP8266WebServer` is single-threaded and handles roughly **4–5 concurrent TCP connections** before new ones start silently queuing or timing out. Browsers routinely open several connections at once for one page load (main HTML + favicon + any extra fetches), and if a client doesn't close its connection cleanly (a phone that locks its screen mid-request is a classic case), that slot can stay tied up.

**Fix:**
- Keep pages small and avoid extra requests (no favicon fetch, no separate CSS/JS files — inline everything, as the examples in this repo do).
- Add `server.client().setNoDelay(true)` and make sure your handler always calls `server.send()` — a handler that returns without sending a response leaves the connection hanging.
- For anything with more than a couple of simultaneous clients, switch to **ESPAsyncWebServer** (`docs/software.md §3`), which doesn't have this single-request-at-a-time limitation.

## 8. Sensor readings are garbage or always the same value

- Double-check you wired to the **GPIO number**, not the silkscreen `Dx` label, in code that uses raw pin numbers (see `docs/GPIO.md` §1).
- For I2C sensors, confirm SDA/SCL aren't swapped (D2=SDA/GPIO4, D1=SCL/GPIO5 by NodeMCU convention).
- A0 is the only analog pin and it's shared — if using ESP-NOW or certain Wi-Fi library features alongside ADC reads, some combinations can produce noisy readings; average a few readings together.
- A "stuck" PIR or digital sensor reading the same value forever, even when you physically trigger it, often means the sensor is wired to one of the boot pins (D3/D4/D8) and something else in your circuit is holding it in a fixed state — move it to a free pin listed in `docs/GPIO.md §5`.

## 9. Random crashes after hours/days, not at boot

This is almost always **heap fragmentation**, not a sudden hardware fault — genuinely underdocumented for beginners.

**Cause:** the ESP8266 has a small heap (well under 50KB usable in a typical Wi-Fi + web server sketch). Repeated `String` concatenation (`html += "...";` in a loop, as the web server examples do for simplicity) allocates and frees memory constantly, and over many hours this can fragment the heap badly enough that even a small allocation fails, even though `ESP.getFreeHeap()` still reports plenty of *total* free memory.

**Fix / diagnosis:**
- Log `ESP.getFreeHeap()` periodically to Serial and watch it over a long run — a heap that trends steadily downward (rather than staying roughly flat) confirms a leak or fragmentation, not a false alarm.
- For pages you serve often, build the HTML once into a `const char*` template instead of rebuilding a new `String` every request, or switch to `server.sendContent()` in chunks.
- Restarting the board on a schedule (e.g., once every 24h via `ESP.restart()`) is a legitimate, commonly used workaround for long-running projects rather than a failure to fix the root cause.

## 10. Board gets warm / regulator gets hot under load

The onboard 3.3V regulator is rated for roughly 500mA. If you're powering sensors, an OLED, and a relay all from the 3.3V pin, you can exceed this. Power extra components from an external 3.3V/5V supply (with common ground) instead.

## 11. Deep sleep: board sleeps but never wakes up

`ESP.deepSleep()` only works if **D0 (GPIO16) is physically wired to the RST pin** — without that wire, the timer expires internally but there's no way to signal the reset circuit to wake the chip back up. See `docs/GPIO.md §4`.

**Also note:** `ESP.deepSleep()` takes microseconds as a `uint64_t`-ish value, but due to a long-standing ESP8266 SDK limitation, a single sleep call reliably supports **up to roughly 71 minutes** (2^32 microseconds) before it wraps/misbehaves — for longer intervals, wake briefly and immediately go back to sleep in a loop rather than trying to sleep for the full duration in one call.
