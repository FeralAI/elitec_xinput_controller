# Elite-C XInput Controller

DIY XInput controller using an Elite-C microcontroller board.

## Description

A full XInput controller contains the following inputs:

* 10 + 1 digital buttons
* 4-way directional input
* 2 analog joysticks (16-bit)
* 2 analog/digital triggers (8-bit)

This requires a total of 21 pins on the MCU board (15 digital inputs, 6 analog inputs), however the common Pro Micro variants only contain 18 usable pins, and up to 20 with some modifications. The Elite-C offers a USB-C interface and provides 24 usable pins. That is enough to cover each input without using shift registers or GPIO expanders while also leaving 3 available pins for other purposes.

The only compromise to be made is the ADC in the ATmega32U4 is only 10-bit, while the XInput joysticks are 16-bit, which results in greatly reduced resolution. An external [4-channel 16-bit ADC module](https://www.cqrobot.com/index.php?route=product/product&product_id=1124) can be used to resolve this. Using an external ADC via the I2C bus will net 2 additional I/O pins for a total of 5 available pins for things like player LEDs or rumble motors. This sketch does not implement any additional functionality beyond the basic inputs.

## Pin Mapping

The Elite-C is labelled with the port pins instead of typical Arduino pin numbering. Here is the mapping:

![Elite-C Pin Mapping](resources/Elite-C-XInput-mapping.png)

| XInput Button | Port Pin | Analog |   | XInput Button | Port Pin | Analog |
| ------------- | -------- | ------ | - | ------------- | -------- | ------ |
| Dpad Up       | PB1      | No     |   | Left Stick X  | PF4      | Yes    |
| Dpad Down     | PB3      | No     |   | Left Stick Y  | PF5      | Yes    |
| Dpad Left     | PB2      | No     |   | Right Stick X | PF6      | Yes    |
| Dpad Right    | PB6      | No     |   | Right Stick Y | PF7      | Yes    |
| A             | PD7      | No     |   | Left Bumper   | PB7      | No     |
| B             | PE6      | No     |   | Right Bumper  | PB0      | No     |
| X             | PB4      | No     |   | Left Trigger  | PF1      | Yes    |
| Y             | PB5      | No     |   | Right Trigger | PF0      | Yes    |
| Back          | PD3      | No     |   | L3            | PD4      | No     |
| Start         | PD2      | No     |   | R3            | PC6      | No     |
| Home/Logo     | PC7      | No     |

> NOTE: PD5 was originally used for Right Bumper, however both Elite-C boards I have seem to have issues using that pin (stays high after activate, floats a bit, activates randomly). This may be a board configuration issue I haven't worked through yet since that is the TX pin on a standard Pro Micro. PB0 is used instead until that issue is resolved.

## Building and Flashing

The Elite-C comes with a DFU bootloader instead of the typical Caterina bootloader of the Pro Micro. As such, direct programming of the Elite-C from Arduino IDE isn't currently supported. My current workflow is to use Arduino IDE to output a compiled `.hex` file, pull the Reset pin on the Elite-C to ground to enter DFU bootloader, then program the Elite-C via the Atmel FLIP application.

## Customizations

In an effort to make this as fast as possible, I'm using a modified version of the XInput library where the USB descriptor is updated to use a 1000Hz (1ms) polling rate instead of the stock 250Hz (4ms). This has brought the average response time to about 1.3ms with the current sketch logic.

## TODOs

* Implement analog reads for sticks and triggers
* Optimize logic
  * Evaluate pin remapping for better read/parse speed
  * Evaluate loop logic
  * Dig into XInput library for optimizations
* Boards file
  * Create XInput boards repo for Elite-C
  * Evaluate options (was copied from Sparkfun Pro Micro board config)
  * Look at STM32 boards for Arduino IDE DFU example
* Latency testing
