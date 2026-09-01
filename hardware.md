# 🔧 Hardware Guide

## ESP8266 NodeMCU

A common NodeMCU development board contains:

```text
┌──────────────────────────────────┐
│          ESP8266 NodeMCU         │
│                                  │
│   ┌──────────────────────────┐   │
│   │       ESP8266 / ESP-12    │   │
│   │       Wi-Fi MCU           │   │
│   └──────────────────────────┘   │
│                                  │
│ USB → USB/UART → ESP8266         │
│                                  │
│ GPIO headers + A0                │
└──────────────────────────────────┘
```

## Main parts

### ESP8266
The processor and Wi-Fi controller.

### USB interface
Most NodeMCU development boards include a USB-to-UART bridge, allowing programming directly from a computer.

### Voltage regulation
The development board normally provides regulated 3.3 V for the ESP8266.

### GPIO
Digital pins for LEDs, buttons, sensors and other peripherals.

### ADC
The ESP8266 provides one ADC channel. The voltage range exposed at the NodeMCU A0 pin depends on the specific board's input circuitry.

## Logic voltage

ESP8266 GPIO is 3.3 V logic. Check the voltage requirements of every external module before connecting it.

## Power

USB is normally the easiest power source for a development board. For external circuits, use an appropriate supply and common ground.

## Hardware checklist

- [ ] Identify board revision
- [ ] Check pin labels
- [ ] Check 3.3 V logic compatibility
- [ ] Identify boot pins
- [ ] Check sensor voltage
- [ ] Use common ground
- [ ] Avoid short circuits
