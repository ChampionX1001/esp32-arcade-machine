#include "CoreUI.h"
#include <sstream>
#include <algorithm>

#include <nlohmann/json.hpp>
#include <thread>
using json = nlohmann::json;

CoreUI::CoreUI(Screen *screen, Input *input, Sound *sound, FileSystem *fs, const std::string &sdRoot, std::function<void(int)> sleeper)
  : _screen(screen), _input(input), _sound(sound), _fs(fs), _sdRoot(sdRoot) {
  if (sleeper) _sleeper = sleeper; else _sleeper = [](int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); };
}

void CoreUI::showBoot(const std::string &bootPath) {
  std::string text;
  if (_fs->readAll(bootPath, text)) {
    _screen->fillScreen(COL_BLACK);
    _screen->drawText(_screen->width()/2 - 80, _screen->height()/2 - 10, text, 2, COL_WHITE);
  } else {
    _screen->fillScreen(COL_BLACK);
    _screen->drawText(10, 10, "Hosyond Arcade", 3, COL_WHITE);
    _screen->drawText(10, 50, "Press any button to continue", 2, COL_LIGHTGRAY);
  }
}

void CoreUI::renderMenu(int sel) {
  _screen->fillScreen(COL_BLACK);
  _screen->drawText(10, 10, "Main Menu", 2, COL_WHITE);

  int x0 = 10;
  int y = 40;
  for (size_t i = 0; i < _menuItems.size(); ++i) {
    int x = x0 + int(i) * 90;
    _screen->fillRect(x, y, 80, 60, (int)i == sel ? COL_DARKGRAY : COL_LIGHTGRAY);
    _screen->drawText(x + 4, y + 65, _menuItems[i].title, 1, (int)i == sel ? COL_YELLOW : COL_WHITE);
  }
}

void CoreUI::mainMenu(const std::string &menuPath) {
  // load menu JSON if present; otherwise rely on programmatically-populated _menuItems
  _menuItems.clear();
  std::string raw;
  if (_fs->readAll(menuPath, raw)) {
    auto j = json::parse(raw);
    for (auto &it : j["menu"]) {
      MenuItem mi;
      mi.id = it.value("id", "");
      mi.title = it.value("title", "");
      mi.path = it.value("path", "");
      mi.thumbnail = it.value("thumbnail", "");
      _menuItems.push_back(mi);
    }
  } else {
    // keep existing _menuItems (possibly populated by host)
  }

  int sel = 0;
  while (true) {
    renderMenu(sel);
    // navigation
    if (_input->left()) { sel = std::max(0, sel-1); }
    if (_input->right()) { sel = std::min((int)_menuItems.size()-1, sel+1); }
    if (_input->btnA()) {
      // launch
      if (_menuItems[sel].id == "pacman") {
        loadPacman(_menuItems[sel].path);
        if (_sound) _sound->playTrack(2); // start track
        runPacman();
        // return to menu after game
      }
    }
    if (_input->btnB()) { return; }

    // tiny debounce / frame delay
    _sleeper(50);
  }
}

void CoreUI::settings() {
  int idx = 0; // 0 volume, 1 leds
  while (true) {
    _screen->fillScreen(COL_BLACK);
    _screen->drawText(10, 10, "Settings", 2, COL_WHITE);
    _screen->drawText(10, 40, "Volume: " + std::to_string(_volume), 1, COL_WHITE);
    _screen->drawText(10, 70, std::string("LEDs: ") + (_ledsOn?"On":"Off"), 1, COL_WHITE);
    _screen->drawText(10, 110, "Press B to back", 1, COL_LIGHTGRAY);

    if (_input->left()) {
      if (idx == 0) _volume = std::max(0, _volume - 1);
      if (idx == 1) _ledsOn = !_ledsOn;
    }
    if (_input->right()) {
      if (idx == 0) _volume = std::min(30, _volume + 1);
      if (idx == 1) _ledsOn = !_ledsOn;
    }
    if (_input->btnA()) { idx = (idx+1)%2; }
    if (_input->btnB()) { break; }
    if (_sound) _sound->playTrack(0);
    _sleeper(80);
  }
}

bool CoreUI::loadPacman(const std::string &path) {
  _map.clear();
  std::string mapPath = path + "/level1.map";
  std::string raw;
  if (!_fs->readAll(mapPath, raw)) return false;
  std::istringstream ss(raw);
  std::string line;
  while (std::getline(ss, line)) {
    // skip comments
    if (line.size() == 0) continue;
    if (line[0] == '#') continue;
    _map.push_back(line);
  }
  _rows = (int)_map.size();
  _cols = (_rows>0) ? (int)_map[0].size() : 0;
  for (int r=0; r<_rows; ++r) for (int c=0; c<_cols; ++c) {
    if (_map[r][c] == 'P') { _p_px = c; _p_py = r; _map[r][c] = '.'; }
  }
  return true;
}

void CoreUI::renderPacmanFrame() {
  _screen->fillScreen(COL_BLACK);
  for (int r=0; r<_rows; ++r) for (int c=0; c<_cols; ++c) {
    char ch = _map[r][c];
    int x = c * _tileSize;
    int y = r * _tileSize;
    if (ch == '#') _screen->fillRect(x, y, _tileSize, _tileSize, COL_BLUE);
    else if (ch == '.') {
      _screen->fillRect(x, y, _tileSize, _tileSize, COL_BLACK);
      _screen->fillRect(x + _tileSize/2 - 1, y + _tileSize/2 - 1, 2, 2, COL_YELLOW);
    } else _screen->fillRect(x, y, _tileSize, _tileSize, COL_BLACK);
  }
  // draw player
  _screen->fillRect(_p_px*_tileSize, _p_py*_tileSize, _tileSize, _tileSize, COL_ORANGE);
}

void CoreUI::movePlayer(int dx, int dy) {
  int nx = _p_px + dx;
  int ny = _p_py + dy;
  if (nx < 0 || nx >= _cols || ny < 0 || ny >= _rows) return;
  if (_map[ny][nx] == '#') return;
  _p_px = nx; _p_py = ny;
  if (_map[ny][nx] == '.') {
    _map[ny][nx] = ' ';
    if (_sound) _sound->playTrack(3); // eat
  }
}

void CoreUI::runPacman() {
  using namespace std::chrono_literals;
  renderPacmanFrame();
  while (true) {
    if (_input->up()) movePlayer(0, -1);
    if (_input->down()) movePlayer(0, +1);
    if (_input->left()) movePlayer(-1, 0);
    if (_input->right()) movePlayer(+1, 0);
    if (_input->btnB()) { break; }
    renderPacmanFrame();
    _sleeper(100);
  }
}

void CoreUI::clearMenu() { _menuItems.clear(); }

void CoreUI::addMenuItem(const std::string &id, const std::string &title, const std::string &path, const std::string &thumbnail) {
  MenuItem mi; mi.id = id; mi.title = title; mi.path = path; mi.thumbnail = thumbnail; _menuItems.push_back(mi);
}
