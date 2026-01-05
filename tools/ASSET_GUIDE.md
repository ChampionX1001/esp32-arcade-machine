Asset guide — creating and replacing files for the SD pack

Audio
- DFPlayer Mini supports MP3 and WAV files. Use MP3 for small size.
- Name convention: use short, predictable names like `pacman_eat.mp3`. DFPlayer files must be copied to the DFPlayer microSD card (not the ESP32 SD). Use numeric file naming (e.g., `001.mp3`, `002.mp3`) if you want to call tracks by number from the DFPlayer.
- Many free tools can create MP3 files (Audacity, FFMPEG). For Windows, use:
  ffmpeg -i input.wav -vn -ar 44100 -ac 2 -b:a 128k output.mp3

Images / Sprites
- The example firmware uses ASCII sprite blocks in `sprites.json` for simplicity. If you want to use BMP/PNG files instead:
  - TFT_eSPI supports 24-bit BMP files. Save images in 16 or 24-bit BMP format.
  - Recommended tile sizes: 8x8, 16x16, or 32x32 pixels.
  - Avoid very large images to keep the memory use low.

Fonts
- If you want nicer fonts, you can generate TFT_eSPI-compatible fonts using the `FontCreator` utilities and drop them on SD, or include them within the firmware.

Tips
- Start with small images and low sample-rate MP3s to test everything works. Replace with higher-quality assets later.
- Keep filenames short and ASCII-only for compatibility with all SD readers.
