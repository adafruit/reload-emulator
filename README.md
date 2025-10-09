# Reload - Portable Cycle-Stepped Emulator for Retro Computers

NOTES:
 - This fork is for the Adafruit Fruit Jam and only builds the Apple IIe emulator
 - Can use built in images OR "Total Replay v5.2.hdv" from SD card (must use exact name!)
 - Remove SD card and reset to use built in floppy images instead
 - No gamepad support
 - No sound support

Emulated systems:

### Apple //e
  - 128 KB RAM installed 
  - Extended 80 column card in the AUX slot
  - Disk II controller and 1 drive in slot 6
  - ProDOS hard disk controller in slot 7

## Quickstart

Grab a UF2 file from the releases page and the Total Replay image from archive.org.

Put `Total Replay v5.2.hdv` (use exactly that filename) in the top directory of an SD card.

Copy the Reload Emulator UF2 file to your Fruit Jam's RP2350 drive.

Plug in a keyboard & gamepad. Insert the SD card. Connect to a compatible display.

Turn it on & play!

## Requirements & Building from source

Refer to the github actions files for the steps to build reload-emulator.
