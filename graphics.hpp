#pragma once

#include <list>
#include <vector>
#include <memory>

#if defined(__unix)
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_image.h"
#else
	#include "SDL.h"
	#include "SDL_image.h"
#endif

#include "board.hpp"

#if defined (__unix)
	#define IMG_PATH "res/Textures/"
#else 
	#define IMG_PATH "res\\Textures\\"
#endif

#define BOARD_TEXTURE "Board2.png"

namespace graphics
{
	enum e_render_state { not_rendered = 0, rendered = 1, pending = 2 };

	struct drawable
	{
		std::unique_ptr<SDL_Rect> rectangle;
		SDL_Texture* texture;
	};

	struct graphic_piece
	{
		std::unique_ptr<SDL_Rect> rectangle;
		SDL_Texture* texture;

		e_type type;
		e_color color;
		coordinate coord;
		e_render_state render_state = not_rendered;
	};

	class Texture_Operator
	{
	protected:
		//don't change the order
		std::string f_type_to_string[6] = { "Knight", "Bishop", "Rook", "Queen", "King", "Pawn" };
		std::string f_color_to_string[2] = { "White", "Black" };

		SDL_Texture* create_texture(std::string img_name, SDL_Renderer* renderer);
		void draw(drawable* object, SDL_Renderer* renderer, double rotation = 0);
		void draw(SDL_Rect* rect, SDL_Texture* tex , SDL_Renderer* renderer, double rotation = 0);
	};

	class Element_Board : public Texture_Operator
	{
	public:
		int size();
		int cell_size();
		const SDL_Rect* origin();

		bool init(int x, int y, int size, std::string texture_name, SDL_Renderer* renderer);
		void render(e_color game_host);
		void clean();
	private:
		SDL_Renderer* f_renderer;
		drawable f_board;
		int f_cell_size;
	};

	class Element_Pieces : public Texture_Operator
	{
	public:
		bool init(const std::vector<std::vector<Piece*>>& board, e_color host, Element_Board* gr_board, SDL_Renderer* renderer);
		bool init_piece(Piece* piece, e_color host);
		void update(const std::vector<std::vector<Piece*>>& board, e_color host);
		void render_all();
		void render_selected();
		void clean();

		void center_piece(graphic_piece* gr_piece, coordinate coord, e_color host);
		void center_piece(graphic_piece* gr_piece, SDL_Point* point);

		std::list<std::unique_ptr<graphic_piece>>& get_all_pieces();

		graphic_piece* get_selected();
		SDL_Point* get_selected_origin();
		void set_selected(graphic_piece* selected_piece);
		void set_selected_origin(SDL_Point origin);

	private:
		Element_Board* f_gr_board;

		SDL_Renderer* f_renderer;
		std::list<std::unique_ptr<graphic_piece>> f_pieces;
		graphic_piece* f_selected_piece = nullptr;
		SDL_Point f_selected_piece_origin = {-1, -1};
	};

	class Element_Available_Moves: public Texture_Operator
	{
	public:
		bool init(int size, std::string texture_name, SDL_Renderer* renderer);
		void render();
		void clean();

		void set_moves(std::list<SDL_Point> new_moves);
	private:
		SDL_Renderer* f_renderer;
		drawable f_moves_highlight;

		std::list<SDL_Point> f_available_moves;
	};

	class Element_Promotion_Selector : public Texture_Operator
	{
	public:
		bool init(Element_Board* gr_board, SDL_Renderer* renderer);
		void update(const std::vector<std::vector<Piece*>>& board);
		void render(e_color color);
		void clean();

		graphic_piece* get_selectetd(SDL_Point* mouse_pos);

		bool is_open();
		void set_open(bool state);
	private:
		bool fill_pieces_for_selector();
		bool create_piece_drawable(e_type type, e_color color, int offset);

		Element_Board* f_gr_board;

		SDL_Renderer* f_renderer;

		bool f_piece_selector_is_open = false;
		SDL_Rect f_selector_background;
		std::list<graphic_piece> f_pieces_for_selector;
	};

	class Scene
	{
	public:
		Scene();
		Scene(e_color game_host);

		bool init_SDL(const char* title, int xpos, int ypos, int width, int hight);
		bool init_objects();
		void handle_events();
		void update();
		void render();
		void clean_objects();
		void clean_SDL();

		bool running();
	private:
		coordinate pixels_to_coord(SDL_Point pixels_pos);
		SDL_Point coord_to_pixels(coordinate coord);

		Board f_logic_board;

		SDL_Window* f_window;
		SDL_Renderer* f_renderer;
		int f_width = 1024, 
			f_hight = 768;

		bool f_is_running = false;
		e_color f_game_host = white;

		SDL_Point f_mouse_pos;
		SDL_Point f_click_offset;
		bool f_lmb_down = false;

		//Distinct elements of the scene
		Element_Board f_el_board;
		Element_Pieces f_el_pieces;
		Element_Available_Moves f_el_available_moves;
		Element_Promotion_Selector f_el_promotion_selector;
	};
}