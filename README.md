# realLeague

A minimal MOBA (Multiplayer Online Battle Arena) game inspired by League of Legends, written in **C++20** using **SDL3** for rendering, input, and audio, and built with **CMake**.

## Features

- Single-player mode with AI-controlled enemies
- Click-to-move champion control (right-click)
- Ability system (Q/W/E/R keys) with cooldowns
- AI state machine: Idle → Moving → Attacking → Retreating
- 3-lane map with jungle, river, towers, and nexus
- A\* pathfinding on a navigation grid
- Fog of war (circular visibility per friendly unit)
- Entity Component System (ECS) architecture
- Health bars, ability cooldown icons, and minimap UI

---

## System Requirements

- OS: Ubuntu 20.04 / Debian 11 or newer (or any compatible APT-based distribution)
- CPU: x86_64, 2+ cores recommended
- RAM: 1 GB minimum
- GPU: Any GPU with OpenGL 2.1+ support (or Mesa software renderer)
- Disk: ~500 MB free (for build dependencies and compiled output)

---

## Installing Dependencies

SDL3 is automatically fetched and built from source at configure time via CMake `FetchContent`. You only need to install the system libraries that SDL3 depends on, along with the standard C++ build tools.

Run the following command to install all required packages:

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    g++ \
    cmake \
    git \
    pkg-config \
    ninja-build \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxss-dev \
    libxfixes-dev \
    libxxf86vm-dev \
    libxinerama-dev \
    libwayland-dev \
    libxkbcommon-dev \
    wayland-protocols \
    libdecor-0-dev \
    libasound2-dev \
    libpulse-dev \
    libpipewire-0.3-dev \
    libdbus-1-dev \
    libudev-dev \
    libdrm-dev \
    libgbm-dev \
    libgl-dev \
    libgles2-mesa-dev \
    libibus-1.0-dev \
    libglm-dev
```

### Package explanations

| Package | Purpose |
| --- | --- |
| `build-essential` | GCC/G++ compiler, `make`, and core build tools |
| `g++` | C++20-capable GNU C++ compiler |
| `cmake` | Build system generator |
| `git` | Version control (required by CMake FetchContent) |
| `pkg-config` | Library metadata lookup used during CMake configuration |
| `ninja-build` | Optional fast build backend for CMake |
| `libx11-dev` | X11 window system (SDL3 X11 backend) |
| `libxext-dev` | X11 extensions |
| `libxrandr-dev` | X11 RandR extension (display resolutions) |
| `libxcursor-dev` | X11 cursor support |
| `libxi-dev` | X11 input extension |
| `libxss-dev` | X11 screen saver extension |
| `libxfixes-dev` | X11 fixes extension |
| `libxxf86vm-dev` | X86 video mode extension |
| `libxinerama-dev` | Multi-monitor X11 support |
| `libwayland-dev` | Wayland display protocol (SDL3 Wayland backend) |
| `libxkbcommon-dev` | Keyboard handling for Wayland |
| `wayland-protocols` | Wayland protocol definitions |
| `libdecor-0-dev` | Wayland client-side window decorations |
| `libasound2-dev` | ALSA audio support |
| `libpulse-dev` | PulseAudio support |
| `libpipewire-0.3-dev` | PipeWire audio/video support |
| `libdbus-1-dev` | D-Bus IPC (used by SDL3 for system events) |
| `libudev-dev` | udev device event support (gamepads, etc.) |
| `libdrm-dev` | Direct Rendering Manager (KMS/DRM backend) |
| `libgbm-dev` | Generic Buffer Management (KMS/DRM backend) |
| `libgl-dev` | OpenGL development headers |
| `libgles2-mesa-dev` | OpenGL ES 2.0 support |
| `libibus-1.0-dev` | IBus input method support |
| `libglm-dev` | GLM math library (vectors, matrices) |

---

## Building the Game

### 1. Clone the repository

```bash
git clone https://github.com/Julien-Turcotte/realLeague.git
cd realLeague
```

### 2. Configure with CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

> SDL3 will be downloaded and compiled automatically during this step. An internet connection is required for the first build.

To use Ninja instead of Make (faster):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
```

### 3. Compile

```bash
cmake --build build -j$(nproc)
```

The compiled binary will be located at `build/realLeague`.

---

## Running the Game

```bash
./build/realLeague
```

---

## Controls

| Input | Action |
| --- | --- |
| **Right-click** | Move champion to target location |
| **Q** | Cast ability 1 (Fireball skillshot) |
| **W** | Cast ability 2 |
| **E** | Cast ability 3 |
| **R** | Cast ultimate ability |
| **Escape** | Quit the game |

---

## Project Structure

```
realLeague/
├── CMakeLists.txt
├── src/
│   └── main.cpp
├── engine/
│   ├── core/          # Game loop, timing
│   ├── render/        # SDL3 renderer abstraction
│   ├── input/         # Keyboard and mouse input
│   ├── audio/         # Audio stub
│   └── ecs/           # Entity Component System (World, components)
├── game/
│   ├── entities/      # EntityFactory (champions, minions, towers, projectiles)
│   ├── systems/       # MovementSystem, CombatSystem, AbilitySystem, AISystem, RenderSystem, CollisionSystem
│   ├── map/           # 3-lane map, navigation grid, A* pathfinding
│   ├── ui/            # Health bars, ability icons, minimap
│   └── abilities/     # Fireball skillshot and ability base classes
└── assets/
    ├── textures/
    └── sounds/
```

---

## Development Roadmap

| Phase | Description | Status |
| --- | --- | --- |
| Phase 1 | Engine basics: SDL3 window, rendering, input, ECS | ✅ MVP |
| Phase 2 | Core gameplay: movement, camera, minions, combat | ✅ MVP |
| Phase 3 | Champions: abilities, cooldowns | ✅ MVP |
| Phase 4 | Map + AI: lanes, towers, bots | ✅ MVP |
| Phase 5 | Polish: UI, effects, sound | 🚧 In progress |
| Phase 6 | Multiplayer: networking, state sync | 📅 Planned |

---

## Troubleshooting

**CMake cannot find SDL3 / FetchContent fails**
- Ensure `git` is installed: `git --version`
- Check internet connectivity during the first `cmake -B build` run

**Linker errors about missing `-lGL`, `-lX11`, etc.**
- Re-run the apt install command above and confirm all dev packages are installed

**Game window does not open / crashes on start**
- Verify your display server is running (X11 or Wayland)
- Try running with `DISPLAY=:0 ./build/realLeague` if using X11

**Wayland errors**
- Set `SDL_VIDEODRIVER=x11` to force X11: `SDL_VIDEODRIVER=x11 ./build/realLeague`

---

## License

This project is a student educational project. See the repository for licensing details.
