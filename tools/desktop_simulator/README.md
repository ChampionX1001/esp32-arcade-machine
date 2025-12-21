Desktop simulator (C++ / SDL2)

This simulator compiles the platform-agnostic CoreUI library and runs a simple SDL-based UI that reads files from `sd_card/`.

Requirements
- Windows: install SDL2 and SDL2_ttf development packages (or use vcpkg)
- CMake and a C++17 toolchain

Build
mkdir build && cd build
cmake ..
cmake --build .

Run
- Make sure `sd_card/` is next to the build tree (the simulator reads `../sd_card/launcher/menu.json`)
- Run the produced `sim` executable

Controls
- Arrow keys = joystick
- Z = Button A (select)
- X = Button B (back)
- Q = quit
