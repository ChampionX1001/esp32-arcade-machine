#pragma once
#include "../core_ui/CoreUI.h"
#include <TFT_eSPI.h>

class ESP32Screen : public Screen {
public:
  ESP32Screen(TFT_eSPI *tft) : _tft(tft) {}
  int width() override { return _tft->width(); }
  int height() override { return _tft->height(); }
  void fillScreen(uint32_t color) override { _tft->fillScreen(color); }
  void fillRect(int x, int y, int w, int h, uint32_t color) override { _tft->fillRect(x,y,w,h,color); }
  void drawText(int x, int y, const std::string &text, int size, uint32_t color) override {
    _tft->setTextSize(size);
    _tft->setTextColor(color);
    _tft->setCursor(x,y);
    _tft->print(text.c_str());
  }
private:
  TFT_eSPI *_tft;
};
