// main.cpp for ESP32 Pacman (Arduino framework)

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Audio.h> // ESP32-audioI2S
#include <SD.h>
#include <FS.h>

// Pin definitions (adjust for your board)
#define SD_CS 5

TFT_eSPI tft = TFT_eSPI();
Audio audio;

// Pacman game map
const int cellSize = 20;
const int mapWidth = 16;
const int mapHeight = 9;
int pacmanX = 1, pacmanY = 1, dirX = 0, dirY = 0;
int gameMap[mapHeight][mapWidth] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,1,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,2,1,2,2,1,1,1,2,2,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,2,1},
    {1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,1},
    {1,2,2,2,2,2,2,1,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,2,1,2,2,1,1,1,2,2,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void drawGame() {
    tft.fillScreen(TFT_BLACK);
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int px = x * cellSize;
            int py = y * cellSize;
            if (gameMap[y][x] == 1) {
                tft.fillRect(px, py, cellSize, cellSize, TFT_BLUE);
            }
            else if (gameMap[y][x] == 2) {
                tft.fillCircle(px + cellSize / 2, py + cellSize / 2, 2, TFT_YELLOW);
            }
        }
    }
    // Draw Pacman
    tft.fillCircle(pacmanX * cellSize + cellSize / 2, pacmanY * cellSize + cellSize / 2, cellSize / 2, TFT_YELLOW);
}

void playWav(const char* filename) {
    audio.stopSong();
    audio.connecttoFS(SD, filename);
}

void handleInput() {
    // Example: Use buttons on GPIOs for input (replace with your actual pins)
    if (digitalRead(32) == LOW) { dirX = -1; dirY = 0; } // Left
    else if (digitalRead(33) == LOW) { dirX = 1; dirY = 0; } // Right
    else if (digitalRead(25) == LOW) { dirX = 0; dirY = -1; } // Up
    else if (digitalRead(26) == LOW) { dirX = 0; dirY = 1; } // Down
    else { dirX = 0; dirY = 0; }
    // Play sound on a specific button (e.g., GPIO 27)
    if (digitalRead(27) == LOW) {
        playWav("/PacManLittleDot.wav");
    }
}

void updateGame() {
    int newX = pacmanX + dirX;
    int newY = pacmanY + dirY;
    if (gameMap[newY][newX] != 1) {
        pacmanX = newX;
        pacmanY = newY;
        if (gameMap[pacmanY][pacmanX] == 2) {
            gameMap[pacmanY][pacmanX] = 0;
            playWav("/PacManLittleDot.wav");
        }
    }
}

void setup() {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Setup buttons
    pinMode(32, INPUT_PULLUP);
    pinMode(33, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);

    // Setup SD card
    //if (!SD.begin(SD_CS)) {
      //  tft.setTextColor(TFT_RED, TFT_BLACK);
       // tft.drawString("SD Card Error!", 10, 10, 2);
       // while (1);
    //}

    // Setup audio
    //audio.setVolume(10); // 0...21
}

void loop() {
    handleInput();
    updateGame();
    drawGame();
    delay(100);
    audio.loop(); // Keep audio running
}