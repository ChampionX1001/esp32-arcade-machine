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
const int pacSpriteSize = FRAME_W; // use fram  e width (32)
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

// Movement & timing
unsigned long gameStartMillis = 0;
bool movementEnabled = false; // becomes true 10s after game start
unsigned long lastPacMove = 0;
unsigned long lastGhostMove = 0;
const unsigned long pacMoveInterval = 200; // ms per pacman step
unsigned long ghostMoveInterval = (unsigned long)(pacMoveInterval / 0.75 + 0.5); // ghosts are 0.75x pacman speed

// Ghost positions (grid cells)
int ghostX[MAX_GHOSTS];
int ghostY[MAX_GHOSTS];



// Draw a ghost at grid cell (gx,gy) with color index g
void drawGhostAt(int g, int gx, int gy) {
    const uint16_t fallbackColor[MAX_GHOSTS] = { TFT_WHITE, TFT_RED, TFT_GREEN, TFT_BLUE };
    int sx = gx * cellSize + 2; // small inset
    int sy = gy * cellSize + pelletDiameter; // moved up 2px earlier
    int radius = (cellSize - 4) / 2;
    int cx = sx + radius;
    int cy = sy + radius;
    tft.fillCircle(cx, cy, radius, fallbackColor[g]);
    // eyes
    tft.fillCircle(cx - radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
    tft.fillCircle(cx + radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
}

// Axis-aligned bounding box overlap test (inclusive)
bool aabbOverlap(int aLeft, int aTop, int aRight, int aBottom,
                 int bLeft, int bTop, int bRight, int bBottom) {
    return !(aRight < bLeft || aLeft > bRight || aBottom < bTop || aTop > bBottom);
}

// Stop the game entirely after a collision
void stopGameDueToCollision() {
    Serial.println("Fatal collision: stopping game.");
    movementEnabled = false;
    // Block the loop to stop all processing — user requested the game to stop completely
    while (true) {
        delay(1000);
    }
}

// Render the four ghosts using their current positions
void renderGhostsInCenter() {
    for (int g = 0; g < MAX_GHOSTS; ++g) {
        drawGhostAt(g, ghostX[g], ghostY[g]);
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

// Helper: redraw a single cell based on the map (used to restore tiles when sprites move)
void redrawCell(int gx, int gy) {
    int px = gx * cellSize;
    int py = gy * cellSize;
    int cell = gameMap[gy][gx];
    if (cell == 1) {
        tft.fillRect(px, py, cellSize, cellSize, TFT_BLUE);
    } else if (cell == 2) {
        tft.fillRect(px, py, cellSize, cellSize, TFT_BLACK);
        tft.fillCircle(px + cellSize / 2, py + cellSize / 2 + pelletYOffset, 2, TFT_YELLOW);
    } else {
        tft.fillRect(px, py, cellSize, cellSize, TFT_BLACK);
    }
}

// Compute distances from Pacman and move each ghost one step along the shortest path
void moveGhostsStep() {
    // BFS from Pacman to all reachable cells
    static int dist[mapHeight][mapWidth];
    for (int y = 0; y < mapHeight; ++y) for (int x = 0; x < mapWidth; ++x) dist[y][x] = -1;

    int qx[mapWidth * mapHeight];
    int qy[mapWidth * mapHeight];
    int head = 0, tail = 0;
    dist[pacmanY][pacmanX] = 0;
    qx[tail] = pacmanX; qy[tail] = pacmanY; tail++;

    while (head < tail) {
        int x = qx[head]; int y = qy[head]; head++;
        const int dx[4] = {1, -1, 0, 0};
        const int dy[4] = {0, 0, 1, -1};
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d]; int ny = y + dy[d];
            if (nx < 0 || nx >= mapWidth || ny < 0 || ny >= mapHeight) continue;
            if (gameMap[ny][nx] == 1) continue; // wall
            if (dist[ny][nx] != -1) continue;
            dist[ny][nx] = dist[y][x] + 1;
            qx[tail] = nx; qy[tail] = ny; tail++;
        }
    }

    // Occupancy map to avoid ghosts stepping on each other
    static bool occ[mapHeight][mapWidth];
    for (int y = 0; y < mapHeight; ++y) for (int x = 0; x < mapWidth; ++x) occ[y][x] = false;
    for (int i = 0; i < MAX_GHOSTS; ++i) {
        if (ghostX[i] >= 0 && ghostY[i] >= 0 && ghostX[i] < mapWidth && ghostY[i] < mapHeight)
            occ[ghostY[i]][ghostX[i]] = true;
    }

    // Move each ghost one step towards Pacman (if reachable)
    for (int g = 0; g < MAX_GHOSTS; ++g) {
        int gx = ghostX[g]; int gy = ghostY[g];
        if (gx < 0 || gy < 0 || gx >= mapWidth || gy >= mapHeight) continue;
        if (dist[gy][gx] == -1) continue; // unreachable
        if (dist[gy][gx] == 0) continue; // already at Pacman's cell

        // free current cell in occupancy map (we'll reserve the new cell soon)
        occ[gy][gx] = false;

        // find neighbor with distance one less and not occupied
        int bestNx = gx, bestNy = gy;
        const int nxDelta[4] = {1, -1, 0, 0};
        const int nyDelta[4] = {0, 0, 1, -1};
        for (int d = 0; d < 4; ++d) {
            int nx = gx + nxDelta[d]; int ny = gy + nyDelta[d];
            if (nx < 0 || nx >= mapWidth || ny < 0 || ny >= mapHeight) continue;
            if (dist[ny][nx] != -1 && dist[ny][nx] < dist[gy][gx] && !occ[ny][nx]) { bestNx = nx; bestNy = ny; break; }
        }

        // reserve new cell in occupancy map
        occ[bestNy][bestNx] = true;

        // Erase old ghost position by redrawing underlying cell
        redrawCell(gx, gy);
        // Update position
        Serial.printf("ghost %d chase step: (%d,%d) -> (%d,%d)\n", g, gx, gy, bestNx, bestNy);
        ghostX[g] = bestNx;
        ghostY[g] = bestNy;
        // Draw ghost at new position
        drawGhostAt(g, ghostX[g], ghostY[g]);

        // Check collision with Pacman using AABB
        int pCx = pacmanX * cellSize + cellSize/2;
        int pCy = pacmanY * cellSize + cellSize/2 + pelletDiameter;
        int pR = pacmanRadius;
        int pLeft = pCx - pR; int pRight = pCx + pR;
        int pTop = pCy - pR; int pBottom = pCy + pR;

        int gsx = ghostX[g] * cellSize + 2;
        int gsy = ghostY[g] * cellSize + pelletDiameter;
        int gR = (cellSize - 4) / 2;
        int gLeft = gsx; int gRight = gsx + 2 * gR;
        int gTop = gsy; int gBottom = gsy + 2 * gR;

        if (aabbOverlap(pLeft, pTop, pRight, pBottom, gLeft, gTop, gRight, gBottom)) {
            Serial.printf("Collision: ghost %d at (%d,%d) collided with Pacman\n", g, ghostX[g], ghostY[g]);
            stopGameDueToCollision();
            return; // abort remaining ghost moves
        }
    }
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
                redrawCell(xx, yy);
            }
        }
    }


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

        // Axis-aligned bounding boxes for Pacman (approximate using circle radius)
        int pCx = pacmanX * cellSize + cellSize/2;
        int pCy = pacmanY * cellSize + cellSize/2 + pelletDiameter;
        int pR = pacmanRadius;
        int pLeft = pCx - pR; int pRight = pCx + pR;
        int pTop = pCy - pR; int pBottom = pCy + pR;

        // Check collision against all ghosts
        for (int g = 0; g < MAX_GHOSTS; ++g) {
            int gsx = ghostX[g] * cellSize + 2;
            int gsy = ghostY[g] * cellSize + pelletDiameter;
            int gR = (cellSize - 4) / 2;
            int gLeft = gsx; int gRight = gsx + 2 * gR;
            int gTop = gsy; int gBottom = gsy + 2 * gR;
            if (aabbOverlap(pLeft, pTop, pRight, pBottom, gLeft, gTop, gRight, gBottom)) {
                Serial.printf("Collision: Pacman at (%d,%d) collided with ghost %d\n", pacmanX, pacmanY, g);
                stopGameDueToCollision();
                return;
            }
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
            // Initialize ghosts one-by-one across the middle columns on row 7
            int targetCenter = mapWidth / 2;
            int spawnCols[4] = { targetCenter - 2, targetCenter - 1, targetCenter, targetCenter + 1 };
            for (int g = 0; g < MAX_GHOSTS; ++g) {
                ghostX[g] = spawnCols[g];
                ghostY[g] = 7; // row 7
                Serial.printf("ghost %d spawned at (%d,%d)\n", g, ghostX[g], ghostY[g]);
            }

            // Draw full static map once, then ghosts and Pacman
            drawMapOnce();
            for (int g = 0; g < MAX_GHOSTS; ++g) drawGhostAt(g, ghostX[g], ghostY[g]);
            prevPacX = -1; // signal drawGame() to perform first-draw path
            drawGame();

            // Start delay timer: movement will begin after 10 seconds
            gameStartMillis = millis();
            movementEnabled = false;
            lastPacMove = gameStartMillis; // prevent Pacman from moving during the 10s pre-move phase
            lastGhostMove = gameStartMillis; // maintain ghost timing
            Serial.println("Game started: movement will begin in 10 seconds...");

            delay(200);
        } else {
            Serial.println("No touch detected, still waiting...");
            delay(50);
            tft.fillScreen(TFT_BLUE);
            return; 
            }
    }

    handleInput();

    unsigned long now = millis();

    // If the game has started but movement hasn't been enabled yet, check the 10s delay
    if (gameStarted && !movementEnabled) {
        if (now - gameStartMillis >= 10000UL) {
            movementEnabled = true;
            lastPacMove = now;
            lastGhostMove = now;
            Serial.println("Movement enabled — Pacman and ghosts will now start moving");
        }
    }

    // Move pacman at its interval (only after the 10s delay)
    if (movementEnabled) {
        if (now - lastPacMove >= pacMoveInterval) {
            updateGame();
            lastPacMove = now;
        }
    }

    // Move ghosts at their (slower) interval (only chase after movementEnabled)
    if (now - lastGhostMove >= ghostMoveInterval) {
        if (movementEnabled) {
            Serial.println("moveGhostsStep() called (chase mode)");
            moveGhostsStep();
        }
        lastGhostMove = now;
    }

    drawGame();
    delay(50);
   // audio.loop(); // Keep audio running
}