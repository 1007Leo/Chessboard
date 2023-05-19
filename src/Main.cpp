#include <iostream>

#include "graphics.hpp"
#include "board.hpp"
#include "chess_engine_provider.hpp"

#define WINDOW_WIDTH 1024
#define WINDOW_HIGHT 768

#define BOARD_SIZE (WINDOW_HIGHT - 200)
#define BOARD_PIXEL_X 100
#define BOARD_PIXEL_Y 100
#define GAME_HOST white

#define GAME_WITH_ENGINE true
#define ENGINE_DIFFICULTY 0
#define ENGINE_PATH "./res/engine/stockfish/stockfish-windows-x86-64-avx2.exe"

int main(int argc, char** argv) 
{
	Board board = Board();
	board.new_game();

	ChessEngineProvider engine = ChessEngineProvider(ENGINE_PATH, ENGINE_DIFFICULTY);
	if (GAME_WITH_ENGINE) {
		engine.start();
	}

	Graphics graphics = Graphics(BOARD_PIXEL_X, BOARD_PIXEL_Y, BOARD_SIZE, GAME_HOST);

	if (!graphics.init("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HIGHT)) {
		std::cout << "Graphics init error";
	}
	graphics.init_objects(board.get_board());

	while (graphics.running()) 
	{
		graphics.handle_events(&board, &engine);
		graphics.update(board.get_board(), board.get_current_turn());
		graphics.render(board.game_over());
	}
	graphics.clean();

	return 0;
}