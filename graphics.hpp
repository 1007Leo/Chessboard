#pragma once

#include <list>
#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#include "board.hpp"

#define IMG_PATH "res\\Textures\\"
#define BOARD_TEXTURE "Board2.png"

struct board_piece 
{
	SDL_Rect* rectangle;
	SDL_Texture* texture;
	bool show = true;
	e_type type;
	e_color color;
	coordinate coord;
};

class Graphics 
{
public:
	Graphics();
	Graphics(int board_pixel_x, int board_pixel_y, int board_size, e_color host_color);

	bool init(const char* title, int xpos, int ypos, int width, int height);
	void handle_events(Board* board);
	void update(const std::vector< std::vector< Piece* > >& board_matrix);
	void init_objects(const std::vector< std::vector< Piece* > >& board);
	void render();
	void clean();

	bool running();

private:
	void render_board();
	void render_pieces();
	void init_piece(e_type type, e_color color, int x, int y);
	void center_piece(board_piece* piece, SDL_Point* f_mouse_pos);
	coordinate pixels_to_coord(SDL_Point pixels_pos);

	std::string f_type[6] = { "King", "Queen", "Bishop", "Knight", "Rook", "Pawn" };
	std::string f_color[2] = { "White", "Black" };

	SDL_Window* f_window;
	SDL_Renderer* f_renderer;
	
	bool f_is_running;
	e_color f_game_host;

	bool f_lmb_down = false;
	board_piece* f_selected_piece = NULL;
	SDL_Point f_selected_piece_origin;
	SDL_Point f_mouse_pos;
	SDL_Point f_click_offset;

	int f_cell_size;
	SDL_Rect f_board_rect;
	SDL_Texture* f_board_tex;
	std::list< board_piece > f_pieces;
};