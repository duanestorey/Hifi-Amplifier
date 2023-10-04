# Hifi-Amplifier
A Custom 5.1 LM3886 Based Custom Amplifier.  The LM3886 has a somewhat mythical status ever since a $3,000 gaincard was released in 1999.  It had amazing sound quality, but inside was only a tweaked LM3886 Texas Instrument 68W amplifier IC.  Since then, an entire community has evolved around these 'gainclones', which truly are hi-fidelity.  This project uses 5 of these, along with a Dolby Decoder IC, to create a complete DIY hi-fidelity 5.1 system.

A breakdown of the directory structure is as follows:

* PCBs - Each of the required PCBs in detail
* Code/ESP32 - The main amplifier code, written to run on an ESP32 WROOM 32 processor  - currently a work in progress and incomplete as the code has recently been changed from a STM32

# Design Philosophy

## PCBs

After iterations over several months to determine PCB usefulness and sizes, most PCBs now utilize a 55mmx55mm size.  This is under the minimum of 100mmx100mm low-cost options 
at several of the large-scale PCB manufacturing sites, and allows useful stacking of PCBs within the chassis.
