# Sprites Directory

Store all sprite and image assets for your arcade machine here.

## Recommended Image Formats

### BMP Format (Recommended)
- **Best choice** for ESP32 due to simple format
- Use 24-bit or 16-bit color depth
- No compression (faster loading)
- Example: `player.bmp`, `enemy.bmp`

### Raw RGB565 Format
- Most efficient for TFT displays
- Direct pixel data without headers
- 2 bytes per pixel
- Example: `background.rgb`

## Sprite Naming Convention

Use clear, descriptive names:
```
player_idle.bmp       # Player standing still
player_walk1.bmp      # Player walking frame 1
player_walk2.bmp      # Player walking frame 2
enemy_01.bmp          # Enemy type 1
enemy_02.bmp          # Enemy type 2
item_coin.bmp         # Coin collectible
item_powerup.bmp      # Power-up item
background_main.bmp   # Main background
ui_score.bmp          # UI element for score
```

## Sprite Specifications

### Size Recommendations
- Keep sprites small (8x8, 16x16, 32x32 pixels)
- Larger sprites use more memory and load slower
- Consider using sprite sheets for animations

### Color Depth
- **16-bit (RGB565)**: Good balance of quality and size
- **24-bit (RGB888)**: Better quality, larger files
- **8-bit indexed**: Smallest size, limited colors

## Creating Sprites

### Using GIMP (Free)
1. Create/edit your sprite
2. Resize to desired dimensions (Image → Scale Image)
3. Export as BMP (File → Export As)
4. Choose 24-bit or 16-bit color depth

### Using Online Tools
- Piskel (piskelapp.com) - Great for pixel art
- Photopea (photopea.com) - Free Photoshop alternative

## Loading Sprites in Code

Example code to load a sprite from SD card:
```cpp
#include <SD.h>

// Load sprite from SD card
File spriteFile = SD.open("/sprites/player.bmp");
if (spriteFile) {
  // Read BMP header and pixel data
  // Draw to display
  spriteFile.close();
}
```

## Sprite Sheet Format

For animated sprites, consider using sprite sheets:
```
animation_sheet.bmp
[frame1][frame2][frame3][frame4]
  16x16  16x16  16x16  16x16
```

## Current Sprites
(Document your sprites here)

- No sprites installed yet - add your first sprite!
