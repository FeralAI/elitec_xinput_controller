// Elite-C Arcade Stick
//
// This sketch handles an arcade stick with 1 4/8-way joystick and 10 digital buttons (14 pins).
// Enabling the DEBUG flag will output to a 128x32 OLED connected via I2C since serial monitoring is unavailable in XInput mode.

//#define DEBUG
#define DEBOUNCE_MICROSECONDS 1
#define USE_JOYSTICKS 0
#define USE_ANALOG_TRIGGERS 0
#define USE_JOYSTICK_EMULATION 0

#ifdef DEBUG
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif
#include <XInput.h>
#include "elitec_mapping.h"
#include "limits.h"

// State variables
uint8_t portStates[PORT_COUNT];             // The current port register values
uint8_t dpadStates[4] = { };                // The dpad input states
uint8_t buttonStates[BUTTON_COUNT] = { };   // The button states
uint8_t triggerStates[2] = { };             // The analog trigger states
uint16_t joystickStatesX[2] = { };          // The left joystick states
uint16_t joystickStatesY[2] = { };          // The right joystick states
#ifdef DEBUG
uint32_t readTime[3] = { ULONG_MAX, 0, 0 };
uint32_t parseTime[3] = { ULONG_MAX, 0, 0 };
uint32_t loopTime[3] = { ULONG_MAX, 0, 0 };
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#endif

inline void setButton(ButtonToPinMapping mapping, uint8_t lastButtonStates[]) __attribute__((always_inline));

void setButton(ButtonToPinMapping mapping, uint8_t lastButtonStates[]) {
  buttonStates[mapping.stateIndex] = (portStates[mapping.portIndex] >> mapping.portPin & 1);
  if (buttonStates[mapping.stateIndex] != lastButtonStates[mapping.stateIndex])
    XInput.setButton(mapping.button, !buttonStates[mapping.stateIndex]);
}

void setup() {
#ifdef DEBUG
  setupDisplay();
#endif
  configureInputs();
}

void loop() {
#ifdef DEBUG
  uint32_t loopStartTime = micros();
  uint32_t startTime = loopStartTime;
#endif

  // Read logic takes 4-16µs
  getInputStates(portStates);

#ifdef DEBUG
  readTime[2] = micros() - startTime;
  startTime = micros();
#endif

  // Cache previous states for this loop
  uint8_t lastDpadStates[4];
  uint8_t lastButtonStates[BUTTON_COUNT];
  memcpy(lastDpadStates, dpadStates, sizeof(lastDpadStates));
  memcpy(lastButtonStates, buttonStates, sizeof(lastButtonStates));

  // Clear the current states
  // memset(dpadStates, 0, sizeof(dpadStates));
  // memset(buttonStates, 0, sizeof(buttonStates));

  // Get/set directional inputs
  dpadStates[MapDpadUp.stateIndex]    = (portStates[MapDpadUp.portIndex] >> MapDpadUp.portPin & 1);
  dpadStates[MapDpadDown.stateIndex]  = (portStates[MapDpadDown.portIndex] >> MapDpadDown.portPin & 1);
  dpadStates[MapDpadLeft.stateIndex]  = (portStates[MapDpadLeft.portIndex] >> MapDpadLeft.portPin & 1);
  dpadStates[MapDpadRight.stateIndex] = (portStates[MapDpadRight.portIndex] >> MapDpadRight.portPin & 1);
  if (memcmp(lastDpadStates, dpadStates, sizeof(lastDpadStates)) != 0) {
    XInput.setDpad(
      dpadStates[MapDpadUp.stateIndex] == 0,
      dpadStates[MapDpadDown.stateIndex] == 0,
      dpadStates[MapDpadLeft.stateIndex] == 0,
      dpadStates[MapDpadRight.stateIndex] == 0,
      true
    );
  }

  // Get/set button inputs
  setButton(MapButtonStart, lastButtonStates);
  setButton(MapButtonBack, lastButtonStates);
  setButton(MapButtonL3, lastButtonStates);
  setButton(MapButtonR3, lastButtonStates);
  setButton(MapButtonLB, lastButtonStates);
  setButton(MapButtonRB, lastButtonStates);
  setButton(MapButtonLogo, lastButtonStates);
  setButton(MapButtonA, lastButtonStates);
  setButton(MapButtonB, lastButtonStates);
  setButton(MapButtonX, lastButtonStates);
  setButton(MapButtonY, lastButtonStates);
  setButton(MapButtonLT, lastButtonStates);
  setButton(MapButtonRT, lastButtonStates);

#ifdef DEBUG
  uint32_t endTime = micros();
  parseTime[2] = endTime - startTime;
  loopTime[2] = endTime - loopStartTime;
  loopTime[0] = min(loopTime[0], loopTime[2]);
  loopTime[1] = max(loopTime[1], loopTime[2]);
  readTime[0] = min(readTime[0], readTime[2]);
  readTime[1] = max(readTime[1], readTime[2]);
  parseTime[0] = min(parseTime[0], parseTime[2]);
  parseTime[1] = max(parseTime[1], parseTime[2]);
  printState();
#endif
  // TODO: Adjust debounce delay, getting multiple presses on 240Hz display
  // delayMicroseconds(DEBOUNCE_MICROSECONDS);
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
  display.print(" ");
  display.print(readTime[1]);
  display.print(" ");
  display.println(readTime[2]);

  display.print("P: ");
  display.print(parseTime[0]);
  display.print(" ");
  display.print(parseTime[1]);
  display.print(" ");
  display.println(parseTime[2]);

  display.print("L: ");
  display.print(loopTime[0]);
  display.print(" ");
  display.print(loopTime[1]);
  display.print(" ");
  display.println(loopTime[2]);

  display.print("S: ");
  for (int i = 0; i < 4; i++)
    display.print(dpadStates[i] ? "0" : "1");
  for (int i = 0; i < BUTTON_COUNT; i++)
    display.print(buttonStates[i] ? "0" : "1");
  display.println();

  display.display();
}
#endif
