cmake_minimum_required(VERSION 3.10)
project(arcade_desktop_simulator)

set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(../../lib/core_ui ../../lib/esp32_adapter ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
add_executable(sim main.cpp ../../lib/core_ui/CoreUI.cpp)

# Link SDL2 and SDL2_ttf (use imported targets if available)
if (TARGET SDL2::SDL2)
  set(SDL2_LIBS SDL2::SDL2 SDL2::SDL2main)
else()
  set(SDL2_LIBS ${SDL2_LIBRARIES})
endif()
if (TARGET SDL2::SDL2_ttf)
  set(SDL2_TTF_LIB SDL2::SDL2_ttf)
else()
  set(SDL2_TTF_LIB ${SDL2_TTF_LIBRARIES})
endif()

target_link_libraries(sim PRIVATE ${SDL2_LIBS} ${SDL2_TTF_LIB} SDL2_ttf nlohmann_json::nlohmann_json)
# On MinGW, ensure mingw32 and core SDL libs are linked explicitly (pkg-config expects -lmingw32 -mwindows -lSDL2main -lSDL2)
if (MINGW)
  target_link_libraries(sim PRIVATE mingw32 SDL2main SDL2)
endif()

