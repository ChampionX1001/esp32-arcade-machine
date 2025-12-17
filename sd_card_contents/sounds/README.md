# Sounds Directory

Store all audio files for your arcade machine here.

## Recommended Audio Formats

### WAV Format (Recommended)
- **Best choice** for ESP32 compatibility
- PCM (uncompressed) format
- 8-bit or 16-bit depth
- Mono channel (stereo uses more memory)
- Sample rates: 8000, 11025, 22050, or 44100 Hz

### File Size Considerations
- 8-bit, 22050 Hz, Mono: ~22 KB per second
- 16-bit, 44100 Hz, Mono: ~88 KB per second
- Keep sound effects short (under 1 second when possible)

## Sound Naming Convention

Use descriptive names for your sound effects:
```
jump.wav              # Player jump sound
coin.wav              # Collect coin sound
explosion.wav         # Explosion effect
powerup.wav           # Power-up collect sound
game_over.wav         # Game over sound
level_complete.wav    # Level completion sound
menu_select.wav       # Menu selection beep
button_press.wav      # Button press click
```

## Audio Specifications

### Recommended Settings for Short SFX
- **Format**: WAV (PCM)
- **Bit Depth**: 8-bit
- **Sample Rate**: 22050 Hz
- **Channels**: Mono
- **Duration**: 0.1 - 2.0 seconds

### Recommended Settings for Music/Longer Audio
- **Format**: WAV (PCM)
- **Bit Depth**: 16-bit
- **Sample Rate**: 22050 Hz or 44100 Hz
- **Channels**: Mono
- **Duration**: Keep under 30 seconds if possible

## Creating/Converting Audio Files

### Using Audacity (Free)
1. Import or record your audio
2. Convert to mono: Tracks → Mix → Mix Stereo Down to Mono
3. Change sample rate: Tracks → Resample → 22050 Hz
4. Export as WAV:
   - File → Export → Export as WAV
   - Choose "Signed 8-bit PCM" or "Signed 16-bit PCM"

### Using Online Tools
- Online-Convert.com
- CloudConvert.com
- Audio-Convert.com

### Command Line (FFmpeg)
```bash
# Convert to 8-bit, 22050 Hz, mono WAV
ffmpeg -i input.mp3 -acodec pcm_u8 -ar 22050 -ac 1 output.wav

# Convert to 16-bit, 22050 Hz, mono WAV
ffmpeg -i input.mp3 -acodec pcm_s16le -ar 22050 -ac 1 output.wav
```

## Playing Sounds in Code

Example code to play a sound from SD card:
```cpp
#include <SD.h>
// Include your audio library (e.g., ESP8266Audio, PCM library)

// Play sound effect
File audioFile = SD.open("/sounds/coin.wav");
if (audioFile) {
  // Read WAV header
  // Play audio data through DAC or I2S
  audioFile.close();
}
```

## Audio Hardware Options

### Built-in DAC (Digital-to-Analog Converter)
- GPIO 25 and GPIO 26 on ESP32
- Simple, no external hardware needed
- Lower quality but sufficient for basic sound effects

### I2S Audio
- Better quality audio output
- Requires I2S DAC module (e.g., MAX98357A)
- More complex setup but professional results

### PWM Speaker
- Direct speaker connection through PWM
- Very simple, low quality
- Good for basic beeps and tones

## Sound Effect Tips

1. **Keep it short**: Arcade sound effects should be snappy (under 1 second)
2. **Normalize audio**: Ensure consistent volume levels
3. **Remove silence**: Trim dead air at start/end of files
4. **Test on device**: Audio may sound different on small speaker vs. computer
5. **Avoid clipping**: Check that audio doesn't distort at full volume

## Current Sounds
(Document your sound files here)

- No sounds installed yet - add your first sound effect!
