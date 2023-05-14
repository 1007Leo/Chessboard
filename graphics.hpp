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
	Piece* piece;
};

struct drawable
{
	SDL_Rect* rectangle;
	SDL_Texture* texture;
};

class Graphics 
{
public:
	Graphics();
	Graphics(int board_pixel_x, int board_pixel_y, int board_size, e_color host_color);
	~Graphics();

	bool init(const char* title, int xpos, int ypos, int width, int height);
	void handle_events(Board* board);
	void update(const std::vector< std::vector< Piece* > >& board_matrix);
	void init_objects(const std::vector< std::vector< Piece* > >& board);
	void render();
	void clean();

	bool running();

private:
	SDL_Texture* get_texture(std::string img_name);
	void draw(drawable object, double rotation = 0);

	void render_board();
	void render_pieces();
	void render_selected_piece();
	void render_available_moves();
	void init_piece(Piece *piece);
	void center_piece(board_piece* piece, SDL_Point* f_mouse_pos);
	void center_piece(board_piece* piece, coordinate coord);
	coordinate pixels_to_coord(SDL_Point pixels_pos);
	SDL_Point coord_to_pixels(coordinate coord);

	std::string f_type[6] = { "King", "Queen", "Bishop", "Knight", "Rook", "Pawn" };
	std::string f_color[2] = { "White", "Black" };

	SDL_Window* f_window;
	SDL_Renderer* f_renderer;
	
	bool f_is_running;
	e_color f_game_host;
	int f_cell_size;

	board_piece* f_selected_piece = NULL;
	SDL_Point f_selected_piece_origin;
	SDL_Point f_mouse_pos;
	SDL_Point f_click_offset;
	bool f_lmb_down = false;

	drawable f_board_obj;
	drawable f_avlbl_move_obj;

	std::list<coordinate> f_available_moves;
	std::list<board_piece> f_pieces;
};