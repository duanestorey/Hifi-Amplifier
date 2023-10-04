<img src="screenshot.png" width="50%">

# Dolby Digital Decoder

This PCB decoders a SPDIF Dolby Digital 5.1 or PCM stream.  It is based on the STA310 set-top box Dolby Digital Decoding IC.

## Speciality Components

* STA310 Dolby Digital Decoding IC
* 27 MHz Clock
* 2.5V 3-pin regulators

## Power Connections

### Input 

1. 3-pin GND, 3V3, 5V digital
2. 3-pin AGND, 3V3A, 5VA analog

### Output

1. 2-pin GND, 5V digital

## Version History

- 1.0: 
    * Initial Release
- 1.1: 
    * Routed I2S_D0 from Dolby Digital (which is two channel Dolby) to the DAC I2S_D4 pin
    * Re-annotated geographically 


