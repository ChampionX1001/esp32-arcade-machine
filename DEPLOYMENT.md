# Deploying to MicroSD Card - Quick Guide

This guide shows you exactly how to prepare your microSD card for the ESP32 arcade machine.

## What You Need

1. **MicroSD Card** (1GB to 32GB recommended)
2. **MicroSD Card Reader** (USB adapter for your computer)
3. **Computer** (Windows, Mac, or Linux)
4. **This repository** (cloned or downloaded)

## Step-by-Step Deployment

### Step 1: Format Your SD Card

#### Windows
1. Insert SD card into card reader
2. Open "This PC" or "My Computer"
3. Right-click on the SD card drive
4. Select "Format..."
5. Choose:
   - **File System:** FAT32 (or FAT)
   - **Allocation unit size:** Default
   - **Volume label:** ARCADE (or any name)
6. Click "Start"
7. Click "OK" when warned about data loss

**Note:** Windows doesn't format >32GB cards as FAT32. Use [SD Card Formatter](https://www.sdcard.org/downloads/formatter/) instead.

#### macOS
1. Insert SD card into card reader
2. Open "Disk Utility" (Applications → Utilities)
3. Select your SD card from the left sidebar
4. Click "Erase" at the top
5. Choose:
   - **Name:** ARCADE
   - **Format:** MS-DOS (FAT)
   - **Scheme:** Master Boot Record
6. Click "Erase"
7. Click "Done"

#### Linux
```bash
# Find your SD card device (usually /dev/sdb or /dev/mmcblk0)
lsblk

# Unmount if mounted (replace /dev/sdX with your device)
sudo umount /dev/sdX1

# Format as FAT32
sudo mkfs.vfat -F 32 -n ARCADE /dev/sdX1

# Eject safely
sudo eject /dev/sdX
```

### Step 2: Copy Files to SD Card

#### Method A: Command Line (Linux/Mac)

```bash
# Navigate to repository
cd /path/to/esp32-arcade-machine

# Copy all contents to SD card (replace /media/ARCADE with your mount point)
cp -r sd_card_contents/* /media/ARCADE/

# Or for Mac:
cp -r sd_card_contents/* /Volumes/ARCADE/

# Verify files were copied
ls -la /media/ARCADE/
```

#### Method B: GUI (Windows/Mac/Linux)

1. Open the `sd_card_contents` folder in this repository
2. You should see:
   ```
   sd_card_contents/
   ├── config/
   ├── games/
   ├── sprites/
   └── sounds/
   ```
3. **Select all folders** (config, games, sprites, sounds)
4. **Copy** them (Ctrl+C or Cmd+C)
5. Open your SD card drive (usually shows as "ARCADE" or "Removable Disk")
6. **Paste** the folders (Ctrl+V or Cmd+V)
7. Wait for copy to complete

### Step 3: Verify SD Card Contents

Your SD card should now look like this:

```
SD Card Root (ARCADE)/
├── config/
│   └── arcade.cfg          ← Configuration file
├── games/
│   └── README.md           ← Game instructions
├── sprites/
│   └── README.md           ← Sprite instructions
└── sounds/
    └── README.md           ← Sound instructions
```

**Check:**
- [ ] All 4 folders are at the root level (not in a subfolder)
- [ ] `config/arcade.cfg` file exists
- [ ] Each folder has a README.md file
- [ ] No extra hidden files or folders

### Step 4: Safely Eject SD Card

#### Windows
1. Click "Safely Remove Hardware" icon in system tray
2. Select your SD card
3. Wait for "Safe to Remove" message
4. Remove the card

#### macOS
1. Drag the SD card icon to Trash (it becomes an eject icon)
2. Or right-click and select "Eject"
3. Wait for icon to disappear
4. Remove the card

#### Linux
```bash
sudo eject /dev/sdX
```

### Step 5: Insert into ESP32 Hardware

1. **Power off** your ESP32 arcade machine
2. **Insert the SD card** into the SD card module
   - Card should click into place
   - Metal contacts should face down (toward the PCB)
3. **Double-check** the card is fully inserted
4. **Power on** the ESP32

### Step 6: Test the Setup

1. Connect ESP32 to your computer via USB
2. Open Serial Monitor (Arduino IDE or PlatformIO)
3. Set baud rate to **115200**
4. Press the ESP32 reset button
5. You should see:
```
========================================
ESP32 Arcade Machine - Starting...
========================================

Initializing SD card...
SD Card Type: SDHC
SD Card Size: 8192MB
Total space: 8192MB
Used space: 1MB
SD card initialized successfully!

Creating directory structure...
Directory already exists: /games
Directory already exists: /sprites
Directory already exists: /sounds
Directory already exists: /config

--- Files on SD Card ---
Listing directory: /
  DIR : config
  DIR : games
  DIR : sprites
  DIR : sounds
--- End of File List ---

--- Configuration ---
# Arcade Machine Configuration File
display_width=320
display_height=240
...
--- End Configuration ---

========================================
Arcade Machine Ready!
========================================
```

## Customizing Your Configuration

After successful deployment, you can customize the setup:

### Edit Configuration on SD Card

1. Remove SD card from ESP32 (power off first!)
2. Insert into your computer
3. Open `config/arcade.cfg` in any text editor
4. Modify settings:
   ```
   # Change display size
   display_width=240
   display_height=320
   
   # Adjust button pins
   button_up=34
   button_down=35
   
   # Enable/disable sound
   sound_enabled=true
   volume=70
   ```
5. Save the file
6. Safely eject SD card
7. Re-insert into ESP32 and reset

### Add Your Game Assets

#### Adding Sprites
1. Create/find your sprite images
2. Convert to BMP format (24-bit or 16-bit)
3. Copy to `sprites/` folder on SD card
4. Example: `sprites/player.bmp`, `sprites/enemy.bmp`

#### Adding Sounds
1. Convert audio to WAV format
   - 8-bit or 16-bit PCM
   - 22050 Hz sample rate
   - Mono channel
2. Copy to `sounds/` folder on SD card
3. Example: `sounds/jump.wav`, `sounds/coin.wav`

#### Adding Game Data
1. Create game data files (levels, maps, scores)
2. Use simple formats: CSV, JSON, or plain text
3. Copy to `games/` folder on SD card
4. Example: `games/level1.map`, `games/highscores.txt`

## Troubleshooting Deployment

### Problem: SD Card Not Recognized by Computer

**Solutions:**
- Try a different card reader
- Clean SD card contacts with soft cloth
- Try a different SD card
- Check if card is write-protected (slide switch)

### Problem: "Cannot Format" Error

**Solutions:**
- Card may be damaged - try different card
- Use official SD Card Formatter tool
- Try formatting on different computer/OS
- Check if card is locked (write-protect switch)

### Problem: Files Won't Copy

**Solutions:**
- Ensure enough free space on SD card
- Check file names (avoid special characters)
- Try copying folders one at a time
- Disable antivirus temporarily during copy

### Problem: ESP32 Says "SD Card Not Found"

**Solutions:**
- ✓ Verify card is FAT32 formatted (not exFAT or NTFS)
- ✓ Check files are in root directory (not in subdirectory)
- ✓ Remove and re-insert SD card into module
- ✓ Try different SD card (compatibility varies)
- ✓ Check SD card module wiring
- ✓ Reformat card and copy files again

### Problem: Config File Not Loading

**Solutions:**
- Check file is exactly named `arcade.cfg` (not `arcade.cfg.txt`)
- Verify file is in `/config/` folder
- Open file to ensure no corruption
- Check for hidden file extensions (Windows may hide .txt)

## Quick Deployment Checklist

Before inserting SD card into ESP32:

- [ ] SD card formatted as FAT32
- [ ] All 4 folders copied to root: config, games, sprites, sounds
- [ ] arcade.cfg file exists in config folder
- [ ] No extra folders or files in root
- [ ] SD card safely ejected from computer
- [ ] Card fully inserted into SD module
- [ ] ESP32 powered off before inserting card

## Redeployment / Updates

To update files on your SD card:

1. Power off ESP32
2. Remove SD card
3. Insert into computer
4. Modify/add/delete files as needed
5. Safely eject
6. Re-insert into ESP32
7. Power on and test

**Tip:** Keep a backup copy of your SD card contents on your computer!

## Next Steps

✅ SD card prepared and deployed
✅ ESP32 recognizes SD card
→ Start adding your game assets
→ Customize arcade.cfg for your hardware
→ Begin game development!

See the main README.md for game development guides and examples.

---

**Need more help?** 
- Check HARDWARE_SETUP.md for wiring help
- See sd_card_contents/README.md for file format details
- Open an issue on GitHub if you're stuck
