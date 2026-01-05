Setting up SDL2 & SDL2_ttf for this project (Windows) — VS Code friendly

Option A: Use vcpkg (recommended)
1. Install Git and Visual Studio Build Tools / CMake if not already installed.
2. Open PowerShell and run the included script to clone and bootstrap vcpkg:
   - `powershell -ExecutionPolicy Bypass -File tools/desktop_simulator/setup_vcpkg.ps1`
3. After vcpkg installs packages, configure CMake to use vcpkg toolchain file (if you used vcpkg). In VS Code (CMake Tools) set `cmake.configureArgs` to include:
   - `-DCMAKE_TOOLCHAIN_FILE=${workspaceFolder}/third_party/vcpkg/scripts/buildsystems/vcpkg.cmake`
4. Use CMake Tools to configure and build the `tools/desktop_simulator` target.

If you prefer MSYS2/MinGW (you already have MSYS2 installed):
- I've added a workspace kit `MinGW-w64 (MSYS2)` in `.vscode/cmake-kits.json` that points to `C:/msys64/mingw64/bin/gcc.exe` and `g++.exe`.
- Open VS Code, install the recommended extensions, then open the **CMake: Select a Kit** command and choose **MinGW-w64 (MSYS2)**.
- Configure the project (CMake: Configure). The default build directory is set to `${workspaceFolder}/tools/desktop_simulator/build` in the workspace settings.
- The integrated terminal defaults to the MSYS2 MinGW64 shell (`C:\msys64\usr\bin\bash.exe`) for easier builds.

Option B: Manually download SDL2/SDL2_ttf development packages
1. Download SDL2 & SDL2_ttf development packages from https://www.libsdl.org and https://www.libsdl.org/projects/SDL_ttf/
2. Point CMake at the include and library directories (edit `tools/desktop_simulator/CMakeLists.txt` or configure via CMake GUI).

VS Code Configuration (CMake Tools)
- Install the CMake Tools extension.
- Open the project folder in VS Code.
- In the status bar, pick a kit (Visual Studio or MinGW) compatible with your toolchain.
- Configure the project (CMake: Configure) and then build the `sim` target.

Running the simulator
- Make sure `sd_card/` folder is present and contains `launcher/menu.json` and `launcher/ui.json`.
- Build and run the `sim` executable from the `tools/desktop_simulator` build folder.

Notes
- If you used vcpkg, the included `CMakeLists.txt` expects SDL2, SDL2_ttf, and nlohmann_json to be available via find_package.
- If you want me to attempt to run vcpkg install on your machine, tell me and I will execute the setup script in a terminal session (requires Git, network, and build tools installed).