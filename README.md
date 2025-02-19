# Chessboard
SDL2-based chess game project.

# Installation
You can download the [latest release](https://github.com/1007Leo/Chessboard/releases/latest) unzip the folder and it's ready to go.

# Build
### Requirements:
* [Cmake](https://cmake.org/download/)
* [SDL2](https://github.com/libsdl-org/SDL/releases) and [SDL2-image](https://github.com/libsdl-org/SDL_image/releases) (devel-2.x.x version)
* Visual Studio or standalone build tools (like [w64devkit](https://github.com/skeeto/w64devkit))

Extract SDL libraries to the libs folder (eg. `libs/SDL2-2.x.x` `libs/SDL2_images-2.x.x`), create the build folder and run following commands from within it:
### Windows
1. `cmake ..` for default generator\
   `cmake .. -G "Visual Studio 17 2022"` for VS\
   `cmake .. -G "MinGW Makefiles"` for MinGW

2. `cmake --build .` or open the solution and build it in Visual Studio
<!-- ### Linux
1. Work in progress -->

# Third-party sources
* [JohnPablok Cburnett Chess](https://opengameart.org/content/chess-pieces-and-board-squares)
