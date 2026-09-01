# 🛠️ Troubleshooting

## Upload fails

Check:

- USB data cable
- Serial port
- Board selection
- USB driver
- Power
- External circuits on boot pins

## Serial output is unreadable

Set Serial Monitor to the baud rate used by the sketch, usually:

```text
115200
```

ESP8266 boot ROM messages can use a different baud rate; that can be normal.

## Wi-Fi fails

Check:

- SSID
- Password
- 2.4 GHz availability
- Signal
- Router settings
- Power stability

## Web page fails

Check:

```text
ESP8266 connected
      ↓
Correct IP printed
      ↓
Phone/PC on same network
      ↓
Open http://IP_ADDRESS
```

## Board resets

Possible causes:

- Unstable power
- Wiring problem
- Boot-pin interference
- Sensor load
- Software crash

## LED is backwards

The onboard LED on many NodeMCU boards is active-low:

```cpp
LOW  = ON
HIGH = OFF
```
