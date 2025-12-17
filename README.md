# ESP32 Arcade Machine

A simple arcade machine project for ESP32-32E with microSD card support. Load games, sprites, and sounds from a microSD card to create your own retro arcade experience!

## Features

- 🎮 MicroSD card support for storing games and assets
- 🎨 Sprite loading system for graphics
- 🔊 Audio playback support for sound effects
- ⚙️ Configuration file system
- 📁 Organized file structure for easy content management
- 🎯 Ready-to-use template for arcade game development

## Hardware Requirements

### Essential Components
- **ESP32-32E** development board
- **MicroSD card module** (SPI interface)
- **MicroSD card** (FAT32 formatted, Class 10 recommended)
- **TFT Display** (optional, but recommended - e.g., ILI9341, ST7735)
- **Buttons/Controls** for game input
- **Power supply** (5V via USB or battery)

### Optional Components
- **Speaker/Buzzer** for audio output (connect to GPIO 25/26 DAC pins)
- **I2S Audio DAC** for better sound quality (e.g., MAX98357A)
- **Battery** for portable operation
- **3D printed enclosure** for arcade cabinet look

## Wiring Diagram

### SD Card Module → ESP32-32E
```
SD Card Pin    →    ESP32 Pin
------------------------------------
CS             →    GPIO 5
MOSI           →    GPIO 23
MISO           →    GPIO 19
SCK            →    GPIO 18
VCC            →    3.3V
GND            →    GND
```

### Default Button Pins (Configurable in arcade.cfg)
```
Button         →    Default GPIO
------------------------------------
UP             →    GPIO 32
DOWN           →    GPIO 33
LEFT           →    GPIO 25
RIGHT          →    GPIO 26
A              →    GPIO 27
B              →    GPIO 14
START          →    GPIO 12
SELECT         →    GPIO 13
```

**Note:** Connect buttons between GPIO and GND with internal pull-up resistors enabled.

## Quick Start Guide

### 1. Software Setup

#### Option A: Using PlatformIO (Recommended)
```bash
# Install PlatformIO Core
pip install platformio

# Clone this repository
git clone https://github.com/ChampionX1001/esp32-arcade-machine.git
cd esp32-arcade-machine

# Build and upload to ESP32
pio run --target upload

# Open serial monitor
pio device monitor
```

#### Option B: Using Arduino IDE
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install ESP32 board support:
   - File → Preferences → Additional Board URLs
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install
3. Open `src/main.cpp` in Arduino IDE
4. Select Board: "ESP32 Dev Module"
5. Select Port: Your ESP32 COM port
6. Click Upload

### 2. Prepare Your MicroSD Card

#### Format the Card
- Format as **FAT32**
- For cards >32GB, use [SD Card Formatter](https://www.sdcard.org/downloads/formatter/)

#### Copy Files to SD Card
```bash
# Copy the SD card contents directory to your microSD card root
cp -r sd_card_contents/* /path/to/your/sdcard/
```

Or manually:
1. Open the `sd_card_contents/` folder in this repository
2. Copy all folders (`config`, `games`, `sprites`, `sounds`) to your SD card root
3. Your SD card should look like:
   ```
   SD Card Root/
   ├── config/
   │   └── arcade.cfg
   ├── games/
   ├── sprites/
   └── sounds/
   ```

### 3. Configure Your Setup

Edit `config/arcade.cfg` on the SD card to match your hardware:
- Display settings (width, height, rotation)
- Button pin assignments
- Game preferences
- Audio settings

### 4. Test the System

1. Insert the prepared microSD card into the SD card module
2. Connect the SD card module to ESP32 using the wiring diagram above
3. Power on the ESP32
4. Open Serial Monitor (115200 baud)
5. You should see:
   ```
   ========================================
   ESP32 Arcade Machine - Starting...
   ========================================
   
   Initializing SD card...
   SD card initialized successfully!
   SD Card Type: SDHC
   SD Card Size: 8192MB
   ...
   Arcade Machine Ready!
   ```

## Project Structure

```
esp32-arcade-machine/
├── src/
│   └── main.cpp              # Main program with SD card initialization
├── sd_card_contents/          # Files to copy to your microSD card
│   ├── config/
│   │   └── arcade.cfg        # Configuration file
│   ├── games/                # Game data files
│   ├── sprites/              # Image/sprite files (BMP format)
│   ├── sounds/               # Audio files (WAV format)
│   └── README.md             # SD card setup instructions
├── platformio.ini             # PlatformIO configuration
├── .gitignore
└── README.md                 # This file
```

## Adding Content to Your Arcade Machine

### Adding Games
1. Place game data files in `sd_card_contents/games/` on your SD card
2. Update your game logic in `src/main.cpp` to load and run the game
3. See `sd_card_contents/games/README.md` for details

### Adding Sprites
1. Create/convert images to BMP format (16-bit or 24-bit)
2. Keep sprites small (8x8, 16x16, 32x32 pixels)
3. Place in `sd_card_contents/sprites/` on your SD card
4. See `sd_card_contents/sprites/README.md` for details

### Adding Sounds
1. Convert audio to WAV format (8-bit, 22050 Hz, mono)
2. Keep sound effects short (under 2 seconds)
3. Place in `sd_card_contents/sounds/` on your SD card
4. See `sd_card_contents/sounds/README.md` for details

## Troubleshooting

### SD Card Not Detected
- ✓ Check all wiring connections (CS, MOSI, MISO, SCK)
- ✓ Verify SD card is formatted as FAT32
- ✓ Try a different SD card (compatibility varies)
- ✓ Check if write-protect switch is off
- ✓ Verify 3.3V power supply to SD module

### Program Won't Upload
- ✓ Check USB cable (must be data cable, not charge-only)
- ✓ Install CP2102 or CH340 drivers if needed
- ✓ Try holding BOOT button while uploading
- ✓ Check correct COM port is selected

### Display Issues
- ✓ Verify display library is installed and configured
- ✓ Check display wiring (SPI or parallel)
- ✓ Adjust display settings in `arcade.cfg`

### Files Not Loading
- ✓ Check file names are correct (case-sensitive)
- ✓ Verify files are in correct directories
- ✓ Ensure SD card isn't corrupted (try reformatting)
- ✓ Check file formats match requirements

## Development

### Building Custom Games

The provided code creates the foundation. To add games:

1. **Read the configuration** from SD card in `setup()`
2. **Load sprites/sounds** as needed
3. **Implement game logic** in `loop()`
4. **Draw to display** using your chosen display library
5. **Read button inputs** for controls
6. **Save game state** to SD card

Example:
```cpp
void loop() {
  // Read button states
  bool upPressed = !digitalRead(BUTTON_UP_PIN);
  
  // Load sprite from SD card
  File sprite = SD.open("/sprites/player.bmp");
  
  // Update game logic
  updatePlayer();
  
  // Draw to display
  drawGameFrame();
  
  // Save high score
  saveToSD("/games/highscore.dat");
}
```

### Recommended Libraries

Add these to `platformio.ini` as needed:
```ini
lib_deps = 
    adafruit/Adafruit GFX Library @ ^1.11.3
    adafruit/Adafruit ILI9341 @ ^1.5.10
    bodmer/TFT_eSPI @ ^2.5.0
```

## Examples

Coming soon:
- Simple Snake game
- Breakout/Pong clone
- Space shooter
- Tetris implementation

## Contributing

Contributions are welcome! Feel free to:
- Add example games
- Improve SD card handling
- Add display support for more screens
- Create 3D printable enclosure designs
- Write tutorials and documentation

## License

This project is open source. See LICENSE file for details.

## Resources

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Arduino SD Library](https://www.arduino.cc/en/reference/SD)
- [Adafruit GFX Graphics Library](https://learn.adafruit.com/adafruit-gfx-graphics-library)

## Credits

Created for ESP32-32E arcade machine enthusiasts. Happy gaming! 🎮