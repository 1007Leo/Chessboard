//#include <iostream>
#include "graphics.hpp"

Graphics::~Graphics()
{
	for (auto& piece : f_pieces) {
		delete piece.piece;
		delete piece.rectangle;
	}

	delete f_board_obj.rectangle;
	delete f_avlbl_move_obj.rectangle;
}

Graphics::Graphics()
{
	f_board_obj.rectangle = new SDL_Rect{ 100, 100, 600, 600 };
	f_cell_size = f_board_obj.rectangle->w / 8;
	f_game_host = white;
}

Graphics::Graphics(int board_pixel_x, int board_pixel_y, int board_size, e_color host_color)
{
	f_board_obj.rectangle = new SDL_Rect{ board_pixel_x, board_pixel_y, board_size, board_size };
	f_cell_size = f_board_obj.rectangle->w / 8;
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
		if (event.key.keysym.sym == SDLK_F1)
		{
			board->new_game();
			this->init_objects(board->get_board());
		}
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
		if (!f_lmb_down && event.button.button == SDL_BUTTON_LEFT && !board->game_over())
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
	f_selected_piece = NULL;
	f_pieces.clear();
	f_available_moves.clear();
	std::string board_file = BOARD_TEXTURE, avlbl_move_file = "avlbl_move.png";
	f_board_obj.texture = get_texture(board_file);
	f_avlbl_move_obj = { new SDL_Rect{0, 0, 20, 20}, get_texture(avlbl_move_file) };
	
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

	int posX = piece->get_coord().column * f_cell_size + f_board_obj.rectangle->x + (int)(0.12 * f_cell_size);
	int posY = piece->get_coord().row * f_cell_size + f_board_obj.rectangle->y + (int)(0.12 * f_cell_size);
	int piece_size = f_cell_size - (int)(f_cell_size * 0.2);

	SDL_Rect* curPiece = new SDL_Rect{ posX, posY, piece_size, piece_size };
	SDL_Texture* curTex = get_texture(name);

	f_pieces.push_back(board_piece{ curPiece, curTex, piece });
}

void Graphics::render_board()
{
	double rotate = 0;
	if (f_game_host == black)
		rotate = 180;
	draw(f_board_obj, rotate);
}

void Graphics::render_available_moves()
{
	for (auto pos : f_available_moves)
	{
		SDL_Point cur_point = coord_to_pixels(pos);
		f_avlbl_move_obj.rectangle->x = cur_point.x + 25;
		f_avlbl_move_obj.rectangle->y = cur_point.y + 25;
		draw(f_avlbl_move_obj);
	}
}

void Graphics::render_selected_piece()
{
	if (f_selected_piece != nullptr)
		draw({ f_selected_piece->rectangle, f_selected_piece->texture });
}

void Graphics::render_pieces()
{
	for (auto const& piece : f_pieces)
	{
		if (piece.piece->is_alive() && &piece != f_selected_piece)
		{
			draw({piece.rectangle, piece.texture});
		}
	}
}

void Graphics::center_piece(board_piece* piece, SDL_Point* f_mouse_pos)
{
	int newX = f_cell_size * ((f_mouse_pos->x - f_board_obj.rectangle->x) / f_cell_size) + f_board_obj.rectangle->x + (int)(0.12 * f_cell_size),
		newY = f_cell_size * ((f_mouse_pos->y - f_board_obj.rectangle->y) / f_cell_size) + f_board_obj.rectangle->y + (int)(0.12 * f_cell_size);
	if (f_mouse_pos->x >= f_board_obj.rectangle->x &&
		f_mouse_pos->y >= f_board_obj.rectangle->y &&
		f_mouse_pos->x < f_board_obj.rectangle->x + f_board_obj.rectangle->w &&
		f_mouse_pos->y < f_board_obj.rectangle->y + f_board_obj.rectangle->h)
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
	int newX = f_cell_size * coord.column + f_board_obj.rectangle->x + (int)(0.12 * f_cell_size),
		newY = f_cell_size * coord.row + f_board_obj.rectangle->y + (int)(0.12 * f_cell_size);

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

	res_point.y = f_board_obj.rectangle->y + coord.row * f_cell_size;
	res_point.x = f_board_obj.rectangle->x + coord.column * f_cell_size;
	
	return res_point;
}

SDL_Texture* Graphics::get_texture(std::string img_name)
{
	SDL_Surface* image = nullptr;
	SDL_Texture* texture = nullptr;

	image = IMG_Load((IMG_PATH + img_name).c_str());
	if (image != nullptr) {
		texture = SDL_CreateTextureFromSurface(f_renderer, image);
		SDL_FreeSurface(image);
	}
	return texture;
}

void Graphics::draw(drawable object, double rotation)
{
	SDL_RenderCopyEx(f_renderer, object.texture, NULL, object.rectangle, rotation, NULL, SDL_FLIP_NONE);
}

coordinate Graphics::pixels_to_coord(SDL_Point pixels_pos)
{
	coordinate res_coord;
	res_coord.row = -1;
	res_coord.column = -1;
	if (pixels_pos.x < f_board_obj.rectangle->x ||
		pixels_pos.x >= f_board_obj.rectangle->x + f_board_obj.rectangle->w ||
		pixels_pos.y < f_board_obj.rectangle->y ||
		pixels_pos.y >= f_board_obj.rectangle->y + f_board_obj.rectangle->h)
		return res_coord;
	res_coord.row = (pixels_pos.y - f_board_obj.rectangle->y) / f_cell_size;
	res_coord.column = (pixels_pos.x - f_board_obj.rectangle->x) / f_cell_size;
	if (f_game_host == black) 
	{
		res_coord.row = 7 - res_coord.row;
		res_coord.column= 7 - res_coord.column;
	}
	return res_coord;
}
