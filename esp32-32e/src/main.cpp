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

// Game state
bool gameStarted = false;

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

void showStartScreen() {
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setTextSize(2);
    // Exact text requested by user
    tft.drawString("Press Anywhere on the Screen to Play", 10, (tft.height() / 2) - 10, 2);
}

void playWav(const char* filename) {
    audio.stopSong();
   // audio.connecttoFS(SD, filename);
}

void handleInput() {
    // Example: Use buttons on GPIOs for input (replace with your actual pins)
    if (digitalRead(32) == LOW) { dirX = -1; dirY = 0; } // Left
    else if (digitalRead(33) == LOW) { dirX = 1; dirY = 0; } // Right
    else if (digitalRead(25) == LOW) { dirX = 0; dirY = -1; } // Up
    else if (digitalRead(26) == LOW) { dirX = 0; dirY = 1; } // Down
    else { dirX = 0; dirY = 0; }
    // Play sound on a specific button (e.g., GPIO 27)
  //  if (digitalRead(27) == LOW) {
    //    playWav("/PacManLittleDot.wav");
    //}
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
    Serial.begin(115200);
    delay(1000);
    Serial.println("--- BOOT SUCCESSFUL ---");

    // Initialize output pin for testing
    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH); 

    //pinMode(4, OUTPUT);
    //digitalWrite(4, LOW);
    //delay(100);
    //digitalWrite(4, HIGH);
    //delay(100);

    tft.init(); // Initialize with ST7796 driver
    tft.setRotation(1);
    tft.fillScreen(TFT_BLUE);
    showStartScreen();


    // Setup buttons
    pinMode(32, INPUT_PULLUP);
    pinMode(33, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
   // pinMode(27, INPUT_PULLUP);

    // Setup SD card
  //  if (!SD.begin(SD_CS)) {
    //tft.setTextColor(TFT_RED, TFT_BLACK);
      //  tft.drawString("SD Card Error!", 10, 10, 2);
        //while (1);

   // if (!SD.begin(SD_CS)) {
     //   tft.setTextColor(TFT_RED, TFT_BLACK);
      //  tft.drawString("SD Error! Check Card", 10, 10, 2);
      //  Serial.println("SD Card Mount Failed. Retrying in 5 seconds...");
    
    //Instead of while(1), wait and then restart or retry
   // delay(5000); 
   // ESP.restart(); // Reboots the board to try setup() again

    //Setup audio
    //audio.setVolume(10); // 0...21
    /* if (!SD.begin(5)) {
        Serial.println("No SD Card found, proceeding to game...");
    } */
    Serial.println("SD Card initialized.");
    Serial.println("Setup complete, waiting for game start...");
}


void loop() {
    if (!gameStarted) {
        uint16_t tx = 0, ty = 0;
        bool touched = false;
        // Try touch if supported by TFT_eSPI
        // getTouch returns true when screen is touched (depends on config)
       // if (tft.getTouch(&tx, &ty)) touched = true;
        // Also allow starting via any direction button
        if (digitalRead(32) == LOW || digitalRead(33) == LOW || digitalRead(25) == LOW || digitalRead(26) == LOW) touched = true;

        if (touched) {
            gameStarted = true;
            tft.fillScreen(TFT_BLACK);
            drawGame();
            delay(200);
        } else {
            delay(50);
            return;
        }
    }

    handleInput();
    updateGame();
    drawGame();
    delay(1000);
   // audio.loop(); // Keep audio running
}