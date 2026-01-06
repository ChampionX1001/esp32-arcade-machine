// main.cpp for ESP32 Pacman (Arduino framework)

#include <TFT_eSPI.h>
#include <SPI.h>
#include <Audio.h> // ESP32-audioI2S
#include <SD.h>
#include <FS.h>
#include <LittleFS.h>
#include <PNGdec.h>

// Pin definitions (adjust for your board)
#define SD_CS 5

TFT_eSPI tft = TFT_eSPI();
Audio audio;
PNG png;
#define MAX_FRAMES 14
#define FRAME_W 32
#define FRAME_H 32
#define PACMAN_FRAME_COUNT 6

// Transparent color sentinel used when decoding PNGs (magenta in RGB565)
const uint16_t TRANSPARENT_COLOR = 0xF81F;

uint16_t* pacFrames[MAX_FRAMES] = { nullptr };
int currentFrameIdx = 0;

// Ghosts (four colors) loaded from LittleFS (explicit filenames)
#define MAX_GHOSTS 4
uint16_t* ghostBuffers[MAX_GHOSTS] = { nullptr };
int ghostW[MAX_GHOSTS] = {0};
int ghostH[MAX_GHOSTS] = {0};
// User-specified filenames (case preserved) that should be placed into the LittleFS data folder
const char* ghostFileNames[MAX_GHOSTS] = { "BlueGhost.png", "RedGhost.png", "WhiteGhost.png", "GreenGhost.png"};

// Temporary decode helpers used by the PNG callback
#define MAX_PNG_LINE 320
static uint16_t pngLineBuf[MAX_PNG_LINE];
static uint16_t* pngDecodeDest = nullptr;
static int pngDecodeDestW = 0;

// Pacman game map
const int cellSize = 16;
const int mapWidth = 30;
const int mapHeight = 20;

// shift pellets down a bit inside each cell (pixels)
const int pelletYOffset = -2;
const int pelletRadius = 2;
const int pelletDiameter = pelletRadius * 2;

// Pacman rendering radius (smaller than half-cell)
const int pacmanRadius = 5;
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

// LittleFS Wrapper Functions for PNGdec
File pngFile;
void * myOpen(const char *f, int32_t *s) { 
    pngFile = LittleFS.open(f, "r"); 
    if (!pngFile) return NULL;
    *s = pngFile.size(); 
    return &pngFile; 
}

void myClose(void *h) { if (pngFile) pngFile.close(); }
int32_t myRead(PNGFILE *p, uint8_t *b, int32_t l) { return pngFile.read(b, l); }
int32_t mySeek(PNGFILE *p, int32_t pos) { return pngFile.seek(pos); }

// Callback: Slices the horizontal sheet into 14 RAM buffers
// Change "void" to "int"
int pngSliceCallback(PNGDRAW *pDraw) {
    uint16_t lineBuffer[FRAME_W * PACMAN_FRAME_COUNT];
    // Use TRANSPARENT_COLOR for alpha pixels
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_LITTLE_ENDIAN, TRANSPARENT_COLOR);

    // Copy only the first PACMAN_FRAME_COUNT frames into pacFrames[0..PACMAN_FRAME_COUNT-1]
    for (int i = 0; i < PACMAN_FRAME_COUNT; i++) {
        memcpy(pacFrames[i] + (pDraw->y * FRAME_W), &lineBuffer[i * FRAME_W], FRAME_W * 2);
    }
    return 1; // Return 1 to continue decoding the next line
}

// Generic PNG callback that writes decoded RGB565 rows into the currently-selected destination buffer
int pngGhostCallback(PNGDRAW *pDraw) {
    if (!pngDecodeDest) return 0;
    if (pDraw->iWidth > MAX_PNG_LINE) return 0; // too wide
    // Use TRANSPARENT_COLOR for alpha pixels (little-endian so values match CPU order)
    png.getLineAsRGB565(pDraw, pngLineBuf, PNG_RGB565_LITTLE_ENDIAN, TRANSPARENT_COLOR);
    // Note: this PNGdec version exposes 'y' and 'iWidth' but not 'x', so assume x==0
    int sx = 0;
    int sy = pDraw->y;
    for (int i = 0; i < pDraw->iWidth; ++i) {
        pngDecodeDest[sy * pngDecodeDestW + sx + i] = pngLineBuf[i];
    }
    return 1;
}

// Decode a PNG at 'path' into an allocated RGB565 buffer (outBuf). Returns true on success.
bool decodePNGToBuffer(const char* path, uint16_t** outBuf, int &outW, int &outH) {
    if (!LittleFS.begin()) { Serial.println("LittleFS mount failed in decodePNGToBuffer"); return false; }
    if (!LittleFS.exists(path)) { Serial.printf("PNG not found: %s\n", path); return false; }
    if (png.open(path, myOpen, myClose, myRead, mySeek, pngGhostCallback) != PNG_SUCCESS) { Serial.printf("PNG open failed: %s\n", path); return false; }

    outW = png.getWidth();
    outH = png.getHeight();
    // allocate buffer and zero it
    *outBuf = (uint16_t*)malloc(outW * outH * 2);
    if (!*outBuf) {
        Serial.println("Out of memory allocating PNG buffer");
        png.close();
        return false;
    }
    // initialize to transparent sentinel
    for (int k = 0; k < outW * outH; ++k) (*outBuf)[k] = TRANSPARENT_COLOR;

    // Set global decode target for the callback
    pngDecodeDest = *outBuf;
    pngDecodeDestW = outW;

    int ret = png.decode(NULL, 0);
    if (ret != PNG_SUCCESS) {
        Serial.printf("PNG decode failed (%d) for %s\n", ret, path);
        free(*outBuf); *outBuf = nullptr; pngDecodeDest = nullptr; pngDecodeDestW = 0; png.close(); return false;
    }

    // reset decode target and close
    pngDecodeDest = nullptr; pngDecodeDestW = 0; png.close();
    Serial.printf("Decoded PNG %s -> %dx%d\n", path, outW, outH);
    return true;
}

// Draw a decoded frame buffer to the TFT honoring TRANSPARENT_COLOR
void drawFrameWithTransparency(uint16_t* buf, int fw, int fh, int sx, int sy) {
    if (!buf) return;
    for (int y = 0; y < fh; ++y) {
        int ty = sy + y;
        if (ty < 0 || ty >= tft.height()) continue;
        for (int x = 0; x < fw; ++x) {
            int tx = sx + x;
            if (tx < 0 || tx >= tft.width()) continue;
            uint16_t c = buf[y * fw + x];
            if (c == TRANSPARENT_COLOR) continue;
            tft.drawPixel(tx, ty, c);
        }
    }
}

void loadSpritesheet(const char* path) {
   // Attempt to mount LittleFS; try formatting if mount fails
   if (!LittleFS.begin()) {
      Serial.println("LittleFS mount failed — attempting format...");
      if (LittleFS.format()) {
        Serial.println("LittleFS format succeeded, retrying mount...");
        if (LittleFS.begin()) Serial.println("LittleFS mounted after format");
        else Serial.println("Mount still failed after format");
      } else {
        Serial.println("LittleFS format failed");
        return;
      }
   }

    // Allocate RAM for frames and zero them to avoid showing uninitialized data
    for (int i = 0; i < MAX_FRAMES; i++) {
        pacFrames[i] = (uint16_t*)malloc(FRAME_W * FRAME_H * 2);
        if (pacFrames[i]) {
            // initialize to transparent sentinel
            for (int k = 0; k < FRAME_W * FRAME_H; ++k) pacFrames[i][k] = TRANSPARENT_COLOR;
        } else Serial.printf("Failed allocating frame %d\n", i);
    }

    if (png.open(path, myOpen, myClose, myRead, mySeek, pngSliceCallback) == PNG_SUCCESS) {
        int sheetW = png.getWidth();
        int sheetH = png.getHeight();
        if (sheetW < FRAME_W * MAX_FRAMES || sheetH < FRAME_H) {
            Serial.printf("Spritesheet unexpected size: %dx%d (need >= %dx%d)\n", sheetW, sheetH, FRAME_W * MAX_FRAMES, FRAME_H);
            png.close();
            for (int i = 0; i < MAX_FRAMES; i++) { free(pacFrames[i]); pacFrames[i] = nullptr; }
            return;
        }

        int ret = png.decode(NULL, 0);
        if (ret != PNG_SUCCESS) {
            Serial.printf("PNG decode failed: %d\n", ret);
            png.close();
            for (int i = 0; i < MAX_FRAMES; i++) { free(pacFrames[i]); pacFrames[i] = nullptr; }
            return;
        }

        png.close();
        Serial.println("Spritesheet loaded from Flash to RAM");
        return;
    } else {
        Serial.println("PNG open failed");
        for (int i = 0; i < MAX_FRAMES; i++) { free(pacFrames[i]); pacFrames[i] = nullptr; }
        return;
    }
}

// Try to locate and load the four ghost PNGs (white, red, green, blue)
void loadGhosts() {
    // Ensure LittleFS is mounted before checking existence
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed in loadGhosts — attempting format...");
        if (LittleFS.format()) {
            Serial.println("LittleFS format succeeded, retrying mount...");
            if (!LittleFS.begin()) { Serial.println("LittleFS mount still failed in loadGhosts"); return; }
        } else {
            Serial.println("LittleFS format failed in loadGhosts"); return;
        }
    }

    const char* tryPatterns[] = {"/%s", "/assets/%s", "/%s", "/assets/%s" };
    for (int g = 0; g < MAX_GHOSTS; ++g) {
        char tryPath[96];
        bool found = false;
        // Try exact filename first, then lowercased variant, and in /assets/
        for (auto &pat : tryPatterns) {
            snprintf(tryPath, sizeof(tryPath), pat, ghostFileNames[g]);
            if (LittleFS.exists(tryPath)) { found = true; break; }
            // lowercased name
            char lower[64]; strncpy(lower, ghostFileNames[g], sizeof(lower)-1); lower[sizeof(lower)-1]=0;
            for (char *p = lower; *p; ++p) *p = tolower(*p);
            snprintf(tryPath, sizeof(tryPath), pat, lower);
            if (LittleFS.exists(tryPath)) { found = true; break; }
        }
        if (!found) {
            Serial.printf("Ghost PNG not found for %s\n", ghostFileNames[g]);
            ghostBuffers[g] = nullptr; ghostW[g] = ghostH[g] = 0;
            continue;
        }
        Serial.printf("Loading ghost PNG %s\n", tryPath);
        int w = 0, h = 0;
        if (decodePNGToBuffer(tryPath, &ghostBuffers[g], w, h)) {
            ghostW[g] = w; ghostH[g] = h;
            Serial.printf("Loaded ghost %s -> %dx%d\n", ghostFileNames[g], w, h);
        } else {
            Serial.printf("Failed to decode ghost %s\n", ghostFileNames[g]);
            ghostBuffers[g] = nullptr; ghostW[g] = ghostH[g] = 0;
        }
    }
}

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

        if (ghostBuffers[g]) {
            int w = ghostW[g]; int h = ghostH[g];
            int d_sx = gx * cellSize + (cellSize - w) / 2;
            int d_sy = gy * cellSize + (cellSize - h) / 2 + pelletDiameter;
            if (d_sx < 0) d_sx = 0;
            if (d_sy < 0) d_sy = 0;
            drawFrameWithTransparency(ghostBuffers[g], w, h, d_sx, d_sy);
        } else {
            // Fallback: draw a simple circular ghost-shaped blob
            Serial.printf("Drawing fallback ghost for %s\n", ghostFileNames[g]);
            int radius = (cellSize - 4) / 2;
            int cx = sx + radius;
            int cy = sy + radius;
            tft.fillCircle(cx, cy, radius, fallbackColor[g]);
            // eyes
            tft.fillCircle(cx - radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
            tft.fillCircle(cx + radius/3, cy - radius/3, max(1, radius/6), TFT_BLACK);
        }
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
        // Draw Pacman sprite for the first frame (use frame buffer if loaded)
        int sx = pacmanX * cellSize + (cellSize - FRAME_W) / 2;
        int sy = pacmanY * cellSize + (cellSize - FRAME_H) / 2 + pelletDiameter;
        pacSprite.fillSprite(TFT_BLACK);
        if (pacFrames[0]) {
            pacSprite.pushImage(0, 0, FRAME_W, FRAME_H, pacFrames[0]);
        } else {
            // fallback to procedural large pacman
            int r = min(FRAME_W, FRAME_H) / 2 - 2;
            pacSprite.fillCircle(FRAME_W/2, FRAME_H/2, r, TFT_YELLOW);
            pacSprite.fillCircle(FRAME_W/2 + 4, FRAME_H/2 - 6, 2, TFT_BLACK);
        }
        pacSprite.pushSprite(sx, sy);
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
    // Update animation frame index
    pacAnimCounter++;
    if (pacAnimCounter >= pacAnimThreshold) {
        currentFrameIdx = (currentFrameIdx + 1) % PACMAN_FRAME_COUNT;
        pacAnimCounter = 0;
    }

    // Position Pacman centered in cell (shift down by pellet diameter)
    int sx = pacmanX * cellSize + (cellSize - FRAME_W) / 2;
    int sy = pacmanY * cellSize + (cellSize - FRAME_H) / 2 + pelletDiameter;

    // Render current frame (or fallback) to the display, honoring transparency
    if (pacFrames[currentFrameIdx]) {
        drawFrameWithTransparency(pacFrames[currentFrameIdx], FRAME_W, FRAME_H, sx, sy);
    } else {
        // fallback procedural: draw into sprite and push
        pacSprite.fillSprite(TFT_BLACK);
        int r = min(FRAME_W, FRAME_H) / 2 - 2;
        pacSprite.fillCircle(FRAME_W/2, FRAME_H/2, r, TFT_YELLOW);
        pacSprite.fillCircle(FRAME_W/2 + 4, FRAME_H/2 - 6, 2, TFT_BLACK);
        pacSprite.pushSprite(sx, sy);
    }

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
 // Load the sheet from assets directory in Flash
    loadSpritesheet("/assets/PacMan_Spritesheet.png"); 
    
    // Create the rendering sprite
    pacSprite.createSprite(FRAME_W, FRAME_H);

    // Load ghosts from LittleFS (white, red, green, blue)
    loadGhosts();

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