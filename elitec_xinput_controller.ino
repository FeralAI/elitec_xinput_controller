/******************************************************************************
 * Elite-C XInput Controller
 * 
 * A full XInput controller contains the following inputs:
 * - 10 + 1 digital buttons
 * - 4-way directional input
 * - 2 analog joysticks (16-bit)
 * - 2 analog/digital triggers (8-bit)
 * 
 * This sketch currently handles all digital inputs, though all required analog
 * pins are currently allocated until analog reads are implemented.
 * 
 * Enabling the DEBUG flag will output to a 128x32 OLED connected via I2C
 * since serial monitoring is unavailable in XInput mode.
 * 
 * If building from source, make sure you are using the -O3 or -Ofast
 * optimization flags to ensure the fastest performance.
 *****************************************************************************/

#define DEBUG
#define USE_JOYSTICKS 0
#define USE_ANALOG_TRIGGERS 0
#define USE_JOYSTICK_EMULATION 0
#define USE_SOCD 1

#ifdef DEBUG
#include "ssd1306.h"
#endif
#include <XInput.h>
#include "elitec_mapping.h"
#include "limits.h"

// State variables
uint8_t dpadStates[4] = { };              // The dpad input states
uint8_t buttonStates[BUTTON_COUNT] = { }; // The button states
uint8_t triggerStates[2] = { };           // The analog trigger states
uint16_t joystickStatesX[2] = { };        // The left joystick states
uint16_t joystickStatesY[2] = { };        // The right joystick states
#ifdef DEBUG
uint32_t readTime[3] = { ULONG_MAX, 0, 0 };
uint32_t parseTime[3] = { ULONG_MAX, 0, 0 };
uint32_t loopTime[3] = { ULONG_MAX, 0, 0 };
#endif

inline void setButton(ButtonToPinMapping mapping, uint8_t portStates[], uint8_t lastButtonStates[]) __attribute__((always_inline));

void setButton(ButtonToPinMapping mapping, uint8_t portStates[], uint8_t lastButtonStates[]) {
  buttonStates[mapping.stateIndex] = (portStates[mapping.portIndex] >> mapping.portPin & 1);
  if (buttonStates[mapping.stateIndex] != lastButtonStates[mapping.stateIndex])
    XInput.setButton(mapping.button, buttonStates[mapping.stateIndex] == 0);
}

void setup() {
  configureInputs();
#ifdef DEBUG
  setupDisplay();
#endif
}

void loop() {
#ifdef DEBUG
  uint32_t loopStartTime = micros();
  uint32_t startTime = loopStartTime;
#endif

  // Read logic takes 4-16µs
  uint8_t portStates[PORT_COUNT];
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

  // Get/set directional inputs
  dpadStates[MapDpadUp.stateIndex]    = (portStates[MapDpadUp.portIndex] >> MapDpadUp.portPin & 1);
  dpadStates[MapDpadDown.stateIndex]  = (portStates[MapDpadDown.portIndex] >> MapDpadDown.portPin & 1);
  dpadStates[MapDpadLeft.stateIndex]  = (portStates[MapDpadLeft.portIndex] >> MapDpadLeft.portPin & 1);
  dpadStates[MapDpadRight.stateIndex] = (portStates[MapDpadRight.portIndex] >> MapDpadRight.portPin & 1);
  if (memcmp(lastDpadStates, dpadStates, sizeof(lastDpadStates)) != 0) {
#if USE_SOCD
    bool up = !dpadStates[MapDpadUp.stateIndex];
    bool down = !dpadStates[MapDpadDown.stateIndex];
    bool left = !dpadStates[MapDpadLeft.stateIndex];
    bool right = !dpadStates[MapDpadRight.stateIndex];
    if (up && down) {
      down = false;
    }
    if (left && right) {
      left = false;
      right = false;
    }
    XInput.setDpad(up, down, left, right, false);
#else
    XInput.setDpad(
      dpadStates[MapDpadUp.stateIndex] == 0,
      dpadStates[MapDpadDown.stateIndex] == 0,
      dpadStates[MapDpadLeft.stateIndex] == 0,
      dpadStates[MapDpadRight.stateIndex] == 0,
      false
    );
#endif
  }

  // Get/set button inputs
  setButton(MapButtonA, portStates, lastButtonStates);
  setButton(MapButtonB, portStates, lastButtonStates);
  setButton(MapButtonX, portStates, lastButtonStates);
  setButton(MapButtonY, portStates, lastButtonStates);
  setButton(MapButtonStart, portStates, lastButtonStates);
  setButton(MapButtonBack, portStates, lastButtonStates);
  setButton(MapButtonL3, portStates, lastButtonStates);
  setButton(MapButtonR3, portStates, lastButtonStates);
  setButton(MapButtonLB, portStates, lastButtonStates);
  setButton(MapButtonRB, portStates, lastButtonStates);
  setButton(MapButtonLogo, portStates, lastButtonStates);
  setButton(MapButtonLT, portStates, lastButtonStates);
  setButton(MapButtonRT, portStates, lastButtonStates);

#ifdef DEBUG
  uint32_t endTime = micros();
  parseTime[2] = endTime - startTime;
  parseTime[0] = min(parseTime[0], parseTime[2]);
  parseTime[1] = max(parseTime[1], parseTime[2]);
  loopTime[2] = endTime - loopStartTime;
  loopTime[0] = min(loopTime[0], loopTime[2]);
  loopTime[1] = max(loopTime[1], loopTime[2]);
  readTime[0] = min(readTime[0], readTime[2]);
  readTime[1] = max(readTime[1], readTime[2]);
  printState();
#endif
}

#ifdef DEBUG
void setupDisplay() {
  ssd1306_128x32_i2c_init();
  ssd1306_clearScreen();
  ssd1306_setFixedFont(ssd1306xled_font6x8);
}

void printState() {
  char buf[20];
  sprintf(buf, "R: %lu %lu %lu", readTime[0], readTime[1], readTime[2]);
  sprintf(buf, "%-20s", buf);
  ssd1306_printFixed(0, 0, buf, STYLE_NORMAL);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "P: %lu %lu %lu", parseTime[0], parseTime[1], parseTime[2]);
  sprintf(buf, "%-20s", buf);
  ssd1306_printFixed(0, 8, buf, STYLE_NORMAL);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "L: %lu %lu %lu", loopTime[0], loopTime[1], loopTime[2]);
  sprintf(buf, "%-20s", buf);
  ssd1306_printFixed(0, 16, buf, STYLE_NORMAL);
  memset(buf, 0, sizeof(buf));

  buf[0] = 'S';
  buf[1] = ':';
  buf[2] = ' ';
  for (int i = 3; i < 7; i++)
    buf[i] = dpadStates[i - 3] ? '0' : '1';
  for (int i = 7; i < 20; i++)
    buf[i] = buttonStates[i - 7] ? '0' : '1';
  ssd1306_printFixed(0, 24, buf, STYLE_NORMAL);     
}
#endif
