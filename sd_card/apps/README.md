Apps plugin format

Place app folders under `/apps/` on the ESP32 SD card. Each app folder should contain a `manifest.json` with at least:
- `id` (unique id)
- `name` (display name)
- `path` (path to app assets; can point to `/games/<game>` or contain assets in-place)

Example:
- `/apps/pacman_app/manifest.json` -> points to `/games/pacman`

The launcher will detect folders in `/apps` with a `manifest.json` and add them to the main menu automatically, so users can add/remove apps without reflashing.
