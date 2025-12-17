# Examples

This directory contains example code to help you get started with your ESP32 arcade machine.

## Available Examples

### 1. sd_card_test.cpp
A simple test program to verify your SD card module is working correctly.

**What it does:**
- Initializes the SD card
- Displays card information (size, used space)
- Creates a test file (`test.txt`)
- Writes "Hello from ESP32!" to the file
- Reads the file back and displays contents

**How to use:**
1. Copy this file to `src/main.cpp` (backup your existing main.cpp first)
2. Upload to your ESP32
3. Open Serial Monitor (115200 baud)
4. You should see the SD card information and test results

**Expected output:**
```
=== SD Card Test ===

Initializing SD card...
SUCCESS! SD card detected.
Card Size: 8192 MB
Total Space: 8192 MB
Used Space: 1 MB

Testing file write...
Write SUCCESS!

Testing file read...
Read SUCCESS! Contents:
Hello from ESP32!

=== Test Complete ===
```

**Troubleshooting:**
- If you see "FAILED! SD card not detected.", check your wiring
- Verify CS pin is GPIO 5 (or change SD_CS_PIN in the code)
- Make sure SD card is formatted as FAT32

## Adding Your Own Examples

Create new example files following this naming convention:
- `button_test.cpp` - Test button inputs
- `display_test.cpp` - Test TFT display
- `sprite_loader.cpp` - Load and display sprites
- `sound_player.cpp` - Play WAV files

Document each example in this README for others to use!
