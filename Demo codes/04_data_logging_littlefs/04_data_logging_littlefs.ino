/*
  ESP8266 NodeMCU - Example 04
  Data Logging to Internal Flash (LittleFS)

  NodeMCU has no SD card slot, so this uses the chip's own internal
  flash memory as a small filesystem instead - good enough for
  sensor logs, settings, or small data files.

  What it does:
  - Every time you send anything in the Serial Monitor, it appends
    one line (timestamp + A0 reading) to /log.txt
  - Type "read" and press Enter to print the whole log file back

  Before uploading:
  1. Board: "NodeMCU 1.0 (ESP-12E Module)"
  2. Tools -> Flash Size: choose an option that reserves filesystem
     space, e.g. "4MB (FS:1MB OTA:~1019KB)" - see docs/software.md
*/

#include <LittleFS.h>

const char* LOG_FILE = "/log.txt";

void appendLogLine() {
  int analogValue = analogRead(A0);
  unsigned long t = millis();

  File file = LittleFS.open(LOG_FILE, "a"); // "a" = append
  if (!file) {
    Serial.println("Failed to open log file for writing.");
    return;
  }

  String line = String(t) + "ms, A0=" + String(analogValue);
  file.println(line);
  file.close();

  Serial.println("Logged: " + line);
}

void printLogFile() {
  if (!LittleFS.exists(LOG_FILE)) {
    Serial.println("No log file yet - send anything first to create one.");
    return;
  }

  File file = LittleFS.open(LOG_FILE, "r");
  Serial.println("----- log.txt -----");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("----- end -----");
  file.close();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting LittleFS logging demo...");

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, formatting...");
    LittleFS.format();
    LittleFS.begin();
  }

  Serial.println("LittleFS ready.");
  Serial.println("Type anything + Enter to log a reading.");
  Serial.println("Type 'read' + Enter to print the log file.");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "read") {
      printLogFile();
    } else if (input.length() > 0) {
      appendLogLine();
    }
  }
}
