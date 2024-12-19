## Version 2.0

### Project Style Guide

#### LEDs

All LEDs should be set to have a default current of 3ma for consistent brightness. Here are the guidelines for colours and for what current limiting resistor is required.

    3V digital - 

### Power Monitors

There are several INA260 power meters.  

1) MAIN Digital/Analog PSU monitor, address GND/SCL
2) MAIN Positive rail for channel amplifiers, address SCL/GND
3) 7V to 5V converter, address GND/GND
4) 7V to 3V converter, address GND/3V
5) Positive analog converter,  3V/GND
6) Entire 7V converter, 3V/3V

### Temperature Sensors

1) Microprocessor board, address GND/GND
2) Digital/Analog PSU, address GND/3V

### Design Goals
- Switch amplifier to LM4766, 2 boards, 4 channels
    - should have a microcontrolled mute
- Design miniDSP using adau1701
- Simply power supplies - take all three from the toroidal
- Modify microprocessor to have more ADC for current monitoring, and more GPIOs using add-on
- Possibly combine subwoofer board with miniDSP post-DAC filtering
- Small signal relay for swithing between filtered from channels or not
- New SPDIF board with more inputs
- Separate DAC Board or make it a HAT

#### Microprocessor

- TODO: Add connector for IR diode [IR Receiver](https://www.conrad.com/en/p/tru-components-os-0038-n-ir-receiver-non-standard-axial-lead-38-khz-5-8-mm-940-nm-35-1567234.html?srsltid=AfmBOooULA3z41DL8Pl4VzeSBMj5sArhngCSZTbEgrS--_IQduufNs6r#productDownloads)
- TODO: Add GPIO board as a HAT

#### SPDIF / DAC

- TODO: Split into separate boards, via a HAT probably
- TODO: Possibly change SPDIF connectors to screw terminals

#### SPDIF

- TODO: ADded more inputs
- TODO: Maybe add isolation transformers
- TODO: Change ground plane so SPDIF ground doesn't interfere with analog DAC ground