#pragma once
#include "../core_ui/CoreUI.h"
#include <FS.h>
#include <SD.h>

class ESP32FS : public FileSystem {
public:
  ESP32FS() {}
  bool exists(const std::string &path) override { return SD.exists(path.c_str()); }
  bool readAll(const std::string &path, std::string &out) override {
    if (!SD.exists(path.c_str())) return false;
    File f = SD.open(path.c_str(), FILE_READ);
    if (!f) return false;
    out.clear();
    while (f.available()) out.push_back((char)f.read());
    f.close();
    return true;
  }
};
