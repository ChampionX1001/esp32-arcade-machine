#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Adafruit_MCP23X17.h>
#include <vector>
#include <FS.h>
#include <SD.h>
#include <ArduinoJson.h>

class PacmanGame {
public:
  PacmanGame(TFT_eSPI *tft, Adafruit_MCP23X17 *mcp, DFRobotDFPlayerMini *df = nullptr) : _tft(tft), _mcp(mcp), _df(df) {}
  bool loadFromSD(const char *path);
  void run();
private:
  TFT_eSPI *_tft;
  Adafruit_MCP23X17 *_mcp;
  DFRobotDFPlayerMini *_df;
  std::vector<String> map;
  int tileSize = 8;
  int cols = 0, rows = 0;
  int px = 1, py = 1; // player tile coords
  void drawTile(int r, int c);
  void drawMap();
  void movePlayer(int dx, int dy);
};

// Implementation inline for a single-file example

bool PacmanGame::loadFromSD(const char *path) {
  // load level1.map
  String mapPath = String(path) + "/level1.map";
  if (!SD.exists(mapPath)) return false;
  File f = SD.open(mapPath);
  if (!f) return false;
  map.clear();
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    if (line.startsWith("#")) continue; // comment
    map.push_back(line);
  }
  f.close();
  rows = map.size();
  cols = (rows>0) ? map[0].length() : 0;
  // find player start
  for (int r=0; r<rows; r++){
    for (int c=0; c<cols; c++){
      if (map[r][c] == 'P') { px = c; py = r; map[r].setCharAt(c, '.'); }
    }
  }
  return true;
}

void PacmanGame::drawTile(int r, int c) {
  char ch = map[r][c];
  int x = c * tileSize;
  int y = r * tileSize;
  if (ch == '#') {
    _tft->fillRect(x, y, tileSize, tileSize, TFT_BLUE);
  } else if (ch == '.') {
    _tft->fillRect(x, y, tileSize, tileSize, TFT_BLACK);
    _tft->fillRect(x + tileSize/2 - 1, y + tileSize/2 - 1, 2, 2, TFT_YELLOW);
  } else {
    _tft->fillRect(x, y, tileSize, tileSize, TFT_BLACK);
  }
}

void PacmanGame::drawMap() {
  for (int r=0; r<rows; r++){
    for (int c=0; c<cols; c++) drawTile(r,c);
  }
  // draw player
  _tft->fillRect(px*tileSize, py*tileSize, tileSize, tileSize, TFT_ORANGE);
}

void PacmanGame::movePlayer(int dx, int dy) {
  int nx = px + dx;
  int ny = py + dy;
  if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) return;
  if (map[ny][nx] == '#') return;
  px = nx; py = ny;
  if (map[ny][nx] == '.') {
    map[ny].setCharAt(nx, ' ');
    // play eat sound via DFPlayer if available
    if (_df) {
      // play sound track 3 (see sd_card/dfplayer/003.mp3)
      _df->play(3);
    }
  }
}

void PacmanGame::run() {
  unsigned long lastFrame = millis();
  const int frameMs = 100;
  drawMap();
  while (true) {
    // read MCP buttons for movement
    if (_mcp->digitalRead(0) == LOW) movePlayer(0, -1); // up
    if (_mcp->digitalRead(1) == LOW) movePlayer(0, +1); // down
    if (_mcp->digitalRead(2) == LOW) movePlayer(-1, 0); // left
    if (_mcp->digitalRead(3) == LOW) movePlayer(+1, 0); // right
    if (_mcp->digitalRead(5) == LOW) { // button2 = exit
      delay(300);
      break; // exit to menu
    }
    if (millis() - lastFrame > frameMs) {
      lastFrame = millis();
      drawMap();
    }
    delay(10);
  }
}
