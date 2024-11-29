<img src="screenshot.png" width="50%">

# Channel Selector

This PCB allows switching of 4 analog stereo inputs and one 5.1 channel input into 5.1.  Channels that are missing
can be created.  It is based on the AX2358 IC.

## Version History

- 1.0: Initial Release
- 1.2: Did an initial build.  Seems like there is digital noise on the output, likely from the I2C signals sharing the audio ground.  This design now has a digital and analog ground on the board - it'll use the digital ground for the AX2358 IC, but use the analog ground for the channel outputs.

## Speciality Components

* AX2358 Channel switching IC
* Nichicon Muse ES 25V 10uF
