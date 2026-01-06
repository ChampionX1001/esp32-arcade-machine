// main.cpp for ESP32 Pacman (Arduino framework)

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Audio.h> // ESP32-audioI2S
#include <SD.h>
#include <FS.h>
#include <LittleFS.h>


// Pin definitions (adjust for your board)
#define SD_CS 5

TFT_eSPI tft = TFT_eSPI();
Audio audio;

#define FRAME_W 32
#define FRAME_H 32

// Pacman will be drawn procedurally as a yellow circle (no PNGs)

#define MAX_GHOSTS 4
const uint16_t fallbackColor[MAX_GHOSTS] = { TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE };

// Pacman game map
const int cellSize = 16;
const int mapWidth = 30;
const int mapHeight = 20;

// shift pellets down a bit inside each cell (pixels)
const int pelletYOffset = -2;
const int pelletRadius = 2;
const int pelletDiameter = pelletRadius * 2;

// Pacman rendering radius (smaller than half-cell)
const int pacmanRadius = 16;
int pacmanX = 1, pacmanY = 1, dirX = 0, dirY = 0;
int prevPacX = -1, prevPacY = -1; // previous Pacman position (for selective redraw)

// Sprite animation state
TFT_eSprite pacSprite = TFT_eSprite(&tft);
const int pacSpriteSize = FRAME_W; // use frame width (32)
int pacAnimCounter = 0;
const int pacAnimThreshold = 3; // lower = faster animation
bool pacMouthOpen = true;
int lastDirX = 1, lastDirY = 0; // last non-zero direction (defaults right)
int gameMap[mapHeight][mapWidth] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,2,1,1,2,2,1,1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,2,2,1,1,2,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,2,1,1,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,1,1,2,2,1},
    {1,2,2,1,1,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,1,1,2,2,1},
    {1,1,1,1,1,2,2,1,2,2,1,1,1,1,1,1,1,1,1,1,2,2,1,2,2,1,1,1,1,1},
    {2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2}, // row 9 (middle)
    {2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2}, // row 10 (middle)
    {1,1,1,1,1,2,2,1,2,2,1,1,1,1,1,1,1,1,1,1,2,2,1,2,2,1,1,1,1,1},
    {1,2,2,1,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,1,2,2,1},
    {1,2,2,1,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,1,2,2,1},
    {1,2,2,2,2,2,2,1,2,2,1,1,1,1,1,1,1,1,1,1,2,2,1,2,2,2,2,2,2,1},
    {1,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,1},
    {1,2,2,1,1,2,2,1,2,2,1,2,2,2,2,2,2,2,2,1,2,2,1,2,2,1,1,2,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

// Game state
bool gameStarted = false;

// PNG file wrapper removed (no PNGs used)

// PNG decoding callbacks removed (no PNGs used)

// PNG decoding helper removed (no PNGs used)

// Draw a decoded frame buffer to the TFT honoring TRANSPARENT_COLOR
// PNG decoding and spritesheet loading removed — Pacman and ghosts are drawn procedurally (no PNGs).

// Render the four ghosts in the central 2x2 box of the map
void renderGhostsInCenter() {
    int baseX = (mapWidth / 2) - 1; // left column of central 2x2
    int baseY = (mapHeight / 2) - 1; // top row of central 2x2
    const uint16_t fallbackColor[MAX_GHOSTS] = { TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE };

    for (int g = 0; g < MAX_GHOSTS; ++g) {
        int gx = baseX + (g % 2);
        int gy = baseY + (g / 2);
        int sx = gx * cellSize + 2; // small inset
        int sy = gy * cellSize + 2 + pelletDiameter;

        // Draw a simple circular ghost-shaped blob (no PNGs)
        int radius = (cellSize - 4) / 2;
        int cx = sx + radius;
        int cy = sy + radius;
        tft.fillCircle(cx, cy, radius, fallbackColor[g]);
        // eyes
        tft.fillCircle(cx - radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
        tft.fillCircle(cx + radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
    }
}

// Draw the static map once (walls and pellets)
void drawMapOnce() {
    tft.fillScreen(TFT_BLACK);
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int px = x * cellSize;
            int py = y * cellSize;
            if (gameMap[y][x] == 1) {
                tft.fillRect(px, py, cellSize, cellSize, TFT_BLUE);
            }
            else if (gameMap[y][x] == 2) {
                tft.fillCircle(px + cellSize / 2, py + cellSize / 2 + pelletYOffset, 2, TFT_YELLOW);
            }
        }
    }

    // Draw the ghosts in the central box
    renderGhostsInCenter();
}

void drawGame() {
    // If this is the first draw after starting, draw the full map and place Pacman
    if (prevPacX == -1) {
        drawMapOnce();
        // Draw Pacman procedurally as a yellow circle (first frame)
        int cx = pacmanX * cellSize + cellSize/2;
        int cy = pacmanY * cellSize + cellSize/2 + pelletDiameter;
        int r = pacmanRadius;
        tft.fillCircle(cx, cy, r, TFT_YELLOW);
        // simple eye
        tft.fillCircle(cx + 3, cy - 4, 1, TFT_BLACK);
        prevPacX = pacmanX;
        prevPacY = pacmanY;
        return;
    }

    // Erase Pacman's previous position by clearing the full sprite bounding box
    if (prevPacX != pacmanX || prevPacY != pacmanY) {
        // bounding box of the previous sprite (centered on the cell)
        int prevSx = prevPacX * cellSize + (cellSize - FRAME_W) / 2;
        int prevSy = prevPacY * cellSize + (cellSize - FRAME_H) / 2 + pelletDiameter;

        // clamp to screen bounds
        int sx = prevSx; if (sx < 0) sx = 0;
        int sy = prevSy; if (sy < 0) sy = 0;
        int sw = FRAME_W; if (sx + sw > tft.width()) sw = tft.width() - sx;
        int sh = FRAME_H; if (sy + sh > tft.height()) sh = tft.height() - sy;
        if (sw > 0 && sh > 0) {
            tft.fillRect(sx, sy, sw, sh, TFT_BLACK);
        }

        // redraw any map cells intersecting that box
        int x0 = sx / cellSize; if (x0 < 0) x0 = 0;
        int y0 = sy / cellSize; if (y0 < 0) y0 = 0;
        int x1 = (sx + sw - 1) / cellSize; if (x1 >= mapWidth) x1 = mapWidth - 1;
        int y1 = (sy + sh - 1) / cellSize; if (y1 >= mapHeight) y1 = mapHeight - 1;

        for (int yy = y0; yy <= y1; ++yy) {
            for (int xx = x0; xx <= x1; ++xx) {
                int px = xx * cellSize;
                int py = yy * cellSize;
                int cell = gameMap[yy][xx];
                if (cell == 1) {
                    tft.fillRect(px, py, cellSize, cellSize, TFT_BLUE);
                } else if (cell == 2) {
                    tft.fillRect(px, py, cellSize, cellSize, TFT_BLACK);
                    tft.fillCircle(px + cellSize / 2, py + cellSize / 2 + pelletYOffset, 2, TFT_YELLOW);
                } else {
                    tft.fillRect(px, py, cellSize, cellSize, TFT_BLACK);
                }
            }
        }
    }

    // Draw Pacman at the new position using a sprite
    /*
    pacAnimCounter++;
    if (pacAnimCounter >= pacAnimThreshold) { pacMouthOpen = !pacMouthOpen; pacAnimCounter = 0; }
    int sx = pacmanX * cellSize + (cellSize - pacSpriteSize) / 2;
    int sy = pacmanY * cellSize + (cellSize - pacSpriteSize) / 2;
    pacSprite.fillSprite(TFT_BLACK);
    int r = pacmanRadius + (pacMouthOpen ? 1 : 0);
    pacSprite.fillCircle(pacSpriteSize/2, pacSpriteSize/2, r, TFT_YELLOW);
    // draw directional mouth when open
    if (pacMouthOpen) {
        int cx = pacSpriteSize/2;
        int cy = pacSpriteSize/2;
        int tri = r; // mouth size
        if (lastDirX > 0) { // right
            pacSprite.fillTriangle(cx + tri, cy, cx - tri/2, cy - tri/2, cx - tri/2, cy + tri/2, TFT_BLACK);
        } else if (lastDirX < 0) { // left
            pacSprite.fillTriangle(cx - tri, cy, cx + tri/2, cy - tri/2, cx + tri/2, cy + tri/2, TFT_BLACK);
        } else if (lastDirY < 0) { // up
            pacSprite.fillTriangle(cx, cy - tri, cx - tri/2, cy + tri/2, cx + tri/2, cy + tri/2, TFT_BLACK);
        } else if (lastDirY > 0) { // down
            pacSprite.fillTriangle(cx, cy + tri, cx - tri/2, cy - tri/2, cx + tri/2, cy - tri/2, TFT_BLACK);
        }
    }
    // eye
    pacSprite.fillCircle(pacSpriteSize/2 + 2, pacSpriteSize/2 - 3, 1, TFT_BLACK);
    pacSprite.pushSprite(sx, sy);

    // Remember current as previous for next iteration
    prevPacX = pacmanX;
    prevPacY = pacmanY;
    */
    // Update simple mouth animation state
    pacAnimCounter++;
    if (pacAnimCounter >= pacAnimThreshold) {
        pacMouthOpen = !pacMouthOpen;
        pacAnimCounter = 0;
    }

    // Position Pacman centered in cell (shift down by pellet diameter)
    int cx = pacmanX * cellSize + cellSize / 2;
    int cy = pacmanY * cellSize + cellSize / 2 + pelletDiameter;
    int r = pacmanRadius;

    // Draw the body
    tft.fillCircle(cx, cy, r, TFT_YELLOW);

    // Draw mouth (a simple triangle) when mouth is open
    if (pacMouthOpen) {
        if (lastDirX > 0) { // right
            tft.fillTriangle(cx, cy, cx + r, cy - r/2, cx + r, cy + r/2, TFT_BLACK);
        } else if (lastDirX < 0) { // left
            tft.fillTriangle(cx, cy, cx - r, cy - r/2, cx - r, cy + r/2, TFT_BLACK);
        } else if (lastDirY < 0) { // up
            tft.fillTriangle(cx, cy, cx - r/2, cy - r, cx + r/2, cy - r, TFT_BLACK);
        } else if (lastDirY > 0) { // down
            tft.fillTriangle(cx, cy, cx - r/2, cy + r, cx + r/2, cy + r, TFT_BLACK);
        }
    }

    // Eye (position depends on direction)
    int ex = cx + (lastDirX > 0 ? 3 : (lastDirX < 0 ? -3 : 2));
    int ey = cy - 4 + (lastDirY > 0 ? 1 : (lastDirY < 0 ? -1 : 0));
    tft.fillCircle(ex, ey, 1, TFT_BLACK);

    prevPacX = pacmanX;
    prevPacY = pacmanY;
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
        // Update last direction if movement occurs
        if (dirX != 0 || dirY != 0) { lastDirX = dirX; lastDirY = dirY; }
        pacmanX = newX;
        pacmanY = newY;
        if (gameMap[pacmanY][pacmanX] == 2) {
            gameMap[pacmanY][pacmanX] = 0;
            // Erase the pellet immediately so it disappears when Pacman eats it
            int px = pacmanX * cellSize;
            int py = pacmanY * cellSize;
            tft.fillRect(px, py, cellSize, cellSize, TFT_BLACK);
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

    // Pacman rendering sprite will be created as FRAME_W x FRAME_H after loading the sheet

    // Diagnostic prints
   // Serial.print("TFT width="); Serial.print(tft.width());
    //Serial.print(" height="); Serial.println(tft.height());

    // Print the pin macros from User_Setup.h for verification
   // Serial.print("TFT pins MOSI="); Serial.print(TFT_MOSI);
   // Serial.print(" SCLK="); Serial.print(TFT_SCLK);
    //Serial.print(" MISO="); Serial.print(TFT_MISO);
    //Serial.print(" CS="); Serial.print(TFT_CS);
   // Serial.print(" DC="); Serial.print(TFT_DC);
   // Serial.print(" BL="); Serial.println(TFT_BL);

    // Draw a center pixel and a small rect to check drawing commands
    //int cx = tft.width() / 2;
   // int cy = tft.height() / 2;
  //  Serial.print("Drawing center pixel at "); Serial.print(cx); Serial.print(","); Serial.println(cy);
  //  tft.drawPixel(cx, cy, TFT_WHITE);
  //  delay(200);
  //  tft.fillRect(cx - 30, cy - 20, 60, 40, TFT_YELLOW);
  //  delay(500);

    // Toggle backlight to verify polarity (GPIO27 used as BL on this board)
    pinMode(27, OUTPUT);
    Serial.println("Toggling BL (GPIO27) to test backlight polarity");
    digitalWrite(27, LOW);
    delay(200);
    digitalWrite(27, HIGH);
    delay(200);
    digitalWrite(27, LOW);
    //delay(200);
    //digitalWrite(27, HIGH);
    //delay(200);
   // digitalWrite(27, LOW);
   // delay(200);
    
    //Draw a small test rectangle to check drawing commands
    tft.fillRect(10, 10, 100, 50, TFT_YELLOW);
    delay(1000);
 // PNG functionality removed; use procedural sprites (no file loading)

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
            // Draw full static map once and then draw Pacman
            drawMapOnce();
            prevPacX = -1; // signal drawGame() to perform first-draw path
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