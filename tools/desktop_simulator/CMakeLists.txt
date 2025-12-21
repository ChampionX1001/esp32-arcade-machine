cmake_minimum_required(VERSION 3.10)
project(arcade_desktop_simulator)

set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ../../lib/core_ui)
add_executable(sim main.cpp ../../lib/core_ui/CoreUI.cpp)

target_link_libraries(sim ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
