# Setup script for vcpkg and required packages (SDL2, SDL2_ttf, nlohmann-json)
# Run in PowerShell as Administrator or normal user (no elevation required for vcpkg local install)

$VCPKG_DIR = "$PSScriptRoot\..\..\third_party\vcpkg"
if (-Not (Test-Path $VCPKG_DIR)) {
    Write-Host "Cloning vcpkg into $VCPKG_DIR..."
    git clone https://github.com/microsoft/vcpkg.git $VCPKG_DIR
    Push-Location $VCPKG_DIR
    .\bootstrap-vcpkg.bat
    Pop-Location
} else {
    Write-Host "vcpkg already exists at $VCPKG_DIR"
}

# Install packages
Push-Location $VCPKG_DIR
./vcpkg install sdl2:x64-windows sdl2_ttf:x64-windows nlohmann-json:x64-windows
# Optional: integrate with MSBuild
./vcpkg integrate install
Pop-Location

Write-Host "Done. To use vcpkg with CMake, pass -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake to cmake."