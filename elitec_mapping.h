#include <stdint.h>
#include <avr/pgmspace.h>
#include <XInput.h>

#ifndef _ELITEC_MAPPING_
#define _ELITEC_MAPPING_

#ifndef USE_JOYSTICKS
#define USE_JOYSTICKS 0
#endif

#ifndef USE_ANALOG_TRIGGERS
#define USE_ANALOG_TRIGGERS 0
#endif

#ifndef USE_JOYSTICK_EMULATION
#define USE_JOYSTICK_EMULATION 0
#endif

struct ButtonToPinMapping {
  uint8_t portIndex;
  uint8_t portPin;
  uint8_t button;
};

#define PIN_JOY_LEFT_X   PF4
#define PIN_JOY_LEFT_Y   PF5
#define PIN_JOY_RIGHT_X  PF6
#define PIN_JOY_RIGHT_Y  PF7

#define PIN_DPAD_UP      PB1
#define PIN_DPAD_DOWN    PB3
#define PIN_DPAD_LEFT    PB2
#define PIN_DPAD_RIGHT   PB6

#define PIN_BUTTON_A     PD7
#define PIN_BUTTON_B     PE6
#define PIN_BUTTON_X     PB4
#define PIN_BUTTON_Y     PB5

#define PIN_BUTTON_LB    PB7
#define PIN_BUTTON_RB    PD5
#define PIN_BUTTON_LT    PF1
#define PIN_BUTTON_RT    PF0
#define PIN_BUTTON_LOGO  PC7

#define PIN_BUTTON_BACK  PD3
#define PIN_BUTTON_START PD2
#define PIN_BUTTON_L3    PD4
#define PIN_BUTTON_R3    PC6

// Input masks for Elite-C
// PD0 - 3/SCL, PD1 - 2/SDA need an external pullup resistor and are used for I2C communication, exclude for now
#define PORTB_INPUT_MASK 0b11111111
#define PORTC_INPUT_MASK 0b11000000
#define PORTD_INPUT_MASK 0b11111100
#define PORTE_INPUT_MASK 0b01000000
#define PORTF_INPUT_MASK 0b11110011

// Index of register state in read results
#define PORTB_INDEX 0
#define PORTC_INDEX 1
#define PORTD_INDEX 2
#define PORTE_INDEX 3
#define PORTF_INDEX 4

#define PORT_COUNT 5
#define INPUT_PIN_COUNT (sizeof(BUTTON_MAP) / sizeof(BUTTON_MAP[0]))

// Add/update pin to button mapping here
const ButtonToPinMapping BUTTON_MAP[] = {
  { PORTB_INDEX, PIN_DPAD_UP, DPAD_UP },
  { PORTB_INDEX, PIN_DPAD_DOWN, DPAD_DOWN },
  { PORTB_INDEX, PIN_DPAD_LEFT, DPAD_LEFT },
  { PORTB_INDEX, PIN_DPAD_RIGHT, DPAD_RIGHT },
  { PORTD_INDEX, PIN_BUTTON_A, BUTTON_A },
  { PORTE_INDEX, PIN_BUTTON_B, BUTTON_B },
  { PORTB_INDEX, PIN_BUTTON_X, BUTTON_X },
  { PORTB_INDEX, PIN_BUTTON_Y, BUTTON_Y },
  { PORTB_INDEX, PIN_BUTTON_LB, BUTTON_LB },
  { PORTD_INDEX, PIN_BUTTON_RB, BUTTON_RB },
  { PORTD_INDEX, PIN_BUTTON_BACK, BUTTON_BACK },
  { PORTD_INDEX, PIN_BUTTON_START, BUTTON_START },
  { PORTC_INDEX, PIN_BUTTON_LOGO, BUTTON_LOGO },
  { PORTD_INDEX, PIN_BUTTON_L3, BUTTON_L3 },
  { PORTC_INDEX, PIN_BUTTON_R3, BUTTON_R3 },
#if USE_ANALOG_TRIGGERS == 0
  { PORTF_INDEX, PIN_BUTTON_LT, TRIGGER_LEFT },
  { PORTF_INDEX, PIN_BUTTON_RT, TRIGGER_RIGHT },
#endif
};

const ButtonToPinMapping JOYSTICK_LEFT_MAP[] = {
  { PORTF_INDEX, PIN_JOY_LEFT_X, JOY_LEFT },
  { PORTF_INDEX, PIN_JOY_LEFT_Y, JOY_LEFT },
};

const ButtonToPinMapping JOYSTICK_RIGHT_MAP[] = {
  { PORTF_INDEX, PIN_JOY_RIGHT_X, JOY_RIGHT },
  { PORTF_INDEX, PIN_JOY_RIGHT_Y, JOY_RIGHT },
};

const ButtonToPinMapping ANALOG_TRIGGER_MAP[] = {
  { PORTF_INDEX, PIN_BUTTON_LT, TRIGGER_LEFT },
  { PORTF_INDEX, PIN_BUTTON_RT, TRIGGER_RIGHT },
};

/**
 * Configures all usable pins as inputs.
 */
void configureInputs() {
  // Set to input (invert mask to set to 0)
  DDRB = DDRB & ~PORTB_INPUT_MASK;
  DDRC = DDRC & ~PORTC_INPUT_MASK;
  DDRD = DDRD & ~PORTD_INPUT_MASK;
  DDRE = DDRE & ~PORTE_INPUT_MASK;
  DDRF = DDRF & ~PORTF_INPUT_MASK;

  // Set to high/pullup
  PORTB = PORTB | PORTB_INPUT_MASK;
  PORTC = PORTC | PORTC_INPUT_MASK;
  PORTD = PORTD | PORTD_INPUT_MASK;
  PORTE = PORTE | PORTE_INPUT_MASK;
  PORTF = PORTF | PORTF_INPUT_MASK;
}

/**
 * Retrieves the input states from all ports
 */
void getInputStates(uint8_t portStates[]) {
  portStates[PORTB_INDEX] = PINB;
  portStates[PORTC_INDEX] = PINC;
  portStates[PORTD_INDEX] = PIND;
  portStates[PORTE_INDEX] = PINE;
  portStates[PORTF_INDEX] = PINF;
}

#endif
