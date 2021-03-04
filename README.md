# Elite-C XInput Controller

WIP DIY XInput controller using an Elite-C microcontroller board.

## Description

A full XInput controller contains the following inputs:

* 10 + 1 digital buttons
* 4-way directional input
* 2 analog joysticks (16-bit)
* 2 analog/digital triggers (8-bit)

This requires a total of 21 pins on the MCU board (15 digital inputs, 6 analog inputs), however the common Pro Micro variants only contain 18 usable pins, and up to 20 with some modifications. The Elite-C offers a USB-C interface and provides 24 usable pins. That is enough to cover each input without using shift registers or GPIO expanders while also leaving 3 available pins for other purposes.

The only compromise to be made is the ADC in the ATmega32U4 is only 10-bit, while the XInput joysticks are 16-bit, which results in greatly reduced resolution. An external [4-channel 16-bit ADC module](https://www.cqrobot.com/index.php?route=product/product&product_id=1124) can be used to resolve this. Using an external ADC via the I2C bus will net 2 additional I/O pins for a total of 5 available pins for things like player LEDs or rumble motors. This sketch does not implement any additional functionality beyond the basic inputs.

## Optimizations

Several steps have been taken to reduce the latency as much as possible:

* **Fast state checking** - A lightweight process handles checking for state changes, avoiding much of the overhead from the XInput library state checks
* **Conditional code inclusion** - Debug and options-based code are excluded from the final payload, the less code the better 😎
* **Inline functions** - Used to avoid a few jumps and save some processing cycles
* **Avoid implicit conversions** - Any values that could normally be checked as "truthy" or implicitly converted are instead checked against explicit values, again saving precious computing cycles
* **Built with `-Ofast` optimization flag** - The program is much smaller and XInput operations run much faster with this or the `-O3` build flag set
* **1000Hz/1ms polling rate** - Allows the controller to respond a few milliseconds faster than the stock 250Hz/4ms polling rate

## Performance

Using the methodology outlined at [WydD's outstanding inputlag.science website](https://inputlag.science/controller/methodology), I've measured the performance of this sketch at the stock 4ms/250Hz polling rate and with a 1ms/1000Hz polling rate. The detailed results can be found in the [Elite-C Controller Input Lag spreadsheet](https://docs.google.com/spreadsheets/d/1yfBr1uXyzMTwqz8_qHCONQ1BwaxIcIZJ1_DqN0GXziE). Here are the important bits:

| Metric    | Native | Overclock |
| --------- | ------ | --------- |
| polling   | 4ms    | 1ms       |
| min       | 0.55ms | 0.55ms    |
| max       | 4.77ms | 1.74ms    |
| avg       | 2.47ms | 0.98ms    |
| stdev     | 1.18ms | 0.29ms    |
| % on time | 86.12% | 95.15%    |
| %1f skip  | 13.88% | 4.85%     |
| %2f skip  | 0%     | 0%        |

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
| B             | PE6      | No     |   | Right Bumper  | PD5      | No     |
| X             | PB4      | No     |   | Left Trigger  | PF1      | Yes    |
| Y             | PB5      | No     |   | Right Trigger | PF0      | Yes    |
| Back          | PD3      | No     |   | L3            | PD4      | No     |
| Start         | PD2      | No     |   | R3            | PC6      | No     |
| Home/Logo     | PC7      | No     |

## Development

### Dependencies

This sketch depends on the following libraries:

* [Arduino XInput Library](https://github.com/dmadison/ArduinoXInput)
* [SSD1306 Display Driver](https://github.com/lexus2k/ssd1306) (only for debug builds)

You will also need the following boards packages for the Arduino IDE:

* [XInput USB Core for Arduino AVR](https://github.com/dmadison/ArduinoXInput_AVR)
* [Elite-C Board w/XInput](https://github.com/jskuby/ArduinoXInput_EliteC)

### Building and Flashing

* Load the sketch into Arduino IDE and make sure you have the dependencies installed.
* Momentarily pull the reset pin on the Elite-C to ground to enter DFU bootloader.

The sketch can be built and uploaded directly from the Arduino IDE, however this will require a hard reset of the device in order to start the XInput program. Another option is to use the `Sketch > Export compiled Binary` option from the menu to output the compiled `.hex` file to the sketch folder, then upload the program `.hex` to the Elite-C flash via the Atmel FLIP application.

## TODOs

* Test analog reads for sticks and triggers
* Evaluate pin remapping
