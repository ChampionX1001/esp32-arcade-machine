#pragma once

#include <string>
#include <vector>
#include <functional>

// Platform abstraction interfaces
struct Screen {
  virtual ~Screen() = default;
  virtual int width() = 0;
  virtual int height() = 0;
  virtual void fillScreen(uint32_t color) = 0;
  virtual void fillRect(int x, int y, int w, int h, uint32_t color) = 0;
  virtual void drawText(int x, int y, const std::string &text, int size, uint32_t color) = 0;
};

struct Input {
  virtual ~Input() = default;
  virtual bool up() = 0;
  virtual bool down() = 0;
  virtual bool left() = 0;
  virtual bool right() = 0;
  virtual bool btnA() = 0; // select
  virtual bool btnB() = 0; // back
  virtual bool btnX() = 0; // extra
  virtual bool btnY() = 0; // extra
};

struct Sound {
  virtual ~Sound() = default;
  virtual void playTrack(int id) = 0; // simple numeric track play
};

struct FileSystem {
  virtual ~FileSystem() = default;
  virtual bool exists(const std::string &path) = 0;
  virtual bool readAll(const std::string &path, std::string &out) = 0;
};

// Lightweight color helpers (RGB 0xRRGGBB)
constexpr uint32_t COL_BLACK = 0x000000;
constexpr uint32_t COL_WHITE = 0xFFFFFF;
constexpr uint32_t COL_YELLOW = 0xFFFF00;
constexpr uint32_t COL_DARKGRAY = 0x404040;
constexpr uint32_t COL_LIGHTGRAY = 0xA0A0A0;
constexpr uint32_t COL_BLUE = 0x0000FF;
constexpr uint32_t COL_ORANGE = 0xFF8000;

// Core UI logic — platform independent
class CoreUI {
public:
  CoreUI(Screen *screen, Input *input, Sound *sound, FileSystem *fs, const std::string &sdRoot = "sd_card");

  // simple flows
  void showBoot(const std::string &bootPath);
  void mainMenu(const std::string &menuPath);
  void settings();

  // Pacman
  bool loadPacman(const std::string &path);
  void runPacman();

  // helpers
  int getVolume() const { return _volume; }
  void setVolume(int v) { _volume = v; if (_sound) _sound->playTrack(0); }

private:
  Screen *_screen;
  Input *_input;
  Sound *_sound;
  FileSystem *_fs;
  std::string _sdRoot;

  int _volume = 20;
  bool _ledsOn = true;

  // Menu state
  struct MenuItem { std::string id; std::string title; std::string path; std::string thumbnail; };
  std::vector<MenuItem> _menuItems;

  // Pacman state
  std::vector<std::string> _map;
  int _p_px = 1, _p_py = 1;
  int _tileSize = 8;
  int _rows = 0, _cols = 0;

  // internal helpers
  void renderMenu(int sel);
  void renderPacmanFrame();
  void movePlayer(int dx, int dy);
};
