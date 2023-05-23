#include <iostream>

#include "graphics.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HIGHT 768

#define GAME_HOST white

#define GAME_WITH_ENGINE true
#define ENGINE_DIFFICULTY 0
#define ENGINE_PATH "./res/engine/stockfish/stockfish-windows-x86-64-avx2.exe"

int main(int argc, char** argv) 
{
	graphics::Scene scene(GAME_HOST);

	if (!scene.init_SDL("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HIGHT))
	{
		std::cout << "Error! Unable to initialize SDL.";
		return 0;
	}

	if (!scene.init_objects())
	{
		std::cout << "Error! Unable to initialize objects.";
		return 0;
	}

	while (scene.running())
	{
		scene.handle_events();
		scene.update();
		scene.render();
	}
	scene.clean_objects();
	scene.clean_SDL();

	return 0;
}