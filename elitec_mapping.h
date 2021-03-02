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
#define PIN_BUTTON_RB    PB0
#define PIN_BUTTON_LT    PF1
#define PIN_BUTTON_RT    PF0
#define PIN_BUTTON_BACK  PD3
#define PIN_BUTTON_START PD2
#define PIN_BUTTON_LOGO  PC7
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
#define BUTTON_COUNT 13

struct ButtonToPinMapping {
  constexpr ButtonToPinMapping(uint8_t i, uint8_t p, uint8_t b, uint8_t s)
    : portIndex(i), portPin(p), bitmask(BuildMask(p)), button(b), stateIndex(s) { }
  uint8_t portIndex;
  uint8_t portPin;
  uint8_t bitmask;
  uint8_t button;
  uint8_t stateIndex;

private:
  constexpr static uint8_t BuildMask(uint8_t offset) {
    return (1 << offset);
  }
};

const ButtonToPinMapping MapDpadUp(PORTB_INDEX, PIN_DPAD_UP, DPAD_UP, 0);
const ButtonToPinMapping MapDpadDown(PORTB_INDEX, PIN_DPAD_DOWN, DPAD_DOWN, 1);
const ButtonToPinMapping MapDpadLeft(PORTB_INDEX, PIN_DPAD_LEFT, DPAD_LEFT, 2);
const ButtonToPinMapping MapDpadRight(PORTB_INDEX, PIN_DPAD_RIGHT, DPAD_RIGHT, 3);
const ButtonToPinMapping MapButtonStart(PORTD_INDEX, PIN_BUTTON_START, BUTTON_START, 4);
const ButtonToPinMapping MapButtonBack(PORTD_INDEX, PIN_BUTTON_BACK, BUTTON_BACK, 5);
const ButtonToPinMapping MapButtonL3(PORTD_INDEX, PIN_BUTTON_L3, BUTTON_L3, 6);
const ButtonToPinMapping MapButtonR3(PORTC_INDEX, PIN_BUTTON_R3, BUTTON_R3, 7);
const ButtonToPinMapping MapButtonLB(PORTB_INDEX, PIN_BUTTON_LB, BUTTON_LB, 8);
const ButtonToPinMapping MapButtonRB(PORTB_INDEX, PIN_BUTTON_RB, BUTTON_RB, 9);
const ButtonToPinMapping MapButtonLogo(PORTC_INDEX, PIN_BUTTON_LOGO, BUTTON_LOGO, 10);
const ButtonToPinMapping MapButtonLT(PORTF_INDEX, PIN_BUTTON_LT, TRIGGER_LEFT, 11);
const ButtonToPinMapping MapButtonRT(PORTF_INDEX, PIN_BUTTON_RT, TRIGGER_RIGHT, 12);

const ButtonToPinMapping MapButtonA(PORTD_INDEX, PIN_BUTTON_A, BUTTON_A, 0);
const ButtonToPinMapping MapButtonB(PORTE_INDEX, PIN_BUTTON_B, BUTTON_B, 1);
const ButtonToPinMapping MapButtonX(PORTB_INDEX, PIN_BUTTON_X, BUTTON_X, 2);
const ButtonToPinMapping MapButtonY(PORTB_INDEX, PIN_BUTTON_Y, BUTTON_Y, 3);

const ButtonToPinMapping MapJoystickLeftX(PORTF_INDEX, PIN_JOY_LEFT_X, JOY_LEFT, 0);
const ButtonToPinMapping MapJoystickLeftY(PORTF_INDEX, PIN_JOY_LEFT_Y, JOY_LEFT, 1);

const ButtonToPinMapping MapJoystickRightX(PORTF_INDEX, PIN_JOY_RIGHT_X, JOY_RIGHT, 0);
const ButtonToPinMapping MapJoystickRightY(PORTF_INDEX, PIN_JOY_RIGHT_Y, JOY_RIGHT, 1);

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
inline void getInputStates(uint8_t portStates[]) __attribute__((always_inline));

void getInputStates(uint8_t portStates[]) {
  portStates[PORTB_INDEX] = PINB;
  portStates[PORTC_INDEX] = PINC;
  portStates[PORTD_INDEX] = PIND;
  portStates[PORTE_INDEX] = PINE;
  portStates[PORTF_INDEX] = PINF;
}

#endif
