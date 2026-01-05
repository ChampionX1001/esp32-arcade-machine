#pragma once
#include "../core_ui/CoreUI.h"
#include <DFRobotDFPlayerMini.h>

class ESP32Sound : public Sound {
public:
  ESP32Sound(DFRobotDFPlayerMini *player) : _player(player) {}
  void playTrack(int id) override { if (_player) _player->play(id); }
private:
  DFRobotDFPlayerMini *_player;
};
