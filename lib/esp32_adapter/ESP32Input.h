#pragma once
#include "../core_ui/CoreUI.h"
#include <Adafruit_MCP23X17.h>

class ESP32Input : public Input {
public:
  ESP32Input(Adafruit_MCP23X17 *mcp, int up=0,int down=1,int left=2,int right=3,int a=4,int b=5,int x=6,int y=7)
    : _mcp(mcp), pin_up(up), pin_down(down), pin_left(left), pin_right(right), pin_a(a), pin_b(b), pin_x(x), pin_y(y) {}
  bool up() override { return (_mcp->digitalRead(pin_up) == LOW); }
  bool down() override { return (_mcp->digitalRead(pin_down) == LOW); }
  bool left() override { return (_mcp->digitalRead(pin_left) == LOW); }
  bool right() override { return (_mcp->digitalRead(pin_right) == LOW); }
  bool btnA() override { return (_mcp->digitalRead(pin_a) == LOW); }
  bool btnB() override { return (_mcp->digitalRead(pin_b) == LOW); }
  bool btnX() override { return (_mcp->digitalRead(pin_x) == LOW); }
  bool btnY() override { return (_mcp->digitalRead(pin_y) == LOW); }
private:
  Adafruit_MCP23X17 *_mcp;
  int pin_up, pin_down, pin_left, pin_right, pin_a, pin_b, pin_x, pin_y;
};
