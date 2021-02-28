// Elite-C Arcade Stick
//
// This sketch handles an arcade stick with 1 4/8-way joystick and 10 digital buttons (14 pins).
// Enabling the DEBUG flag will output to a 128x32 OLED since serial monitoring is unavailable in XInput mode.

//#define DEBUG
#define DEBOUNCE_MICROSECONDS 5

#ifdef DEBUG
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif
#include <XInput.h>
#include "elitec_mapping.h"

const uint8_t inputPinCount = sizeof(BUTTON_MAP) / sizeof(BUTTON_MAP[0]);

// State variables
uint16_t pressed = 0;
uint8_t registerStates[PORT_COUNT];
#ifdef DEBUG
unsigned long startTime = 0;
unsigned long readTime[3] = { ULONG_MAX, 0, 0 };
unsigned long parseTime[3] = { ULONG_MAX, 0, 0 };
unsigned long loopTime[3] = { ULONG_MAX, 0, 0 };
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#endif

void setup() {
#ifdef DEBUG
  setupDisplay();
#endif
  configureInputs();
}

void loop() {
#ifdef DEBUG
  unsigned long loopStartTime = micros();
#endif
  readRegisters();
  parseInputs();
#ifdef DEBUG
  loopTime[2] = micros() - loopStartTime;
  loopTime[0] = min(loopTime[0], loopTime[2]);
  loopTime[1] = max(loopTime[1], loopTime[2]);
  readTime[0] = min(readTime[0], readTime[2]);
  readTime[1] = max(readTime[1], readTime[2]);
  parseTime[0] = min(parseTime[0], parseTime[2]);
  parseTime[1] = max(parseTime[1], parseTime[2]);
  printState();
#endif
  // TODO: Adjust debounce delay, getting multiple presses on 240Hz display
  delayMicroseconds(DEBOUNCE_MICROSECONDS);
}

// Read logic takes 4-16µs
void readRegisters() {
#ifdef DEBUG
  startTime = micros();
#endif
  getInputStates(registerStates);
#ifdef DEBUG
  readTime[2] = micros() - startTime;
#endif
}

// Parsing logic takes 120-164µs
// XInput calls add ~50-100µs
void parseInputs() {
#ifdef DEBUG
  startTime = micros();
#endif
  uint16_t lastPressed = pressed;
  pressed = 0;
  for (int i = 0; i < inputPinCount; i++) {
    // If HIGH, not pressed
    if (registerStates[BUTTON_MAP[i].portIndex] >> BUTTON_MAP[i].portPin & 1) {
      pressed &= ~(1 << i);          // Set bit to 0
      if (lastPressed & (1 << i))    // Check if last pressed bit is 1
        XInput.setButton(BUTTON_MAP[i].button, false);
    } else {
      pressed |= (1 << i);           // Set bit to 1
      if (!(lastPressed & (1 << i))) // Check if last pressed bit is 0
        XInput.setButton(BUTTON_MAP[i].button, true);
    }
  }
#ifdef DEBUG
  parseTime[2] = micros() - startTime;
#endif
}

#ifdef DEBUG
void setupDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void printState() {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("R: ");
  display.print(readTime[0]);
  display.print(", ");
  display.print(readTime[1]);
  display.print(", ");
  display.println(readTime[2]);

  display.print("P: ");
  display.print(parseTime[0]);
  display.print(", ");
  display.print(parseTime[1]);
  display.print(", ");
  display.println(parseTime[2]);

  display.print("L: ");
  display.print(loopTime[0]);
  display.print(", ");
  display.print(loopTime[1]);
  display.print(", ");
  display.println(loopTime[2]);

  display.print("S: ");
  for (int i = 0; i < inputPinCount; i++)
     display.print(pressed & (1 << i) ? "1" : "0");
  display.println();

  display.display();
}
#endif
