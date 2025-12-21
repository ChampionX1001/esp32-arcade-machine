// firmware_updater.ino
// Example: how the ESP32 can flash a firmware file from the SD card.
// Place a firmware binary at /firmware/update.bin on the SD card. When the function
// `installUpdateFromSD()` runs, it will attempt to write that file to flash.

#include <Arduino.h>
#include <Update.h>
#include <FS.h>
#include <SD.h>

bool installUpdateFromSD(const char *path = "/firmware/update.bin") {
  if (!SD.exists(path)) return false;
  File f = SD.open(path);
  if (!f) return false;

  size_t size = f.size();
  Serial.printf("Updating from %s, size=%u bytes\n", path, (unsigned)size);

  if (!Update.begin(size)) {
    Serial.println("Not enough space for update");
    f.close();
    return false;
  }

  size_t written = Update.writeStream(f);
  if (written != size) {
    Serial.printf("Update wrote %u of %u bytes\n", (unsigned)written, (unsigned)size);
  }

  if (!Update.end()) {
    Serial.printf("Update failed: %s\n", Update.errorString());
    f.close();
    return false;
  }

  f.close();
  if (Update.isFinished()) {
    Serial.println("Update successful — restarting");
    ESP.restart();
    return true;
  }
  Serial.println("Update not finished");
  return false;
}

// WARNING: Running update will erase application flash and reboot. Use with caution.
