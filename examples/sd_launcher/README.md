SD Launcher example

What this example does
- Demonstrates how to read a JSON-based menu and game assets from an SD card
- Shows how to initialize the MCP23017, TFT, and FastLED strip
- Implements a minimal Pac-Man clone that uses an ASCII map and simple sprites

Setup / Libraries
Install these libraries using the Arduino Library Manager:
- ArduinoJson (bblanchon)
- Adafruit MCP23X17
- FastLED
- TFT_eSPI
- DFPlayer_Mini_Mp3 (if you want DFPlayer support)
- Bluepad32 (optional — for controller support)

Copying the SD pack
Copy the contents of `sd_card/` to the root of your microSD card. Insert the card into the Hosyond board and power up.

Notes & Extensibility
- The pacman example uses an ASCII `level1.map`. You can add levels by creating more maps and adapting the `PacmanGame` class to load them.
- DFPlayer audio files are stored on the DFPlayer device's own microSD card. Use the top-level `dfplayer/` placeholders as examples and copy those files (e.g., `002.mp3`, `003.mp3`) to your DFPlayer SD. Update `games/*/sounds.json` to map names to tracks if needed.
- To implement firmware updates from SD: add a routine that looks for `/firmware/update.bin` and uses the `Update` API to write to flash.

Running identical C++ tests on a PC
- I added a platform-agnostic C++ Core UI library in `lib/core_ui/` and a desktop SDL2 simulator in `tools/desktop_simulator/`.
- The simulator compiles the same CoreUI code used by the ESP32 example and reads assets from `sd_card/` so you can run and test the UI/game behavior on your computer (see `tools/desktop_simulator/README.md`).

Auto-running code from SD
- You can copy source or binary files (Arduino sketch, compiled .bin) to the SD card, but the ESP32 will not automatically compile or run Arduino source files present on SD.
- To run code from SD without reflashing, the device must have a loader/engine (e.g., a script interpreter or a plugin system) that the firmware intentionally reads and executes. The example launcher can be extended to detect an `apps/` directory and dynamically load game descriptors and assets at runtime (no reflash required). If you want, I can add an `apps/` plugin format (folder + manifest + asset-driven binary) which the launcher will auto-run when found.

Limitations
- Running arbitrary compiled native ELF code from SD to execute in-place on the ESP32 is not supported without a dedicated loader/flashing process; flashing a `.bin` using the updater is possible but replaces firmware.
