opendroplet-avr
===============

## What is this?
Open Droplet prototyping

## Getting Started (work in progress, needs proper documentation)
1. Clone/Download the repository
2. Install the IDE and libraries, see http://openenergymonitor.org/emon/buildingblocks/setting-up-the-arduino-environment
3. Connect opendroplet-avr board to your computer.
 1. with any Arduino compatible bootloader via USB-serial adapter, FTDI232, PL2303HX, etc.
 2. with DualOptiboot bootloader via OTA (over the air) update - needs base node connected to IDE and DualOptiboot bootloader
 3. for a bare virgin micro controller or if the board is bricked, use an ICSP connection and
    1. flash the compiled .hex file with avrdude or
    2. flash the DualOptiboot (TODO: insert link here) bootloader to your microcontroller. You may then use a serial connection to upload the actual firmware, see 4.i-ii.
   
## Usage

The EEPROM contains a UID, a unique identification which will be sent out with every packet, to distinguish between adjascent sensors that are in range, for debugging purposes and for a unique assignment in the cloud. You can disable the UID in the flash code or if you fill the EEPROM address with zeros.

You can alter the UID without using an ICSP adapter with the help of the /opendroplet-configurator Arduino sketch. Just remember to flash the normal firmware again after you changed the config.

## License
See LICENSE file.
