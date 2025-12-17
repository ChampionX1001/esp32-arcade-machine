# Games Directory

Place your game ROM files or game data in this directory.

## Supported Game Types

### Custom Games
If you're developing custom games for this arcade machine, store the game data files here:
- Level maps
- Game state files
- High score data

### File Naming Convention
Use descriptive names for your games:
- `snake_levels.dat`
- `breakout_config.txt`
- `tetris_scores.dat`

### Example Game Data File Format

For a simple game config (JSON format):
```json
{
  "game": "snake",
  "difficulty": "medium",
  "speed": 5,
  "grid_size": 20,
  "initial_length": 3
}
```

For level data (CSV format):
```csv
level,width,height,obstacles
1,20,15,0
2,20,15,5
3,20,15,10
```

## Adding New Games

1. Place game data files in this directory
2. Update your main game logic in `src/main.cpp` to load the data
3. Test thoroughly before deploying to the arcade machine

## Current Games
(Add your game files here and document them below)

- No games installed yet - add your first game!
