"""
Simple UI + Pacman simulator using Pygame to exercise the SD-pack UI on Windows.
Run: pip install pygame
Then: python sim.py

Controls:
- Arrow keys: left/right/up/down (joystick)
- Z: Button1 (A)
- X: Button2 (B)
- Q: exit
"""

import pygame
import sys
import json
import os

ROOT = os.path.join(os.path.dirname(__file__), '..', '..', 'sd_card')
LAUNCHER_MENU = os.path.join(ROOT, 'launcher', 'menu.json')
UI_DEF = os.path.join(ROOT, 'launcher', 'ui.json')

pygame.init()
W, H = 480, 320
screen = pygame.display.set_mode((W,H))
font = pygame.font.SysFont('Arial', 20)

def draw_text(s, x, y, color=(255,255,255)):
    surf = font.render(s, True, color)
    screen.blit(surf, (x,y))

def load_json(path):
    with open(path,'r', encoding='utf-8') as f:
        return json.load(f)

ui = load_json(UI_DEF)
menu = load_json(LAUNCHER_MENU)

# boot screen
screen.fill((0,0,0))
draw_text(ui['boot']['title'], 160, 120)
draw_text(ui['boot']['subtitle'], 160, 150)
draw_text('Press any key', 160, 200)
pygame.display.flip()

clock = pygame.time.Clock()

# wait for any key
waiting = True
while waiting:
    for ev in pygame.event.get():
        if ev.type == pygame.KEYDOWN:
            waiting = False
        if ev.type == pygame.QUIT:
            pygame.quit(); sys.exit()
    clock.tick(30)

# Main menu - horizontal layout
sel = 0
running = True
while running:
    for ev in pygame.event.get():
        if ev.type == pygame.QUIT:
            running = False
        if ev.type == pygame.KEYDOWN:
            if ev.key == pygame.K_LEFT:
                sel = max(0, sel-1)
            if ev.key == pygame.K_RIGHT:
                sel = min(len(menu['menu'])-1, sel+1)
            if ev.key == pygame.K_z: # select
                # launch pacman simulation
                pass
            if ev.key == pygame.K_q:
                running = False

    screen.fill((0,0,0))
    draw_text('Main Menu', 20, 10)
    x = 20
    y = 50
    for i, item in enumerate(menu['menu']):
        rect = pygame.Rect(x + i*120, y, 100, 80)
        color = (200,200,40) if i==sel else (80,80,80)
        pygame.draw.rect(screen, color, rect)
        draw_text(item['title'], rect.x+10, rect.y+85)
    pygame.display.flip()
    clock.tick(30)

pygame.quit()
print('Simulator closed')
