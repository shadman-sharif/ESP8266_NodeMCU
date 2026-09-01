# 🔌 GPIO Guide

> This guide targets a common ESP-12E-style NodeMCU board. Verify your exact hardware.

## Pin table

| NodeMCU | GPIO | Notes |
|---|---:|---|
| D0 | GPIO16 | Digital I/O; special limitations |
| D1 | GPIO5 | Common I²C SCL |
| D2 | GPIO4 | Common I²C SDA |
| D3 | GPIO0 | Boot strap pin |
| D4 | GPIO2 | Built-in LED on many boards; boot strap pin |
| D5 | GPIO14 | Common SPI SCLK |
| D6 | GPIO12 | Common SPI MISO |
| D7 | GPIO13 | Common SPI MOSI |
| D8 | GPIO15 | Boot strap pin |
| RX | GPIO3 | UART RX |
| TX | GPIO1 | UART TX |
| A0 | ADC0 | Analog input |

## Boot pins

GPIO0, GPIO2 and GPIO15 have startup requirements.

Do not attach external circuits that accidentally force these pins into an invalid state during reset.

## Built-in LED

Many NodeMCU boards use GPIO2 for the onboard LED and the LED is commonly active-low:

```cpp
digitalWrite(LED_BUILTIN, LOW);   // ON
digitalWrite(LED_BUILTIN, HIGH);  // OFF
```

## Digital output

```cpp
const int LED_PIN = D1;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
```

## Digital input

```cpp
const int BUTTON_PIN = D2;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("Button pressed");
  }
  delay(100);
}
```

## Analog input

```cpp
void setup() {
  Serial.begin(115200);
}

void loop() {
  int value = analogRead(A0);
  Serial.println(value);
  delay(500);
}
```

Check the exact A0 input range for your board before applying voltage.

## GPIO safety

Avoid:

- Unsupported input voltage
- Short circuits
- Driving two outputs against each other
- Incorrect boot-pin states
- Exceeding GPIO current limits
