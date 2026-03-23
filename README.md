# realLeague

A minimal MOBA (Multiplayer Online Battle Arena) game built with C++20 and SDL3.

## Features
- Entity Component System (ECS) architecture
- 3 lanes with AI minions
- Player-controlled champion with abilities (Q=Fireball)
- Auto-attack combat system
- Camera that follows the player
- Fog of war
- Minimap UI
- A* pathfinding

## Requirements
- CMake 3.20+
- C++20 compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Internet connection (SDL3 is downloaded automatically)

## Build Instructions

### Linux/macOS
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/realLeague
```

### Windows
```powershell
cmake -B build
cmake --build build --config Release
.\build\Release\realLeague.exe
```

## Controls
- **Right Click**: Move champion
- **Q**: Fireball (skillshot toward cursor)
- **W/E/R**: Other abilities (placeholder)

## Architecture
- `engine/` - Core engine systems (ECS, rendering, input, audio)
- `game/`   - Game-specific logic (entities, systems, map, UI)
- `src/`    - Entry point and main Game class
- `include/`- Shared types (Vec2, Game)
- `assets/` - Textures and sounds