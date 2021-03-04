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

#define ADC_MAX 1023 // 10 bit

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
#define PIN_BUTTON_BACK  PD3
#define PIN_BUTTON_START PD2
#define PIN_BUTTON_LOGO  PC7
#define PIN_BUTTON_L3    PD4
#define PIN_BUTTON_R3    PC6

#define PIN_ANALOG_LX A3
#define PIN_ANALOG_LY A2
#define PIN_ANALOG_RX A1
#define PIN_ANALOG_RY A0
#define PIN_ANALOG_LT A4
#define PIN_ANALOG_RT A5

// Input masks for Elite-C
// PD0 - 3/SCL, PD1 - 2/SDA are used for I2C communication, exclude for now
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
  constexpr ButtonToPinMapping(uint8_t i, uint8_t p, XInputControl b, uint8_t s)
    : portIndex(i), portPin(p), button(b), stateIndex(s) { }
  uint8_t portIndex;
  uint8_t portPin;
  XInputControl button;
  uint8_t stateIndex;
};

static const ButtonToPinMapping MapDpadUp(PORTB_INDEX, PIN_DPAD_UP, DPAD_UP, 0);
static const ButtonToPinMapping MapDpadDown(PORTB_INDEX, PIN_DPAD_DOWN, DPAD_DOWN, 1);
static const ButtonToPinMapping MapDpadLeft(PORTB_INDEX, PIN_DPAD_LEFT, DPAD_LEFT, 2);
static const ButtonToPinMapping MapDpadRight(PORTB_INDEX, PIN_DPAD_RIGHT, DPAD_RIGHT, 3);

static const ButtonToPinMapping MapButtonA(PORTD_INDEX, PIN_BUTTON_A, BUTTON_A, 0);
static const ButtonToPinMapping MapButtonB(PORTE_INDEX, PIN_BUTTON_B, BUTTON_B, 1);
static const ButtonToPinMapping MapButtonX(PORTB_INDEX, PIN_BUTTON_X, BUTTON_X, 2);
static const ButtonToPinMapping MapButtonY(PORTB_INDEX, PIN_BUTTON_Y, BUTTON_Y, 3);
static const ButtonToPinMapping MapButtonStart(PORTD_INDEX, PIN_BUTTON_START, BUTTON_START, 4);
static const ButtonToPinMapping MapButtonBack(PORTD_INDEX, PIN_BUTTON_BACK, BUTTON_BACK, 5);
static const ButtonToPinMapping MapButtonL3(PORTD_INDEX, PIN_BUTTON_L3, BUTTON_L3, 6);
static const ButtonToPinMapping MapButtonR3(PORTC_INDEX, PIN_BUTTON_R3, BUTTON_R3, 7);
static const ButtonToPinMapping MapButtonLB(PORTB_INDEX, PIN_BUTTON_LB, BUTTON_LB, 8);
static const ButtonToPinMapping MapButtonRB(PORTD_INDEX, PIN_BUTTON_RB, BUTTON_RB, 9);
static const ButtonToPinMapping MapButtonLogo(PORTC_INDEX, PIN_BUTTON_LOGO, BUTTON_LOGO, 10);
static const ButtonToPinMapping MapButtonLT(PORTF_INDEX, PIN_BUTTON_LT, TRIGGER_LEFT, 11);
static const ButtonToPinMapping MapButtonRT(PORTF_INDEX, PIN_BUTTON_RT, TRIGGER_RIGHT, 12);

static const ButtonToPinMapping MapJoystickLeftX(PORTF_INDEX, PIN_JOY_LEFT_X, JOY_LEFT, 0);
static const ButtonToPinMapping MapJoystickLeftY(PORTF_INDEX, PIN_JOY_LEFT_Y, JOY_LEFT, 1);
static const ButtonToPinMapping MapJoystickRightX(PORTF_INDEX, PIN_JOY_RIGHT_X, JOY_RIGHT, 0);
static const ButtonToPinMapping MapJoystickRightY(PORTF_INDEX, PIN_JOY_RIGHT_Y, JOY_RIGHT, 1);

static const ButtonToPinMapping MapAnalogLT(PORTF_INDEX, PIN_BUTTON_LT, TRIGGER_LEFT, 0);
static const ButtonToPinMapping MapAnalogRT(PORTF_INDEX, PIN_BUTTON_RT, TRIGGER_RIGHT, 1);

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
#if USE_ANALOG_TRIGGERS && USE_JOYSTICKS
  PORTF = PORTF | (PORTF_INPUT_MASK & ~(0 | (1 << PIN_JOY_LEFT_X) | (1 << PIN_JOY_LEFT_Y) | (1 << PIN_JOY_RIGHT_X) | (1 << PIN_JOY_RIGHT_Y) | (1 << PIN_BUTTON_LT) | (1 << PIN_BUTTON_RT)));
#elif USE_ANALOG_TRIGGERS
  PORTF = PORTF | (PORTF_INPUT_MASK & ~(0 | (1 << PIN_BUTTON_LT) | (1 << PIN_BUTTON_RT)));
#elif USE_JOYSTICKS
  PORTF = PORTF | (PORTF_INPUT_MASK & ~(0 | (1 << PIN_JOY_LEFT_X) | (1 << PIN_JOY_LEFT_Y) | (1 << PIN_JOY_RIGHT_X) | (1 << PIN_JOY_RIGHT_Y)));
#else
  PORTF = PORTF | PORTF_INPUT_MASK;
#endif

  XInput.setJoystickRange(0, ADC_MAX);
  XInput.setTriggerRange(0, ADC_MAX);
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
