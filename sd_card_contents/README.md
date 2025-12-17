# SD Card Contents for ESP32 Arcade Machine

This directory contains the file structure that should be copied to your microSD card.

## Directory Structure

```
sd_card_contents/
├── config/          # Configuration files
│   └── arcade.cfg   # Main arcade machine configuration
├── games/           # Game ROM files or game data
│   └── README.md    # Game installation instructions
├── sprites/         # Game sprite images (BMP, PNG format)
│   └── README.md    # Sprite format information
└── sounds/          # Game sound effects (WAV format recommended)
    └── README.md    # Sound format information
```

## How to Prepare Your SD Card

### 1. Format the SD Card
- Use FAT32 format (most compatible with ESP32)
- For cards larger than 32GB, use a tool like SD Card Formatter

### 2. Copy Files to SD Card
```bash
# Copy the entire contents of this directory to your SD card root
cp -r sd_card_contents/* /path/to/your/sdcard/
```

### 3. Required Directory Structure
The ESP32 will automatically create these directories if they don't exist:
- `/games/` - Store your game files here
- `/sprites/` - Store sprite/image assets here
- `/sounds/` - Store audio files here
- `/config/` - Configuration files (arcade.cfg is required)

### 4. Configuration File
The `arcade.cfg` file in the `/config/` directory contains:
- Display settings (resolution, rotation)
- Control pin mappings (buttons)
- Game settings (difficulty, sound)
- System settings (FPS, debug mode)

Edit this file with a text editor to match your hardware setup.

## File Format Recommendations

### Images (Sprites)
- **Format**: BMP (24-bit or 16-bit) or raw RGB565
- **Naming**: Use descriptive names like `player.bmp`, `enemy.bmp`
- Keep file sizes small for faster loading

### Sounds
- **Format**: WAV (8-bit or 16-bit PCM)
- **Sample Rate**: 22050 Hz or 44100 Hz
- **Naming**: Use descriptive names like `jump.wav`, `coin.wav`

### Game Data
- Store game levels, maps, or save data in the `/games/` directory
- Use simple text formats (CSV, JSON) for easy parsing

## Testing Your SD Card

1. Insert the SD card into your ESP32 arcade machine
2. Power on the device
3. Open Serial Monitor (115200 baud)
4. You should see:
   ```
   Initializing SD card...
   SD card initialized successfully!
   Created directory: /games
   Created directory: /sprites
   Created directory: /sounds
   Created directory: /config
   ```

## Troubleshooting

### SD Card Not Detected
- Check wiring connections (CS, MOSI, MISO, SCK)
- Try a different SD card (some cards are not compatible)
- Ensure SD card is formatted as FAT32
- Check if card is write-protected

### Files Not Loading
- Verify file names are correct (case-sensitive on some systems)
- Check file format compatibility
- Ensure files are in correct directories
- Check file size limits (keep individual files under 1MB for best performance)

### Slow Performance
- Use Class 10 or UHS-I SD cards for better speed
- Keep files small and optimized
- Avoid fragmented SD cards (reformat if needed)
