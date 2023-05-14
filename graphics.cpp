//#include <iostream>
#include "graphics.hpp"

Graphics::~Graphics()
{
	for (auto& piece : f_pieces) {
		delete piece.piece;
		delete piece.rectangle;
	}
}

Graphics::Graphics()
{
	f_board_rect = { 100, 100, 600, 600 };
	f_cell_size = f_board_rect.w / 8;
	f_game_host = white;
}

Graphics::Graphics(int board_pixel_x, int board_pixel_y, int board_size, e_color host_color)
{
	f_board_rect = { board_pixel_x, board_pixel_y, board_size, board_size };
	f_cell_size = f_board_rect.w / 8;
	f_game_host = host_color;
}

bool Graphics::init(const char* title, int xpos, int ypos, int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		f_window = SDL_CreateWindow(title, xpos, ypos, width, height, SDL_WINDOW_SHOWN);
		f_renderer = SDL_CreateRenderer(f_window, -1, 0);

		if (!f_window || !f_renderer)
			f_is_running = false;
		else
			f_is_running = true;
		
	}
	return f_is_running;
}

void Graphics::handle_events(Board* board)
{
	SDL_Delay(10);
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		f_is_running = false;	
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE)
			f_is_running = false;
		break;
	case SDL_MOUSEMOTION:
		f_mouse_pos = { event.motion.x, event.motion.y };

		if (f_lmb_down && f_selected_piece != NULL)
		{
			f_selected_piece->rectangle->x = f_mouse_pos.x - f_click_offset.x;
			f_selected_piece->rectangle->y = f_mouse_pos.y - f_click_offset.y;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (f_lmb_down && event.button.button == SDL_BUTTON_LEFT)
		{
			if (f_selected_piece != NULL)
			{
				if (board->make_move(pixels_to_coord(f_selected_piece_origin), pixels_to_coord(f_mouse_pos)))
				{
					center_piece(f_selected_piece, &f_mouse_pos);
				}
				else
					center_piece(f_selected_piece, &f_selected_piece_origin);
				
				f_selected_piece->rectangle->h -= 10;
				f_selected_piece->rectangle->w -= 10;

				f_selected_piece = NULL;
			}
			f_lmb_down = false;
		}
		f_available_moves.clear();
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (!f_lmb_down && event.button.button == SDL_BUTTON_LEFT)
		{
			f_lmb_down = true;
			for (auto & piece : f_pieces)
			{
				if (SDL_PointInRect(&f_mouse_pos, piece.rectangle) && piece.piece->is_alive() && piece.piece->get_color() == board->get_current_turn())
				//if (SDL_PointInRect(&f_mouse_pos, piece.rectangle) && piece.piece->is_alive()) // w/o move order
				{
					f_selected_piece = &piece;
					f_selected_piece_origin.x = piece.rectangle->x;
					f_selected_piece_origin.y = piece.rectangle->y;
					
					f_selected_piece->rectangle->h += 10;
					f_selected_piece->rectangle->w += 10;
					
					f_click_offset.x = piece.rectangle->w / 2 - 2;
					f_click_offset.y = piece.rectangle->h / 2 - 2;
					f_selected_piece->rectangle->x = f_mouse_pos.x - f_click_offset.x;
					f_selected_piece->rectangle->y = f_mouse_pos.y - f_click_offset.y;

					
					f_available_moves = board->get_available_moves(f_selected_piece->piece);
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

void Graphics::update(const std::vector< std::vector< Piece* > > &board_matrix)
{
	for (auto& piece : f_pieces)
	{
		if ( piece.piece->is_alive() && &piece != f_selected_piece)
		{
			coordinate update_coord = piece.piece->get_coord();
			if (f_game_host == black)
			{
				update_coord.row = 7 - update_coord.row;
				update_coord.column = 7 - update_coord.column;
			}
			center_piece(&piece, update_coord);
		}
	}
}

void Graphics::init_objects(const std::vector< std::vector< Piece* > >& board)
{
	std::string name = BOARD_TEXTURE, moves_name = "SelectedCircle.png";
	f_board_tex = SDL_CreateTextureFromSurface( f_renderer, IMG_Load( (IMG_PATH + name).c_str() ) );
	f_avlbl_moves_tex = SDL_CreateTextureFromSurface(f_renderer, IMG_Load((IMG_PATH + moves_name).c_str()));
	
	for (auto &row : board) 
	{
		for (auto &piece : row) 
		{
			if (piece != nullptr)
				init_piece(piece);
		}
	}
}

void Graphics::render()
{
	SDL_SetRenderDrawColor(f_renderer, 0, 0, 0, 255);
	SDL_RenderClear(f_renderer);

	// add to renderer
	render_board();
	render_pieces();
	render_available_moves();
	render_selected_piece();

	SDL_RenderPresent(f_renderer);
}

void Graphics::clean()
{
	SDL_DestroyWindow(f_window);
	SDL_DestroyRenderer(f_renderer);
	SDL_Quit();
}

bool Graphics::running()
{
	return f_is_running;
}

void Graphics::init_piece(Piece* piece)
{
	std::string name = f_type[piece->get_type()] + f_color[piece->get_color()] + ".png";

	int posX = piece->get_coord().column * f_cell_size + f_board_rect.x + (int)(0.12 * f_cell_size);
	int posY = piece->get_coord().row * f_cell_size + f_board_rect.y + (int)(0.12 * f_cell_size);
	int piece_size = f_cell_size - (int)(f_cell_size * 0.2);

	SDL_Rect* curPiece = new SDL_Rect{ posX, posY, piece_size, piece_size };
	SDL_Texture* curTex = SDL_CreateTextureFromSurface(f_renderer, IMG_Load((IMG_PATH + name).c_str()));

	f_pieces.push_back(board_piece{ curPiece, curTex, piece });
}

void Graphics::render_board()
{
	double angle = 0;
	if (f_game_host == black)
		angle = 180;
	SDL_RenderCopyEx(f_renderer, f_board_tex, NULL, &f_board_rect, angle, NULL, SDL_FLIP_NONE);
}

void Graphics::render_available_moves()
{
	SDL_Rect avlbl_move_rect{0, 0, 20, 20};

	for (auto pos : f_available_moves)
	{
		SDL_Point cur_point = coord_to_pixels(pos);
		avlbl_move_rect.x = cur_point.x + 25;
		avlbl_move_rect.y = cur_point.y + 25;
		SDL_RenderCopy(f_renderer, f_avlbl_moves_tex, NULL, &avlbl_move_rect);
	}
}

void Graphics::render_selected_piece()
{
	if (f_selected_piece != nullptr)
		SDL_RenderCopy(f_renderer, f_selected_piece->texture, NULL, f_selected_piece->rectangle);
}

void Graphics::render_pieces()
{
	for (auto const& piece : f_pieces)
	{
		if (piece.piece->is_alive() && &piece != f_selected_piece)
		{
			SDL_RenderCopy(f_renderer, piece.texture, NULL, piece.rectangle);
		}
	}
}

void Graphics::center_piece(board_piece* piece, SDL_Point* f_mouse_pos)
{
	int newX = f_cell_size * ((f_mouse_pos->x - f_board_rect.x) / f_cell_size) + f_board_rect.x + (int)(0.12 * f_cell_size),
		newY = f_cell_size * ((f_mouse_pos->y - f_board_rect.y) / f_cell_size) + f_board_rect.y + (int)(0.12 * f_cell_size);
	if (f_mouse_pos->x >= f_board_rect.x &&
		f_mouse_pos->y >= f_board_rect.y &&
		f_mouse_pos->x < f_board_rect.x + f_board_rect.w &&
		f_mouse_pos->y < f_board_rect.y + f_board_rect.h)
	{
		piece->rectangle->x = newX;
		piece->rectangle->y = newY;
	}
	else
	{
		piece->rectangle->x = f_selected_piece_origin.x;
		piece->rectangle->y = f_selected_piece_origin.y;
	}
}

void Graphics::center_piece(board_piece* piece, coordinate coord)
{
	int newX = f_cell_size * coord.column + f_board_rect.x + (int)(0.12 * f_cell_size),
		newY = f_cell_size * coord.row + f_board_rect.y + (int)(0.12 * f_cell_size);

	if (coord.row >= 0 && coord.row <= 7 &&
		coord.column >= 0 && coord.column <= 7)
	{
		piece->rectangle->x = newX;
		piece->rectangle->y = newY;
	}
}

SDL_Point Graphics::coord_to_pixels(coordinate coord)
{
	SDL_Point res_point{ 0, 0 };
	if (coord.row < 0 || coord.row > 7 || coord.column < 0 || coord.column > 7)
		return res_point;

	if (f_game_host == black)
	{
		coord.row = 7 - coord.row;
		coord.column = 7 - coord.column;
	}

	res_point.y = f_board_rect.y + coord.row * f_cell_size;
	res_point.x = f_board_rect.x + coord.column * f_cell_size;
	
	return res_point;
}

coordinate Graphics::pixels_to_coord(SDL_Point pixels_pos)
{
	coordinate res_coord;
	res_coord.row = -1;
	res_coord.column = -1;
	if (pixels_pos.x < f_board_rect.x ||
		pixels_pos.x >= f_board_rect.x + f_board_rect.w ||
		pixels_pos.y < f_board_rect.y ||
		pixels_pos.y >= f_board_rect.y + f_board_rect.h)
		return res_coord;
	res_coord.row = (pixels_pos.y - f_board_rect.y) / f_cell_size;
	res_coord.column = (pixels_pos.x - f_board_rect.x) / f_cell_size;
	if (f_game_host == black) 
	{
		res_coord.row = 7 - res_coord.row;
		res_coord.column= 7 - res_coord.column;
	}
	return res_coord;
}
