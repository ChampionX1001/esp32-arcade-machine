This folder is the SD-card payload for the Hosyond ESP32-32E arcade machine.

Copy the contents of this `sd_card/` folder directly to the root of your microSD card. The launcher (ESP32 firmware) will look for `/config.json`, `/launcher/menu.json`, and `/games/*`.

Structure
- /config.json           -> global hardware/config settings
- /launcher/menu.json    -> list of games shown in the startup menu
- /games/<game>/         -> each game directory with its assets
  - manifest.json
  - level1.map
  - sprites.json
  - sounds.json
- DFPlayer audio files should be placed on the DFPlayer's own microSD card (see top-level `dfplayer/` placeholders for examples).

Notes
- Copy MP3s like `001.mp3`, `002.mp3` to the DFPlayer SD card. The example firmware will call tracks by number using the DFPlayer library.
- Fonts are drawn by the firmware; you can drop bitmap files here if you add loader support.

See `../SD_PACK.md` for more detailed usage and how the example launcher uses these files.