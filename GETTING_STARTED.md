# Getting Started Checklist

Use this checklist to set up your ESP32 arcade machine step by step.

## Hardware Preparation

### Components Checklist
- [ ] ESP32-32E development board
- [ ] MicroSD card module (SPI interface)
- [ ] MicroSD card (4-32GB, Class 10 recommended)
- [ ] USB cable (data capable, not charge-only)
- [ ] Jumper wires (6x minimum for SD card)
- [ ] Computer with USB port
- [ ] Optional: TFT display, buttons, speaker

### Shopping List
If you don't have these components yet, here's what to buy:
- **ESP32-32E**: $5-15 (Amazon, AliExpress)
- **SD Card Module**: $2-5 (SPI interface)
- **MicroSD Card**: $5-15 (8-16GB Class 10)
- **Jumper Wires**: $3-5 (40 pack)
- **Total Cost**: ~$15-40 for basic setup

## Software Setup

### Computer Setup
- [ ] Install PlatformIO OR Arduino IDE
  - PlatformIO: `pip install platformio`
  - Arduino IDE: Download from arduino.cc
- [ ] Install ESP32 board support (if using Arduino IDE)
- [ ] Install USB drivers (CP2102 or CH340 if needed)
- [ ] Clone or download this repository

### Test Your Setup
- [ ] Connect ESP32 to computer via USB
- [ ] Open PlatformIO/Arduino IDE
- [ ] Select correct board (ESP32 Dev Module)
- [ ] Select correct COM port
- [ ] Try uploading a simple blink sketch

## Hardware Assembly

### SD Card Module Wiring
Follow HARDWARE_SETUP.md for detailed instructions.

- [ ] Connect SD module VCC → ESP32 3.3V
- [ ] Connect SD module GND → ESP32 GND
- [ ] Connect SD module MISO → ESP32 GPIO 19
- [ ] Connect SD module MOSI → ESP32 GPIO 23
- [ ] Connect SD module SCK → ESP32 GPIO 18
- [ ] Connect SD module CS → ESP32 GPIO 5
- [ ] Double-check all connections
- [ ] Verify no short circuits

### Optional Components
- [ ] Connect display (see HARDWARE_SETUP.md)
- [ ] Connect buttons (see arcade.cfg for default pins)
- [ ] Connect speaker to GPIO 25/26 (DAC pins)

## MicroSD Card Preparation

Follow DEPLOYMENT.md for detailed instructions.

### Format SD Card
- [ ] Insert SD card into computer
- [ ] Format as FAT32
  - Windows: Right-click drive → Format → FAT32
  - Mac: Disk Utility → Erase → MS-DOS (FAT)
  - Linux: `sudo mkfs.vfat -F 32 /dev/sdX1`
- [ ] Verify format completed successfully

### Copy Files to SD Card
- [ ] Navigate to `sd_card_contents/` folder
- [ ] Select all folders (config, games, sprites, sounds)
- [ ] Copy to SD card root directory
- [ ] Verify all folders copied correctly
- [ ] Safely eject SD card from computer

### Verify SD Card Contents
Your SD card root should have:
- [ ] `/config/` folder with `arcade.cfg` file
- [ ] `/games/` folder with README.md
- [ ] `/sprites/` folder with README.md
- [ ] `/sounds/` folder with README.md

## First Upload & Test

### Upload Test Code
- [ ] Insert SD card into SD card module
- [ ] Power off ESP32
- [ ] Connect all hardware (SD module to ESP32)
- [ ] Power on ESP32
- [ ] Open `examples/sd_card_test.cpp`
- [ ] Copy to `src/main.cpp` (backup original)
- [ ] Upload to ESP32
- [ ] Open Serial Monitor (115200 baud)
- [ ] Verify "SUCCESS! SD card detected."

### Troubleshooting Test
If SD card test fails:
- [ ] Check all wiring connections
- [ ] Verify SD card is FAT32 formatted
- [ ] Try different SD card
- [ ] Check CS pin is GPIO 5
- [ ] Verify 3.3V power to SD module

### Upload Main Code
- [ ] Restore original `src/main.cpp`
- [ ] Upload to ESP32
- [ ] Open Serial Monitor (115200 baud)
- [ ] Verify system boots successfully
- [ ] Check directories are created
- [ ] Verify config file is read

## Configuration

### Edit Configuration File
- [ ] Remove SD card from ESP32 (power off first)
- [ ] Insert SD card into computer
- [ ] Open `config/arcade.cfg` in text editor
- [ ] Update display settings if needed
- [ ] Update button pins if using different GPIO
- [ ] Save changes
- [ ] Safely eject SD card
- [ ] Re-insert into ESP32 and test

## Adding Content

### Add Your First Sprite
- [ ] Create or find a sprite image (8x8 to 32x32 pixels)
- [ ] Convert to BMP format (24-bit)
- [ ] Copy to SD card `/sprites/` folder
- [ ] Name it descriptively (e.g., `player.bmp`)
- [ ] Test loading in your code

### Add Your First Sound
- [ ] Create or find a sound effect
- [ ] Convert to WAV format (8-bit, 22050 Hz, mono)
- [ ] Keep it short (under 2 seconds)
- [ ] Copy to SD card `/sounds/` folder
- [ ] Name it descriptively (e.g., `jump.wav`)
- [ ] Test playing in your code

### Create Game Data
- [ ] Design your game data format (CSV, JSON, or text)
- [ ] Create data files
- [ ] Copy to SD card `/games/` folder
- [ ] Update code to load game data
- [ ] Test in your game

## Development

### Start Coding Your Game
- [ ] Open `src/main.cpp`
- [ ] Review the SD card initialization code
- [ ] Add your game logic to `loop()`
- [ ] Test frequently with uploads
- [ ] Add comments to your code
- [ ] Save backups regularly

### Recommended Next Steps
- [ ] Add display library (TFT_eSPI, Adafruit GFX)
- [ ] Implement button reading
- [ ] Create sprite loading function
- [ ] Add sound playback
- [ ] Build your first game (Snake? Pong?)
- [ ] Share your creation!

## Common Issues & Solutions

### "SD card not detected"
- ✓ Check wiring (especially CS, MOSI, MISO, SCK)
- ✓ Verify 3.3V power
- ✓ Format as FAT32
- ✓ Try different SD card

### "Cannot upload to ESP32"
- ✓ Check USB cable (must be data cable)
- ✓ Install drivers (CP2102 or CH340)
- ✓ Hold BOOT button while uploading
- ✓ Select correct COM port

### "Files not loading"
- ✓ Check file paths (case-sensitive)
- ✓ Verify files in correct directories
- ✓ Check file formats match specifications
- ✓ Look at Serial Monitor for error messages

### "Display not working"
- ✓ Add display library to platformio.ini
- ✓ Check display wiring
- ✓ Verify display power requirements
- ✓ Test with display example code first

## Help & Resources

### Documentation
- [ ] README.md - Project overview and quick start
- [ ] DEPLOYMENT.md - SD card preparation guide
- [ ] HARDWARE_SETUP.md - Wiring and hardware guide
- [ ] sd_card_contents/README.md - File format details
- [ ] examples/README.md - Example code explanations

### Online Resources
- ESP32 Documentation: docs.espressif.com
- PlatformIO Docs: docs.platformio.org
- Arduino SD Library: arduino.cc/en/reference/SD
- GitHub Issues: Report problems or ask questions

### Community
- Share your arcade machine project!
- Post issues on GitHub
- Help others with their builds
- Contribute examples and improvements

## Project Completion Checklist

You've successfully set up your arcade machine when:
- [x] Hardware is wired correctly
- [x] SD card is prepared with correct structure
- [x] Code uploads without errors
- [x] Serial Monitor shows successful SD card initialization
- [x] All directories created on SD card
- [x] Config file loads correctly
- [ ] First game or demo running
- [ ] Ready to create more content!

---

**Congratulations!** 🎉 You now have a working ESP32 arcade machine foundation. Start adding games, sprites, and sounds to bring your arcade to life!

**Next:** Build your first game or explore the examples folder for inspiration.
