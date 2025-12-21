Pac-Man game template

This folder contains a minimal Pac-Man-style level and the assets the launcher expects:
- `level1.map` — ASCII map used by the example `PacmanGame` class
- `sprites.json` — small ASCII tile definitions used by the simple renderer
- `sounds.json` — mapping of sound names to filenames (for DFPlayer/firmware)
- `manifest.json` — game manifest
- `thumb.bmp` or `thumb.txt` — thumbnail placeholder

To add sounds to DFPlayer, copy the MP3 files to the DFPlayer SD with numeric names. Example mapping from `sounds.json`:
{
  "eat": "003.mp3",
  "start": "002.mp3"
}

The example firmware plays track 2 on game start and track 3 for pellet eating if the DFPlayer exists.