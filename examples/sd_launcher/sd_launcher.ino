/*
  sd_launcher.ino
  Example launcher that reads a simple menu and game assets from the microSD card.

  Features included:
  - Reads /config.json and /launcher/menu.json from SD
  - Initializes MCP23017 buttons (Adafruit_MCP23X17)
  - Initializes WS2812 LED demo (FastLED)
  - Initializes DFPlayer serial for MP3 playback
  - Simple menu on TFT (TFT_eSPI)
  - Loads the Pac-Man game assets from /games/pacman and runs a minimal playable loop.

  Libraries required (install via Library Manager):
  - ArduinoJson
  - Adafruit MCP23X17
  - FastLED
  - DFPlayer_Mini_Mp3
  - Bluepad32
  - TFT_eSPI
  - SD (or SD_MMC on some boards)

  This example is intentionally small and heavily commented so you can extend it.
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include "PacmanGame.h"
#include "UI.h"
#include <DFRobotDFPlayerMini.h>

// Hardware globals (defaults, may be overwritten by sd config.json)
Adafruit_MCP23X17 mcp;
TFT_eSPI tft = TFT_eSPI();
DFRobotDFPlayerMini dfplayer;

// LED strip
#define MAX_LEDS 144
CRGB leds[MAX_LEDS];
int ledDataPin = 2;
int numPixels = 32;

// DFPlayer pins (default, may be overridden)
int df_rx_pin = 16; // to DFPlayer TX
int df_tx_pin = 17; // to DFPlayer RX

// SD paths
const char *CONFIG_PATH = "/config.json";
const char *MENU_PATH = "/launcher/menu.json";

// Simple helper: read a JSON file from SD into a DynamicJsonDocument
bool loadJsonFromSD(const char *path, DynamicJsonDocument &doc, size_t cap=4096) {
  if (!SD.exists(path)) {
    Serial.printf("File not found: %s\n", path);
    return false;
  }
  File f = SD.open(path, FILE_READ);
  if (!f) return false;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    Serial.printf("JSON parse error in %s: %s\n", path, err.c_str());
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("SD Launcher starting...");

  // Initialize TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.println("Initializing...");

  // Initialize SD
  if (!SD.begin()) {
    Serial.println("SD.begin() failed — check your wiring/CS pin and that SD is inserted.");
    tft.println("SD init failed");
    while (1) { delay(1000); }
  }

  // Load config.json
  DynamicJsonDocument cfg(4096);
  if (loadJsonFromSD(CONFIG_PATH, cfg)) {
    // read MCP address (optional), led settings
    if (cfg.containsKey("led_strip")) {
      ledDataPin = cfg["led_strip"]["data_pin"] | ledDataPin;
      numPixels = cfg["led_strip"]["num_pixels"] | numPixels;
      if (numPixels > MAX_LEDS) numPixels = MAX_LEDS;
    }
    if (cfg.containsKey("dfplayer")) {
      df_rx_pin = cfg["dfplayer"]["rx_pin"] | df_rx_pin;
      df_tx_pin = cfg["dfplayer"]["tx_pin"] | df_tx_pin;
    }
  } else {
    Serial.println("No config found — using defaults");
  }

  // Init MCP23017 on I2C
  Wire.begin();
  if (!mcp.begin_I2C()) {
    Serial.println("Failed to init MCP23017 — check I2C");
  } else {
    // example: set first 8 pins as inputs with internal pullups (up/down/left/right + 4 buttons)
    for (int i = 0; i < 8; i++) {
      mcp.pinMode(i, INPUT);
      mcp.pullUp(i, HIGH);
    }
  }

  // Init LED strip (FastLED)
  FastLED.addLeds<WS2812B, 2, GRB>(leds, numPixels);
  FastLED.setBrightness(64);

  // Init DFPlayer on Serial1
  Serial1.begin(9600, SERIAL_8N1, df_rx_pin, df_tx_pin);
  if (!dfplayer.begin(Serial1)) {
    Serial.println("DFPlayer not found or not responding");
  } else {
    dfplayer.volume(20);
  }

  // Instantiate UI engine
  UIEngine ui(&tft, &mcp, &dfplayer, -1, "0.1");

  // Boot screen
  DynamicJsonDocument cfgDoc(2048);
  loadJsonFromSD(CONFIG_PATH, cfgDoc);
  const char *bootPath = "/launcher/boot.txt";
  if (cfgDoc.containsKey("ui") && cfgDoc["ui"].containsKey("boot_path")) bootPath = cfgDoc["ui"]["boot_path"].as<const char*>();
  ui.showBoot(bootPath);

  // Wait for any button, then jump to main menu
  ui.waitForAnyButton();

  while (true) {
    // Caller should handle launching games; menu will be shown below and selection returned
    // Load menu to check selection
    DynamicJsonDocument menuDoc(4096);
    if (!loadJsonFromSD(MENU_PATH, menuDoc)) continue;

    // Detect apps in /apps and append them to the menu
    if (SD.exists("/apps")) {
      File appsDir = SD.open("/apps");
      File entry = appsDir.openNextFile();
      while (entry) {
        if (entry.isDirectory()) {
          String id = String(entry.name());
          String manifest = String("/apps/") + id + "/manifest.json";
          if (SD.exists(manifest.c_str())) {
            DynamicJsonDocument appDoc(1024);
            if (loadJsonFromSD(manifest.c_str(), appDoc)) {
              JsonArray arr = menuDoc["menu"].as<JsonArray>();
              JsonObject itm = arr.createNestedObject();
              itm["id"] = appDoc["id"].as<const char*>();
              itm["title"] = appDoc["name"].as<const char*>();
              itm["path"] = String("/apps/") + appDoc["id"].as<const char*>();
            }
          }
        }
        entry = appsDir.openNextFile();
      }
      appsDir.close();
    }

    JsonArray menu = menuDoc["menu"].as<JsonArray>();
    // Ask UI for selected path (UI returns the selected path)
    String selected = ui.mainMenu(MENU_PATH);
    if (selected.length() == 0) { delay(200); continue; }

    // Resolve apps -> target path
    String targetPath = selected;
    if (selected.startsWith("/apps/")) {
      String manifest = selected + "/manifest.json";
      DynamicJsonDocument appDoc(1024);
      if (loadJsonFromSD(manifest.c_str(), appDoc) && appDoc.containsKey("path")) {
        targetPath = String(appDoc["path"].as<const char*>());
      }
    }

    // Basic launcher: if targetPath references pacman, launch the PacmanGame
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0);
    tft.println("Loading...");
    if (targetPath.indexOf("pacman") >= 0 && SD.exists("/games/pacman/manifest.json")) {
      PacmanGame game(&tft, &mcp, &dfplayer);
      game.loadFromSD("/games/pacman");
      // play start track 002 on DFPlayer if available
      if (dfplayer.available()) dfplayer.play(2);
      game.run();
    }
    // After game ends, continue loop and show menu again
    delay(200);
  }
}

void loop() {
  // Not used; the main menu loop blocks and runs the game.
}
