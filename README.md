# Chessboard
SDL2-based chess game project.

# Installation
You can download the [latest release](https://github.com/1007Leo/Chessboard/releases/latest) unzip the folder and it's ready to go.

# Build
### Requirements:
* [Cmake](https://cmake.org/download/)
* [SDL2](https://github.com/libsdl-org/SDL/releases),  [SDL2-image](https://github.com/libsdl-org/SDL_image/releases) and [SDL2-ttf](https://github.com/libsdl-org/SDL_ttf/releases) (devel-2.x.x version)
* [Boost](https://www.boost.org/releases/1.86.0/) (1.86 or earlier)
* Visual Studio or standalone build tools (like [w64devkit](https://github.com/skeeto/w64devkit))

Extract SDL and Boost libraries to the libs folder (eg. `libs/SDL2-2.x.x` `libs/SDL2_images-2.x.x` `libs/SDL2_ttf-2.x.x` `libs/boost_1_86_0`). You can either use prebult version of boost or buld it yourself. <b>MSVC prebuilt libraries won't work with MinGW generator.</b> Then create the build folder and run the following commands from within it:
### Windows
1. `cmake ..` for your default generator\
   `cmake .. -G "Visual Studio 17 2022"` or other versions for VS\
   `cmake .. -G "MinGW Makefiles"` for MinGW

2. `cmake --build . --config=Release` or open the solution and build it in Visual Studio

Or alternatively, you can run the `run.bat` with `-r` for release and `-d` for debug.
<!-- ### Linux
1. Work in progress -->

# Third-party sources
* [JohnPablok Cburnett Chess](https://opengameart.org/content/chess-pieces-and-board-squares)
* [Default](https://font.download/font/default) font
* [Stockfish](https://stockfishchess.org/) engine
