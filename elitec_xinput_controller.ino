/******************************************************************************
 * Elite-C XInput Controller v0.1.0
 * 
 * A full XInput controller contains the following inputs:
 * - 10 + 1 digital buttons
 * - 4-way directional input
 * - 2 analog joysticks (16-bit)
 * - 2 analog/digital triggers (8-bit)
 * 
 * Enabling the DEBUG flag will output to a 128x32 OLED connected via I2C
 * since serial monitoring is unavailable in XInput mode.
 * 
 * If building from source, make sure you are using the -O3 or -Ofast
 * optimization flags to ensure the fastest performance.
 *****************************************************************************/

//#define DEBUG
#define USE_JOYSTICKS 0
#define USE_ANALOG_TRIGGERS 0
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
#if USE_JOYSTICKS
uint16_t joystickStates[4] = { };         // The joystick states: 0 = Lx, 1 = Ly, 2 = Rx, 3 = Ry
#endif
#if USE_ANALOG_TRIGGERS
uint8_t triggerStates[2] = { };           // The analog trigger states
#endif
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

#if USE_JOYSTICKS
inline void setJoystickValues(ButtonToPinMapping mappingX, ButtonToPinMapping mappingY, uint16_t valueX, uint16_t valueY, uint16_t lastStates[]) __attribute__((always_inline));

void setJoystickValues(ButtonToPinMapping mappingX, ButtonToPinMapping mappingY, uint16_t valueX, uint16_t valueY, uint16_t lastStates[]) {
  joystickStates[mappingX.stateIndex] = applyJoystickDeadzone(valueX);
  joystickStates[mappingY.stateIndex] = applyJoystickDeadzone(valueY);
  if (joystickStates[mappingX.stateIndex] != lastStates[mappingX.stateIndex] || joystickStates[mappingY.stateIndex] != lastStates[mappingY.stateIndex])
    XInput.setJoystick(mappingX.button, joystickStates[mappingX.stateIndex], joystickStates[mappingY.stateIndex]);
}
#endif

#if USE_ANALOG_TRIGGERS
inline void setTriggerValue(ButtonToPinMapping mapping, uint8_t value, uint8_t lastStates[]) __attribute__((always_inline));

void setTriggerValue(ButtonToPinMapping mapping, uint8_t value, uint8_t lastStates[]) {
  triggerStates[mapping.stateIndex] = value;
  if (triggerStates[mapping.stateIndex] != lastStates[mapping.stateIndex])
    XInput.setTrigger(mapping.button, triggerStates[mapping.stateIndex]);
}
#endif

void setup() {
  configureInputs();
#ifdef DEBUG
  setupDisplay();
#endif
#if USE_JOYSTICKS || USE_ANALOG_TRIGGERS
  // This will prevent message backlog from constantly sending analog events
  XInput.setAutoSend(false);
#endif
}

void loop() {
#ifdef DEBUG
  uint32_t loopStartTime = micros();
  uint32_t startTime = loopStartTime;
#endif

  // Read logic takes 1-12µs
  uint8_t portStates[PORT_COUNT];
  getPortStates(portStates);

#ifdef DEBUG
  readTime[2] = micros() - startTime;
  startTime = micros();
#endif

  // Cache previous states for this loop
  uint8_t lastDpadStates[4];
  memcpy(lastDpadStates, dpadStates, sizeof(lastDpadStates));
  uint8_t lastButtonStates[BUTTON_COUNT];
  memcpy(lastButtonStates, buttonStates, sizeof(lastButtonStates));
#if USE_JOYSTICKS
  uint16_t lastJoystickStates[4];
  memcpy(lastJoystickStates, joystickStates, sizeof(lastJoystickStates));
#endif
#if USE_ANALOG_TRIGGERS
  uint8_t lastTriggerStates[2];
  memcpy(lastTriggerStates, triggerStates, sizeof(lastTriggerStates));
#endif

  // Dpad values
  dpadStates[MapDpadUp.stateIndex]    = (portStates[MapDpadUp.portIndex] >> MapDpadUp.portPin & 1);
  dpadStates[MapDpadDown.stateIndex]  = (portStates[MapDpadDown.portIndex] >> MapDpadDown.portPin & 1);
  dpadStates[MapDpadLeft.stateIndex]  = (portStates[MapDpadLeft.portIndex] >> MapDpadLeft.portPin & 1);
  dpadStates[MapDpadRight.stateIndex] = (portStates[MapDpadRight.portIndex] >> MapDpadRight.portPin & 1);
  if (memcmp(lastDpadStates, dpadStates, sizeof(lastDpadStates)) != 0) {
#if USE_SOCD
    XInput.setDpad(
      dpadStates[MapDpadUp.stateIndex] == 0,
      dpadStates[MapDpadDown.stateIndex] == 0,
      dpadStates[MapDpadLeft.stateIndex] == 0,
      dpadStates[MapDpadRight.stateIndex] == 0,
      true
    );
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

  // Button values
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

  // Trigger values
#if USE_ANALOG_TRIGGERS
  setTriggerValue(MapAnalogLT, analogRead(PIN_ANALOG_LT), lastTriggerStates);
  setTriggerValue(MapAnalogRT, analogRead(PIN_ANALOG_RT), lastTriggerStates);
#else
  setButton(MapButtonLT, portStates, lastButtonStates);
  setButton(MapButtonRT, portStates, lastButtonStates);
#endif

  // Joystick values
#if USE_JOYSTICKS
  setJoystickValues(MapJoystickLeftX, MapJoystickLeftY, analogRead(PIN_ANALOG_LX), analogRead(PIN_ANALOG_LY), lastJoystickStates);
  setJoystickValues(MapJoystickRightX, MapJoystickRightY, analogRead(PIN_ANALOG_RX), analogRead(PIN_ANALOG_RY), lastJoystickStates);
#endif

#if USE_JOYSTICKS || USE_ANALOG_TRIGGERS
  // This will prevent message backlog from constantly sending analog events
  XInput.send();
#endif

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
  
//  sprintf(buf, "L: %lu %lu %lu", loopTime[0], loopTime[1], loopTime[2]);
//  sprintf(buf, "%-20s", buf);
//  ssd1306_printFixed(0, 0, buf, STYLE_NORMAL);

//  memset(buf, 0, sizeof(buf));
//  sprintf(buf, "J: %u %u %u %u", joystickStates[0], joystickStates[1], joystickStates[2], joystickStates[3]);
//  sprintf(buf, "%-21s", buf);
//  ssd1306_printFixed(0, 8, buf, STYLE_NORMAL);

//  memset(buf, 0, sizeof(buf));
//  sprintf(buf, "T: %u %u", triggerStates[0], triggerStates[1]);
//  sprintf(buf, "%-21s", buf);
//  ssd1306_printFixed(0, 16, buf, STYLE_NORMAL);

  memset(buf, 0, sizeof(buf));
  buf[0] = 'S';
  buf[1] = ':';
  buf[2] = ' ';
  for (int i = 3; i < 7; i++)
    buf[i] = dpadStates[i - 3] ? '0' : '1';
  for (int i = 7; i < 7 + BUTTON_COUNT; i++)
    buf[i] = buttonStates[i - 7] ? '0' : '1';
  ssd1306_printFixed(0, 24, buf, STYLE_NORMAL);
}
#endif
