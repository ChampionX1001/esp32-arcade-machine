#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "CoreUI.h"

namespace fs = std::filesystem;

// Desktop implementations
struct SDLScreen : Screen {
  SDL_Renderer *renderer;
  TTF_Font *font;
  int w,h;
  SDLScreen(SDL_Renderer *r, TTF_Font *f, int ww, int hh): renderer(r), font(f), w(ww), h(hh) {}
  int width() override { return w; }
  int height() override { return h; }
  void fillScreen(uint32_t color) override {
    uint8_t r = (color >> 16) & 0xFF; uint8_t g = (color >> 8) & 0xFF; uint8_t b = color & 0xFF;
    SDL_SetRenderDrawColor(renderer, r,g,b,255);
    SDL_RenderClear(renderer);
  }
  void fillRect(int x, int y, int w, int h, uint32_t color) override {
    uint8_t r = (color >> 16) & 0xFF; uint8_t g = (color >> 8) & 0xFF; uint8_t b = color & 0xFF;
    SDL_Rect rect{ x,y,w,h };
    SDL_SetRenderDrawColor(renderer, r,g,b,255);
    SDL_RenderFillRect(renderer, &rect);
  }
  void drawText(int x, int y, const std::string &text, int size, uint32_t color) override {
    SDL_Color c{ (uint8_t)((color>>16)&0xFF), (uint8_t)((color>>8)&0xFF), (uint8_t)(color&0xFF), 255 };
    TTF_Font *f = font;
    SDL_Surface *s = TTF_RenderUTF8_Blended(f, text.c_str(), c);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect dst{ x, y, s->w, s->h };
    SDL_FreeSurface(s);
    SDL_RenderCopy(renderer, t, NULL, &dst);
    SDL_DestroyTexture(t);
  }
};

struct SDLInput : Input {
  const Uint8 *keys;
  bool leftPressed = false, rightPressed=false, upPressed=false, downPressed=false;
  bool a=false,b=false,x=false,y=false;
  void poll() { SDL_PumpEvents(); keys = SDL_GetKeyboardState(NULL);
    leftPressed = keys[SDL_SCANCODE_LEFT]; rightPressed = keys[SDL_SCANCODE_RIGHT];
    upPressed = keys[SDL_SCANCODE_UP]; downPressed = keys[SDL_SCANCODE_DOWN];
    a = keys[SDL_SCANCODE_Z]; b = keys[SDL_SCANCODE_X]; x = keys[SDL_SCANCODE_A]; y = keys[SDL_SCANCODE_S]; }
  bool up() override { return upPressed; }
  bool down() override { return downPressed; }
  bool left() override { return leftPressed; }
  bool right() override { return rightPressed; }
  bool btnA() override { return a; }
  bool btnB() override { return b; }
  bool btnX() override { return x; }
  bool btnY() override { return y; }
};

struct DummySound : Sound { void playTrack(int id) override { std::cout << "Play track " << id << std::endl; } };

struct LocalFS : FileSystem {
  std::string root;
  LocalFS(const std::string &r): root(r) {}
  bool exists(const std::string &path) override { return fs::exists(root + path); }
  bool readAll(const std::string &path, std::string &out) override {
    std::string full = root + path;
    if (!fs::exists(full)) return false;
    std::ifstream ifs(full);
    out.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return true;
  }
};

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) { std::cerr << SDL_GetError(); return 1; }
  if (TTF_Init() != 0) { std::cerr << TTF_GetError(); return 1; }

  int W = 480, H = 320;
  SDL_Window *win = SDL_CreateWindow("Hosyond UI Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, 0);
  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
  if (!font) font = TTF_OpenFont("arial.ttf", 16);

  SDLScreen screen(ren, font, W, H);
  SDLInput input;
  DummySound sound;
  LocalFS fs("../sd_card");

  CoreUI ui(&screen, &input, &sound, &fs, "../sd_card");

  // Boot
  ui.showBoot("/launcher/boot.txt");
  SDL_RenderPresent(ren);

  // wait for any key
  bool wait = true;
  while (wait) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) { if (ev.type == SDL_QUIT) { SDL_Quit(); return 0; } if (ev.type == SDL_KEYDOWN) wait = false; }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // enter main menu (this call blocks until exit)
  // we need a loop to keep polling input
  bool running = true;
  int sel = 0;
  // We'll manually re-implement menu loop here to pump events and update input
  std::string raw;
  fs::path menuPath = fs::path("../sd_card/launcher/menu.json");
  std::ifstream ifs(menuPath);
  std::string menuJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  auto j = nlohmann::json::parse(menuJson);
  auto items = j["menu"];

  while (running) {
    input.poll();
    // draw menu
    screen.fillScreen(COL_BLACK);
    screen.drawText(10, 10, "Main Menu", 2, COL_WHITE);
    int x0 = 10; int y = 40;
    for (int i=0;i<(int)items.size();++i) {
      int x = x0 + i*90;
      screen.fillRect(x,y,80,60, i==sel?COL_DARKGRAY:COL_LIGHTGRAY);
      screen.drawText(x+4,y+65, items[i].value("title",""), 1, i==sel?COL_YELLOW:COL_WHITE);
    }
    SDL_RenderPresent(ren);

    if (input.left()) sel = std::max(0, sel-1);
    if (input.right()) sel = std::min((int)items.size()-1, sel+1);
    if (input.btnA()) {
      std::string path = items[sel].value("path", std::string(""));
      if (path == "/games/pacman") {
        ui.loadPacman(path);
        ui.runPacman();
      }
    }
    if (input.btnB()) running = false;

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) { if (ev.type == SDL_QUIT) { running = false; } }

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  TTF_CloseFont(font);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  TTF_Quit(); SDL_Quit();
  return 0;
}
