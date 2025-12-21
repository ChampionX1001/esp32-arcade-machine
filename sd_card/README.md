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
- /audio/                -> MP3 placeholders for DFPlayer

Notes
- Replace placeholder MP3 files in `/audio/` with your own MP3s. DFPlayer expects filenames like `001.mp3`, but we also provide mapping files so the firmware can call named sounds.
- Fonts are drawn by the firmware; you can drop bitmap files here if you add loader support.

See `../SD_PACK.md` for more detailed usage and how the example launcher uses these files.