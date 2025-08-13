#include <iostream>

#include "graphics.hpp"
#include "chess_engine_provider.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HIGHT 768

#define GAME_HOST white

#define GAME_WITH_ENGINE true
#define ENGINE_DIFFICULTY 0
#define ENGINE_PATH "./res/engine/stockfish/stockfish-windows-x86-64-avx2.exe"

int main(int argc, char** argv) 
{
	ChessEngineProvider engine = ChessEngineProvider(ENGINE_PATH, ENGINE_DIFFICULTY);
	graphics::Scene scene(GAME_HOST, &engine, GAME_WITH_ENGINE);

	if (!scene.init_SDL("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HIGHT))
	{
		std::cout << "Error! Unable to initialize SDL or TTF.";
		return 0;
	}

	if (!scene.init_objects())
	{
		std::cout << "Error! Unable to initialize objects.";
		return 0;
	}

	while (scene.running())
	{
		scene.handle_engine();
		scene.handle_events();
		scene.update();
		scene.render();
	}
	scene.clean_objects();
	scene.clean_SDL();

	return 0;
}