#pragma once

#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <Adafruit_MCP23X17.h>
#include <DFRobotDFPlayerMini.h>

class UIEngine {
public:
  UIEngine(TFT_eSPI *tft, Adafruit_MCP23X17 *mcp, DFRobotDFPlayerMini *df, int batteryPin = -1, const char *version = "0.0")
    : _tft(tft), _mcp(mcp), _df(df), _batteryPin(batteryPin), _version(version) {}

  void showBoot(const char *path);
  String mainMenu(const char *menuPath);
  void settings();
  void gamesList(const char *menuPath);
  int waitForAnyButton(unsigned long timeout = 0);
  void setLEDsEnabled(bool on);
  void setVolume(int vol);
  int getVolume() { return _volume; }
private:
  TFT_eSPI *_tft;
  Adafruit_MCP23X17 *_mcp;
  DFRobotDFPlayerMini *_df;
  int _batteryPin;
  const char *_version;
  int _volume = 20;
  bool _ledsOn = true;

  bool readJSONFile(const char *path, DynamicJsonDocument &doc, size_t cap=4096);
  bool buttonPressed(int pin) { return (_mcp->digitalRead(pin) == LOW); }
};

// Implementation
#include <FS.h>
#include <SD.h>

bool UIEngine::readJSONFile(const char *path, DynamicJsonDocument &doc, size_t cap) {
  if (!SD.exists(path)) return false;
  File f = SD.open(path, FILE_READ);
  if (!f) return false;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  return !err;
}

int UIEngine::waitForAnyButton(unsigned long timeout) {
  unsigned long start = millis();
  while (true) {
    for (int i = 0; i < 16; i++) {
      if (_mcp->pinMode && _mcp->digitalRead(i) == LOW) {
        return i;
      }
    }
    if (timeout && millis()-start > timeout) return -1;
    delay(10);
  }
}

void UIEngine::showBoot(const char *path) {
  // draw simple text-based boot screen
  String s = "";
  if (SD.exists(path)) {
    File f = SD.open(path);
    while (f.available()) s += (char)f.read();
    f.close();
  } else {
    s = "Hosyond Arcade\nPress any button";
  }
  _tft->fillScreen(TFT_BLACK);
  _tft->setTextColor(TFT_WHITE);
  _tft->setTextDatum(MC_DATUM);
  _tft->drawString(s, _tft->width()/2, _tft->height()/2);
}

void UIEngine::setLEDsEnabled(bool on) { _ledsOn = on; }
void UIEngine::setVolume(int vol) { _volume = constrain(vol, 0, 30); if (_df) _df->volume(_volume); }

String UIEngine::mainMenu(const char *menuPath) {
  DynamicJsonDocument menuDoc(4096);
  if (!readJSONFile(menuPath, menuDoc)) return String("");
  JsonArray menu = menuDoc["menu"].as<JsonArray>();
  int sel = 0;
  while (true) {
    _tft->fillScreen(TFT_BLACK);
    _tft->setTextSize(2);
    _tft->setTextColor(TFT_WHITE);
    _tft->setCursor(10, 10);
    _tft->println("Main Menu");

    for (int i=0; i < (int)menu.size(); i++) {
      int x = 10 + i*90;
      int y = 40;
      // thumbnail or placeholder
      const char *thumb = menu[i]["thumbnail"];
      _tft->fillRect(x, y, 80, 60, (i==sel)?TFT_DARKGREY:TFT_LIGHTGREY);
      _tft->setTextColor((i==sel)?TFT_YELLOW:TFT_WHITE);
      _tft->setCursor(x+5, y+65);
      _tft->print(menu[i]["title"].as<const char*>());
    }

    // read joystick left/right
    if (buttonPressed(2)) { sel = max(0, sel-1); delay(150); }
    if (buttonPressed(3)) { sel = min((int)menu.size()-1, sel+1); delay(150); }
    if (buttonPressed(4)) { // button1: select
      const char* path = menu[sel]["path"].as<const char*>();
      // return the selected path to the caller
      return String(path);
    }
    if (buttonPressed(5)) { // button2: go to settings
      settings(); delay(200);
    }
    delay(10);
  }
}

void UIEngine::settings() {
  // simple two controls: volume and LED toggle
  int idx = 0; // 0=vol, 1=led
  while (true) {
    _tft->fillScreen(TFT_BLACK);
    _tft->setTextSize(2);
    _tft->setTextColor(TFT_WHITE);
    _tft->setCursor(10,10);
    _tft->println("Settings");

    _tft->setCursor(10,40);
    _tft->print("Volume: "); _tft->print(_volume);
    _tft->setCursor(10,70);
    _tft->print("LEDs: "); _tft->println(_ledsOn?"On":"Off");
    _tft->setCursor(10,120);
    _tft->print("Press Button2 to back");

    if (buttonPressed(2)) { // left/volume down
      if (idx == 0) { setVolume(_volume - 1); delay(150); }
    }
    if (buttonPressed(3)) { // right/volume up
      if (idx == 0) { setVolume(_volume + 1); delay(150); }
    }
    if (buttonPressed(4)) { // button1 toggles led control when on idx 1
      idx = (idx+1)%2; delay(150);
    }
    if (buttonPressed(5)) { // back
      delay(200); break; }

    // toggle LED when idx==1 and left/right pressed
    if (idx==1) {
      if (buttonPressed(2) || buttonPressed(3)) { setLEDsEnabled(!_ledsOn); delay(150); }
    }
    delay(10);
  }
}

void UIEngine::gamesList(const char *menuPath) {
  // re-use main menu for now (could be different UI)
  mainMenu(menuPath);
}
