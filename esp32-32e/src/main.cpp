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
const int cellSize = 16;
const int mapWidth = 30;
const int mapHeight = 20;
int pacmanX = 1, pacmanY = 1, dirX = 0, dirY = 0;
int gameMap[mapHeight][mapWidth] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,1,2,2,2,2,2,2,2,1,1,1,1,2,2,2,1,2,2,2,2,2,2,2},
    {1,2,1,1,1,2,2,1,2,2,1,1,1,2,2,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,2,1,2,2,2,1,2,2,2,2,2,2,2,2,2,2},
    {1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2},
    {1,2,2,2,2,2,2,1,2,2,2,2,2,2,2,1,1,1,1,1,2,2,2,2,1,2,2,1,2,2},
    {1,2,1,1,1,2,2,1,2,2,1,1,1,2,2,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,1,2,2,2,2,2,1,2,2,2,2,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1,1,1},
};  

// Game state
bool gameStarted = false;

void drawGame() {
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
    //if (digitalRead(32) == LOW) { dirX = -1; dirY = 0; } // Left
    if (digitalRead(35) == LOW) { dirX = 1; dirY = 0; } // Right (use IO35 instead of IO33 which is TOUCH_CS)
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


    Serial.println("TFT is initialized");
    tft.init(); // Initialize with ST7796 driver
    tft.setRotation(1);
    // Quick color test to verify display commands reach the panel
    tft.fillScreen(TFT_RED);
    delay(300);
    tft.fillScreen(TFT_GREEN);
    delay(300);
    tft.fillScreen(TFT_BLUE);
    delay(300);

    // Diagnostic prints
    Serial.print("TFT width="); Serial.print(tft.width());
    Serial.print(" height="); Serial.println(tft.height());

    // Print the pin macros from User_Setup.h for verification
    Serial.print("TFT pins MOSI="); Serial.print(TFT_MOSI);
    Serial.print(" SCLK="); Serial.print(TFT_SCLK);
    Serial.print(" MISO="); Serial.print(TFT_MISO);
    Serial.print(" CS="); Serial.print(TFT_CS);
    Serial.print(" DC="); Serial.print(TFT_DC);
    Serial.print(" BL="); Serial.println(TFT_BL);

    // Draw a center pixel and a small rect to check drawing commands
    int cx = tft.width() / 2;
    int cy = tft.height() / 2;
    Serial.print("Drawing center pixel at "); Serial.print(cx); Serial.print(","); Serial.println(cy);
    tft.drawPixel(cx, cy, TFT_WHITE);
    delay(200);
    tft.fillRect(cx - 30, cy - 20, 60, 40, TFT_YELLOW);
    delay(500);

    // Toggle backlight to verify polarity (GPIO27 used as BL on this board)
    pinMode(27, OUTPUT);
    Serial.println("Toggling BL (GPIO27) to test backlight polarity");
    digitalWrite(27, LOW);
    delay(200);
    digitalWrite(27, HIGH);
    delay(200);
    digitalWrite(27, LOW);
    delay(200);
    digitalWrite(27, HIGH);
    delay(200);
    digitalWrite(27, LOW);
    delay(200);
    
    // Draw a small test rectangle to check drawing commands
    tft.fillRect(10, 10, 100, 50, TFT_YELLOW);
    delay(1000);

    showStartScreen();

    // Touch diagnostics: check IRQ pin and poll getTouch()
    pinMode(TOUCH_IRQ, INPUT_PULLUP);
    Serial.print("Touch IRQ initial state (LOW when touched): ");
    Serial.println(digitalRead(TOUCH_IRQ));
    Serial.println("Touch the screen now — polling for getTouch() for 5 seconds...");
    unsigned long st = millis();
    while (millis() - st < 5000) {
        uint16_t tx = 0, ty = 0;
        // Print IRQ pin state for visibility
        Serial.print("IRQ="); Serial.print(digitalRead(TOUCH_IRQ)); Serial.print(" ");
        if (tft.getTouch(&tx, &ty)) {
            Serial.print("getTouch TRUE: "); Serial.print(tx); Serial.print(","); Serial.println(ty);
            break;
        }
        delay(200);
    }
    delay(2000);

    // Setup PullUps
    pinMode(32, INPUT_PULLUP);
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);

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
        digitalWrite(27, HIGH);

        // getTouch returns true when screen is touched (depends on config)
        if (tft.getTouch(&tx, &ty)) {
            Serial.print("Touch detected at "); Serial.print(tx); Serial.print(", "); Serial.println(ty);
            touched = true;}
        // Also allow starting via any direction button
        //if (digitalRead(32) == LOW || digitalRead(33) == LOW || digitalRead(25) == LOW || digitalRead(26) == LOW) touched = true;
        
        
        Serial.println("Waiting for touch to start the game...");
        if (touched) {
            gameStarted = true;
            tft.fillScreen(TFT_BLACK);
            drawGame();
            delay(200);
        } else {
            Serial.println("No touch detected, still waiting...");
            delay(50);
            tft.fillScreen(TFT_BLUE);
            return; 
            }
    }

    handleInput();
    updateGame();
    drawGame();
    delay(100);
   // audio.loop(); // Keep audio running
}