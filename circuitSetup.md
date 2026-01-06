# ESP32-32E Arcade Machine Setup

## Board 

Board: 	Hosyond 4.0'' 320x480 Touch Screen with ESP32-32E

Power: 5V INUI Power Bank, from a breadboar's power rails into the Hosyond board's USB-C

TFT LCD Diplay: Built into the Hosyond board using ST7796S driver

## Audio

DFPlayer Mini: Conntected to Hosyond board's UART Serial Port
- 5V to VCC
- GND to GND
- Board TX to DFPlayer RX
- Board RX to DFPlayer TX

DFPlayer Mini has an SD card with all of the project's WAV audio files.
These files are named by the description of their sound. For example, "<b>pacman_eat.wav</b>".