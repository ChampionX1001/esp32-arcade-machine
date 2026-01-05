#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Adafruit_MCP23X17.h>
#include <DFRobotDFPlayerMini.h>

#include "../../lib/core_ui/CoreUI.h"
#include "../../lib/esp32_adapter/ESP32Screen.h"
#include "../../lib/esp32_adapter/ESP32Input.h"
#include "../../lib/esp32_adapter/ESP32Sound.h"
#include "../../lib/esp32_adapter/ESP32FS.h"

TFT_eSPI tft;
Adafruit_MCP23X17 mcp;
DFRobotDFPlayerMini dfplayer;

void setup() {
  Serial.begin(115200);
  delay(100);
  tft.init(); tft.setRotation(1);

  if (!SD.begin()) {
    Serial.println("SD init failed");
  }

  Wire.begin(); mcp.begin_I2C();
  Serial1.begin(9600, SERIAL_8N1, 16, 17); // pins default; update if needed
  dfplayer.begin(Serial1);

  ESP32Screen screen(&tft);
  ESP32Input input(&mcp);
  ESP32Sound sound(&dfplayer);
  ESP32FS fs;

  // pass delay wrapper as sleeper
  CoreUI ui(&screen, &input, &sound, &fs, "/", [](int ms){ delay(ms); });

  // boot
  ui.showBoot("/launcher/boot.txt");
  // wait for any button
  while (!input.btnA() && !input.btnB() && !input.btnX() && !input.btnY()) { delay(20); }

  // launch menu (blocks until user exits)
  ui.mainMenu("/launcher/menu.json");
}

void loop() {
}
