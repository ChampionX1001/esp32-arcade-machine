ESP32 Arcade SD Pack — Usage Guide

Overview
This SD-pack provides a simple, extensible data layout which the example launcher firmware (`examples/sd_launcher`) will read at boot. Copy the `sd_card/` contents to the root of your microSD card.

Files of interest
- `/config.json` — hardware mapping and defaults (MCP23017 address, LED pin, DFPlayer tracks)
- `/launcher/menu.json` — menu entries displayed at startup
- `/games/<game>/` — assets for each game (map, sprites, sounds)

How the example launcher works
1. On boot the firmware reads `/config.json` and `/launcher/menu.json`.
2. The TFT displays the menu and you can navigate using the MCP23017-driven buttons or a Bluetooth controller (Bluepad32).
3. Selecting a game will load its manifest and assets from the game's folder and start the game loop.

Apps plugin format
- You can add app folders under `/apps/`. Each app must contain a `manifest.json` with `id`, `name`, and `path` fields (path may point to an existing `/games/<game>` folder). The launcher automatically detects these apps and adds them to the menu.

MP3 files
- DFPlayer MP3 files should be placed on the DFPlayer's own microSD card (not the ESP32's SD). Use numeric filenames like `001.mp3`, `002.mp3` etc. This repository includes top-level `dfplayer/` placeholders to prepare the DFPlayer SD if needed. Update `games/<game>/sounds.json` to map named sounds to track numbers or filenames as appropriate for your DFPlayer setup.

FAQ
Q: Can I put a `.bin` firmware on the SD and make the ESP32 flash itself?
A: Yes — the ESP32 can perform an update from SD (using the `Update` API). The `examples/sd_launcher/firmware_updater.ino` file contains a small helper routine `installUpdateFromSD()` that looks for `/firmware/update.bin` and writes it to flash.

How to use it safely:
- Place your compiled `.bin` on the SD card at `/firmware/update.bin` (double-check board and partitioning).
- Boot the device into a state where the updater routine runs (e.g., a debug menu) and call `installUpdateFromSD()`.
- The device will attempt to write the image and then restart if successful.

Warning: firmware updates overwrite flash and may brick your device if the binary is invalid or for a different board. Use with care.

Notes
- This pack is intentionally small and self-contained so you can inspect and replace assets easily.
- Remember to keep the audio filenames and menu manifests consistent.

Next step: flash or run the example launcher sketch in `examples/sd_launcher/`.

Simulating the UI on Windows
- Use the Python simulator in `tools/ui_simulator/sim.py` to quickly exercise the UI without hardware.
- Install dependencies: `pip install pygame` and run `python tools/ui_simulator/sim.py`.
- The simulator reads `sd_card/launcher/menu.json` and `sd_card/launcher/ui.json` and allows arrow keys and Z/X keys to simulate joystick and buttons.

DFPlayer audio for the DFPlayer Mini device
- The DFPlayer uses its OWN microSD card. Copy files named `001.mp3`, `002.mp3`, etc. into the DFPlayer's SD card. We also include a top-level `dfplayer/` folder with placeholder MP3 files to help prepare the DFPlayer SD card. Ensure the DFPlayer's files are numbered for easy mapping to track numbers used in code (e.g., `002.mp3` = track 2).