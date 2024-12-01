## Version 2.0

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

- Added connector for IR diode