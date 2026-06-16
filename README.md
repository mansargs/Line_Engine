# LineEngine

LineEngine is a lightweight C++/OpenGL application that renders 3D line maps from `.lge` files.
It uses SDL2 for windowing/input, GLEW for OpenGL extensions, and glm for math.

**Screenshots**

World example:
![World](imgs/World.png)

Fractal example:
![Fract](imgs/fract.png)

Julia set example:
![Julia](imgs/julia.png)

Van Gogh portrait example:
![VanGogh](imgs/van-gogh.png)

## Features

- Lightweight OpenGL line renderer for `.lge` map files.
- Isometric and top views with configurable rotations and scaling.
- Keyboard + mouse controls for interactive exploration.
- Ships with a large set of `test_maps` for quick demonstrations.
- Linux `.deb` package with desktop launcher, app icon, and `.lge` file association.

## Requirements

### Build from source

- C++17 compiler (g++, clang++)
- CMake 3.15+
- SDL2 development headers
- GLEW development headers
- glm development headers
- OpenGL development libraries

On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libglew-dev libglm-dev libgl1-mesa-dev
```

### Optional (recommended on Linux)

- **zenity** — file picker when launching LineEngine from the apps menu without opening a file first

```bash
sudo apt install zenity
```

## Build

From the project root:

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

The binary is at `build/bin/lineengine`. CMake copies `Shaders/` and `test_maps/` into that directory automatically.

### CMake options

| Option | Default | Description |
|--------|---------|-------------|
| `LGE_INSTALL_DEMO_MAPS` | `ON` | Include demo maps in system install / `.deb` |
| `LGE_BUILD_DEB` | `ON` | Enable CPack `.deb` generation (Linux only) |

Example — smaller package without demo maps:

```bash
cmake .. -DLGE_INSTALL_DEMO_MAPS=OFF
```

## Run

From the project root:

```bash
./build/bin/lineengine test_maps/basictest.lge
```

Or from the build output directory:

```bash
cd build/bin
./lineengine test_maps/basictest.lge
```

Pass the path to a `.lge` file as an argument. If no file is given on Linux and **zenity** is installed, a file picker opens. Without zenity, run LineEngine with a file path on the command line or double-click a `.lge` file after installing the package.

Development helper script (file picker + error dialogs):

```bash
./launch-lineengine
./launch-lineengine test_maps/basictest.lge
```

## Install (Linux `.deb`)

Build the package:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build . -j$(nproc)
cpack -G DEB
```

Install on this machine or copy `lineengine_1.0.0_amd64.deb` to another computer:

```bash
sudo dpkg -i lineengine_1.0.0_amd64.deb
sudo apt -f install   # if dependencies are missing
```

### Installed layout

| Path | Contents |
|------|----------|
| `/usr/bin/lineengine` | Application binary |
| `/usr/share/lineengine/Shaders/` | GLSL shaders |
| `/usr/share/lineengine/test_maps/` | Demo maps (if built with `LGE_INSTALL_DEMO_MAPS=ON`) |
| `/usr/share/applications/lineengine.desktop` | Apps menu entry |
| `/usr/share/mime/packages/lineengine.xml` | MIME type for `*.lge` |
| `/usr/share/icons/hicolor/*/apps/lineengine.png` | App icon |

Example after install:

```bash
lineengine /usr/share/lineengine/test_maps/basictest.lge
```

### Desktop integration

After installing the `.deb`:

- **Apps menu** — LineEngine appears with its icon.
- **Double-click a `.lge` file** — opens in LineEngine (no zenity required).
- **Apps menu launch without a file** — opens a zenity file picker if zenity is installed; otherwise pass a file path on the command line.

Override the data directory (shaders) if needed:

```bash
LGE_DATA_DIR=/custom/path lineengine map.lge
```

## Controls

- Escape: Quit
- `I`: Reset to isometric view
- `T`: Reset to top (orthographic) view
- Arrow keys or `W`/`A`/`S`/`D`: Pan the view
- `X`/`Y`/`Z` (isometric only): Rotate +5° on respective axis
- `J`/`K`/`L` (isometric only): Rotate -5° on respective axis
- `+`/`=` and `-`: Zoom in/out
- Mouse wheel: Zoom in/out
- Left mouse button + drag: Pan (click & drag)

## Example maps

See the bundled maps in [test_maps](test_maps/) for many examples including world maps, fractals and artistic inputs.

- **Source tree:** `test_maps/`
- **Dev build:** `build/bin/test_maps/`
- **Installed package:** `/usr/share/lineengine/test_maps/`

## Shaders

Shaders are in `Shaders/` and are copied into the build directory by CMake. When installed, they live in `/usr/share/lineengine/Shaders/`. Edit them and rebuild to change rendering appearance.

## Contributing

Contributions and bug reports are welcome. Open an issue or submit a pull request with a focused change.

## License

This project is open source and free to use by anyone.
You are allowed to use, modify, distribute, and build upon this code for any purpose, including commercial use.
No permission is required.
However, this project is provided "as is", without any warranty or liability.
Copyright (c) 2026 Manvel Sargsyan
