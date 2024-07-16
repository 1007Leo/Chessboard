#include "graphics.hpp"

SDL_Texture* graphics::Texture_Operator::create_texture(std::string img_name, SDL_Renderer* renderer)
{
	SDL_Surface* image = nullptr;
	SDL_Texture* texture = nullptr;

	image = IMG_Load((IMG_PATH + img_name).c_str());
	if (image != nullptr) {
		texture = SDL_CreateTextureFromSurface(renderer, image);
		SDL_FreeSurface(image);
	}
	return texture;
}

void graphics::Texture_Operator::draw(drawable* object, SDL_Renderer* renderer, double rotation)
{
	SDL_RenderCopyEx(renderer, object->texture, NULL, object->rectangle.get(), rotation, NULL, SDL_FLIP_NONE);
}

void graphics::Texture_Operator::draw(SDL_Rect* rect, SDL_Texture* tex, SDL_Renderer* renderer, double rotation)
{
	SDL_RenderCopyEx(renderer, tex, NULL, rect, rotation, NULL, SDL_FLIP_NONE);
}

graphics::Scene::Scene()
{
}

graphics::Scene::Scene(e_color game_host)
{
	f_game_host = game_host;
}

bool graphics::Scene::init_SDL(const char* title, int xpos, int ypos, int width, int hight)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		f_hight = hight;
		f_width = width;

		f_window = SDL_CreateWindow(title, xpos, ypos, width, hight, SDL_WINDOW_SHOWN);
		f_renderer = SDL_CreateRenderer(f_window, -1, 0);

		if (!f_window || !f_renderer)
			f_is_running = false;
		else
			f_is_running = true;

	}
	return f_is_running;
}

bool graphics::Scene::init_objects()
{
	f_logic_board.new_game();

	if (f_el_board.init(100, 100, f_hight - 200, BOARD_TEXTURE, f_renderer) &&
		f_el_pieces.init(f_logic_board.get_board(), f_game_host, &f_el_board, f_renderer) &&
		f_el_available_moves.init(20, "avlbl_move.png", f_renderer) &&
		f_el_promotion_selector.init(&f_el_board, f_renderer))
		return true;
	return false;
}

void graphics::Scene::handle_events()
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
			this->clean_objects();
			this->init_objects();
		}
		break;
	case SDL_MOUSEMOTION:
		f_mouse_pos = { event.motion.x, event.motion.y };

		if (f_lmb_down && f_el_pieces.get_selected() != nullptr)
		{
			f_el_pieces.get_selected()->rectangle.get()->x = f_mouse_pos.x - f_click_offset.x;
			f_el_pieces.get_selected()->rectangle.get()->y = f_mouse_pos.y - f_click_offset.y;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (f_lmb_down && event.button.button == SDL_BUTTON_LEFT)
		{
			if (f_el_pieces.get_selected() != nullptr)
			{
				if (f_logic_board.make_move(pixels_to_coord(*f_el_pieces.get_selected_origin()), pixels_to_coord(f_mouse_pos)))
				{
					f_el_pieces.center_piece(f_el_pieces.get_selected(), &f_mouse_pos);
				}
				else
					f_el_pieces.center_piece(f_el_pieces.get_selected(), f_el_pieces.get_selected_origin());

				f_el_pieces.get_selected()->rectangle.get()->h -= 10;
				f_el_pieces.get_selected()->rectangle.get()->w -= 10;

				f_el_pieces.set_selected(nullptr);
			}
			f_lmb_down = false;
		}
		f_el_available_moves.clean();
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (!f_lmb_down && event.button.button == SDL_BUTTON_LEFT && !f_logic_board.game_over())
		{
			f_lmb_down = true;

			if (f_el_promotion_selector.is_open())
			{
				graphic_piece* selected = f_el_promotion_selector.get_selectetd(&f_mouse_pos);
				if (selected != nullptr)
				{
					f_el_pieces.init_piece(f_logic_board.initiate_promotion(selected->type), f_game_host);
					f_el_promotion_selector.set_open(false);
				}
				break;
			}
			for (auto& piece : f_el_pieces.get_all_pieces())
			{
				if (SDL_PointInRect(&f_mouse_pos, piece.get()->rectangle.get())
					&& piece.get()->render_state == rendered
					&& piece.get()->color == f_logic_board.get_current_turn()
				)
				{
					f_el_pieces.set_selected(piece.get());
					f_el_pieces.set_selected_origin({ piece.get()->rectangle.get()->x,  piece.get()->rectangle.get()->y });

					f_el_pieces.get_selected()->rectangle.get()->w += 10;
					f_el_pieces.get_selected()->rectangle.get()->h += 10;

					f_click_offset.x = piece.get()->rectangle.get()->w / 2 - 2;
					f_click_offset.y = piece.get()->rectangle.get()->h / 2 - 2;

					f_el_pieces.get_selected()->rectangle.get()->x = f_mouse_pos.x - f_click_offset.x;
					f_el_pieces.get_selected()->rectangle.get()->y = f_mouse_pos.y - f_click_offset.y;

					std::list<SDL_Point> moves_positions;
					for (auto& coord : f_logic_board.get_available_moves(f_logic_board.get_piece_at(f_el_pieces.get_selected()->coord)))
					{
						moves_positions.push_back(coord_to_pixels(coord));
					}
					f_el_available_moves.set_moves(moves_positions);
					moves_positions.clear();
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

void graphics::Scene::update()
{
	f_el_pieces.update(f_logic_board.get_board(), f_game_host);
	f_el_promotion_selector.update(f_logic_board.get_board());
}

void graphics::Scene::render()
{
	SDL_SetRenderDrawColor(f_renderer, 0, 0, 0, 255);
	SDL_RenderClear(f_renderer);

	f_el_board.render(f_game_host);
	f_el_pieces.render_all();
	f_el_available_moves.render();
	f_el_pieces.render_selected();
	f_el_promotion_selector.render((e_color)((f_logic_board.get_current_turn() + 1) % 2));

	SDL_RenderPresent(f_renderer);
}

void graphics::Scene::clean_objects()
{
	f_el_board.clean();
	f_el_pieces.clean();
	f_el_available_moves.clean();
	f_el_promotion_selector.clean();
}

void graphics::Scene::clean_SDL()
{
	SDL_DestroyWindow(f_window);
	SDL_DestroyRenderer(f_renderer);
	SDL_Quit();
}

bool graphics::Scene::running()
{
	return f_is_running;
}

coordinate graphics::Scene::pixels_to_coord(SDL_Point pixels_pos)
{
	coordinate res_coord = {-1, -1};
	const SDL_Rect* board_origin = f_el_board.origin();

	if (pixels_pos.x < board_origin->x ||
		pixels_pos.x >= board_origin->x + f_el_board.size() ||
		pixels_pos.y < board_origin->y ||
		pixels_pos.y >= board_origin->y + f_el_board.size())
		return res_coord;
	res_coord.row = (pixels_pos.y - board_origin->y) / f_el_board.cell_size();
	res_coord.column = (pixels_pos.x - board_origin->x) / f_el_board.cell_size();
	if (f_game_host == black)
	{
		res_coord.row = 7 - res_coord.row;
		res_coord.column = 7 - res_coord.column;
	}
	return res_coord;
}

SDL_Point graphics::Scene::coord_to_pixels(coordinate coord)
{
	SDL_Point res_point{ 0, 0 };
	if (coord.row < 0 || coord.row > 7 || coord.column < 0 || coord.column > 7)
		return res_point;

	if (f_game_host == black)
	{
		coord.row = 7 - coord.row;
		coord.column = 7 - coord.column;
	}

	res_point.x = f_el_board.origin()->x + coord.column * f_el_board.cell_size();
	res_point.y = f_el_board.origin()->y + coord.row * f_el_board.cell_size();

	return res_point;
}

int graphics::Element_Board::size()
{
	return f_board.rectangle->w;
}

int graphics::Element_Board::cell_size()
{
	return f_cell_size;
}

const SDL_Rect* graphics::Element_Board::origin()
{
	return f_board.rectangle.get();
}

bool graphics::Element_Board::init(int x, int y, int size, std::string texture_name, SDL_Renderer* renderer)
{
	f_renderer = renderer;
	f_board.rectangle = std::make_unique<SDL_Rect>(SDL_Rect{ x,y,size,size });
	f_cell_size = size / 8;
	f_board.texture = create_texture(texture_name, renderer);
	if (f_board.texture == nullptr)
		return false;
	return true;
}

void graphics::Element_Board::render(e_color game_host)
{
	double rotate = 0;
	if (game_host == black)
		rotate = 180;
	draw(&f_board, f_renderer, rotate);
}

void graphics::Element_Board::clean()
{
	SDL_DestroyTexture(f_board.texture);
}

bool graphics::Element_Pieces::init(const std::vector<std::vector<Piece*>>& board, e_color host, Element_Board* gr_board, SDL_Renderer* renderer)
{
	f_renderer = renderer;
	f_gr_board = gr_board;
	for (auto& row : board)
	{
		for (auto& piece : row)
		{
			if (piece != nullptr)
				if (!init_piece(piece, host))
					return false;
		}
	}
	return true;
}

void graphics::Element_Pieces::update(const std::vector<std::vector<Piece*>>& board, e_color host)
{
	for (auto& gr_piece : f_pieces)
		gr_piece->render_state = not_rendered;

	for (auto& row : board)
		for (auto& piece : row)
		{
			if (piece != nullptr)
			{
				for (auto& gr_piece : f_pieces)
				{
					if (gr_piece->render_state != pending && 
						gr_piece->type == piece->get_type() && 
						gr_piece->color == piece->get_color())
					{
						if (gr_piece->coord != piece->get_coord())
						{
							gr_piece->coord = piece->get_coord();
							center_piece(gr_piece.get(), gr_piece->coord, host);
						}
						gr_piece->render_state = pending;
						break;
					}
				}
			}
		}
}

void graphics::Element_Pieces::render_all()
{
	for (auto& piece : f_pieces)
	{
		if (piece->render_state == pending)
		{
			draw(piece->rectangle.get(), piece->texture, f_renderer);
			piece->render_state = rendered;
		}
	}
}

void graphics::Element_Pieces::render_selected()
{
	if (f_selected_piece != nullptr)
	{
		draw(f_selected_piece->rectangle.get(), f_selected_piece->texture, f_renderer);
		f_selected_piece->render_state = rendered;
	}
}

void graphics::Element_Pieces::clean()
{
	for (auto& piece : f_pieces)
		SDL_DestroyTexture(piece.get()->texture);
	f_pieces.clear();
}

graphics::graphic_piece* graphics::Element_Pieces::get_selected()
{
	return f_selected_piece;
}

SDL_Point* graphics::Element_Pieces::get_selected_origin()
{
	return &f_selected_piece_origin;
}

void graphics::Element_Pieces::set_selected(graphic_piece* selected_piece)
{
	f_selected_piece = selected_piece;
}

void graphics::Element_Pieces::set_selected_origin(SDL_Point origin)
{
	f_selected_piece_origin = origin;
}

bool graphics::Element_Pieces::init_piece(Piece* piece, e_color host)
{
	std::string name = f_type_to_string[piece->get_type()] + f_color_to_string[piece->get_color()] + ".png";

	coordinate piece_coord = piece->get_coord();
	if (host == black) 
	{
		piece_coord.row = 7 - piece_coord.row;
		piece_coord.column = 7 - piece_coord.column;
	}

	int cell_size = f_gr_board->cell_size();

	int posX = piece_coord.column * cell_size + f_gr_board->origin()->x + (int)(0.12 * cell_size);
	int posY = piece_coord.row * cell_size + f_gr_board->origin()->y + (int)(0.12 * cell_size);
	int piece_size = cell_size - (int)(cell_size * 0.2);

	SDL_Texture* curTex = create_texture(name, f_renderer);
	if (curTex == nullptr)
		return false;

	std::unique_ptr<graphic_piece> cur_gr_pc = std::make_unique<graphic_piece> ( 
		graphic_piece{ 
			std::make_unique<SDL_Rect>(SDL_Rect{ posX, posY, piece_size, piece_size }), 
			curTex, 
			piece->get_type(), 
			piece->get_color(), 
			piece->get_coord()
		}
	);

	f_pieces.push_back(std::move(cur_gr_pc));
	return true;
}

void graphics::Element_Pieces::center_piece(graphic_piece* gr_piece, coordinate coord, e_color host)
{
	if (host == black)
	{
		coord.row = 7 - coord.row;
		coord.column = 7 - coord.column;
	}

	int cell_size = f_gr_board->cell_size();

	int newX = cell_size * coord.column + f_gr_board->origin()->x + (int)(0.12 * cell_size),
		newY = cell_size * coord.row + f_gr_board->origin()->y + (int)(0.12 * cell_size);

	if (coord.row >= 0 && coord.row <= 7 &&
		coord.column >= 0 && coord.column <= 7)
	{
		gr_piece->rectangle.get()->x = newX;
		gr_piece->rectangle.get()->y = newY;
	}
}

void graphics::Element_Pieces::center_piece(graphic_piece* gr_piece, SDL_Point* point)
{
	const SDL_Rect* board_origin = f_gr_board->origin();
	int cell_size = f_gr_board->cell_size();

	int newX = cell_size * ((point->x - board_origin->x) / cell_size) + board_origin->x + (int)(0.12 * cell_size),
		newY = cell_size * ((point->y - board_origin->y) / cell_size) + board_origin->y + (int)(0.12 * cell_size);
	if (point->x >= board_origin->x &&
		point->y >= board_origin->y &&
		point->x < board_origin->x + f_gr_board->size() &&
		point->y < board_origin->y + f_gr_board->size())
	{
		gr_piece->rectangle.get()->x = newX;
		gr_piece->rectangle.get()->y = newY;
	}
	else
	{
		gr_piece->rectangle.get()->x = f_selected_piece_origin.x;
		gr_piece->rectangle.get()->y = f_selected_piece_origin.y;
	}
}

std::list<std::unique_ptr<graphics::graphic_piece>>& graphics::Element_Pieces::get_all_pieces()
{
	return f_pieces;
}

bool graphics::Element_Available_Moves::init(int size, std::string texture_name, SDL_Renderer* renderer)
{
	f_renderer = renderer;
	SDL_Texture* tex = create_texture(texture_name, renderer);
	if (tex == nullptr)
		return false;
	f_moves_highlight = { std::make_unique<SDL_Rect> (SDL_Rect{ 0, 0, size, size }), tex};
	return true;
}

void graphics::Element_Available_Moves::render()
{
	for (auto& pos : f_available_moves)
	{
		SDL_Point cur_point = pos;
		f_moves_highlight.rectangle.get()->x = cur_point.x + 25;
		f_moves_highlight.rectangle.get()->y = cur_point.y + 25;
		draw(&f_moves_highlight, f_renderer);
	}
}

void graphics::Element_Available_Moves::clean()
{
	f_available_moves.clear();
}

void graphics::Element_Available_Moves::set_moves(std::list<SDL_Point> new_moves)
{
	f_available_moves.clear();
	f_available_moves = new_moves;
}

bool graphics::Element_Promotion_Selector::init(Element_Board* gr_board, SDL_Renderer* renderer)
{
	f_renderer = renderer;
	f_gr_board = gr_board;

	int width = 4 * (gr_board->cell_size() + 20) + 20,
		hight = gr_board->cell_size() + 20;
	int pos_x = gr_board->size() / 2 + gr_board->origin()->x - width / 2,
		pos_y = gr_board->size() / 2 + gr_board->origin()->y - hight / 2;
	f_selector_background = { pos_x , pos_y, width, hight };
	return fill_pieces_for_selector();
}

//change board to pieces list
void graphics::Element_Promotion_Selector::update(const std::vector<std::vector<Piece*>>& board)
{
	for (auto& row : board)
		for (auto& piece : row)
		{
			if (piece != nullptr && piece->is_promoting() && !f_piece_selector_is_open)
			{
				f_piece_selector_is_open = true;
			}
		}
}

void graphics::Element_Promotion_Selector::render(e_color color)
{
	if (f_piece_selector_is_open)
	{
		SDL_SetRenderDrawColor(f_renderer, 200, 150, 30, 255);
		SDL_RenderFillRect(f_renderer, &f_selector_background);
		for (auto& pcs : f_pieces_for_selector)
		{
			if (pcs.color == color)
				draw(pcs.rectangle.get(), pcs.texture, f_renderer);
		}
	}
}

void graphics::Element_Promotion_Selector::clean()
{
	for (auto& piece : f_pieces_for_selector)
		SDL_DestroyTexture(piece.texture);
	f_pieces_for_selector.clear();
}

graphics::graphic_piece* graphics::Element_Promotion_Selector::get_selectetd(SDL_Point* mouse_pos)
{
	for (auto& pcs : f_pieces_for_selector)
	{
		if (SDL_PointInRect(mouse_pos, pcs.rectangle.get()))
		{
			return &pcs;
		}
	}
	return nullptr;
}

bool graphics::Element_Promotion_Selector::is_open()
{
	return f_piece_selector_is_open;
}

void graphics::Element_Promotion_Selector::set_open(bool state)
{
	f_piece_selector_is_open = state;
}

bool graphics::Element_Promotion_Selector::fill_pieces_for_selector()
{
	bool res = true;
	for (int i = Knight; i <= Queen; i++)
	{
		for (int j = white; j <= black; j++)
			res = res && create_piece_drawable((e_type)i, (e_color)j, i);
	}
	return res;
}

bool graphics::Element_Promotion_Selector::create_piece_drawable(e_type type, e_color color, int offset)
{
	std::string name = f_type_to_string[type] + f_color_to_string[color] + ".png";
	SDL_Texture* texture = create_texture(name, f_renderer);
	if (texture == nullptr)
		return false;
	int pos_x = f_selector_background.x + 20,
		pos_y = f_selector_background.y + 10;

	int cell_size = f_gr_board->cell_size();
	std::unique_ptr<SDL_Rect> rectangle = std::make_unique<SDL_Rect>(SDL_Rect{ pos_x + offset * (cell_size + 20), pos_y, cell_size, cell_size });
	f_pieces_for_selector.push_back({ std::move(rectangle), texture, type, color, {-1, -1}});
	return true;
}
