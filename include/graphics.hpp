#pragma once

#include <list>
#include <array>
#include <vector>
#include <memory>
#include <map>

#if defined(__unix)
	#include "SDL2/SDL.h"
	#include "SDL2/SDL_image.h"
	#include "SDL2/SDL_ttf.h"
#else
	#include "SDL.h"
	#include "SDL_image.h"
	#include "SDL_ttf.h"
#endif

#include "board.hpp"
#include "chess_engine_provider.hpp"
#include "settings_provider.hpp"

namespace graphics
{
	inline SettingsProvider settings;

	enum e_render_state { not_rendered = 0, rendered = 1, pending = 2 };

	struct sdl_deleter
	{
		void operator()(SDL_Texture* p) const { SDL_DestroyTexture(p); }
		void operator()(SDL_Surface* p) const { SDL_FreeSurface(p); }
		void operator()(TTF_Font* p) const { TTF_CloseFont(p); }
	};

	struct drawable
	{
		std::unique_ptr<SDL_Rect> rectangle;
		std::unique_ptr<SDL_Texture, sdl_deleter> texture{nullptr, sdl_deleter()};
	};

	struct graphic_piece
	{
		drawable piece_obj;

		e_type type;
		e_color color;
		coordinate coord;
		e_render_state render_state = not_rendered;
	};

	class Texture_Operator
	{
	public:
		//don't change the order
		inline static const std::array<std::string, 6> f_type_to_string = { "Knight", "Bishop", "Rook", "Queen", "King", "Pawn" };
		inline static const std::array<std::string, 2> f_color_to_string = { "White", "Black" };
	protected:
		std::unique_ptr<SDL_Texture, sdl_deleter> create_texture(std::string img_name, SDL_Renderer* renderer);
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
	private:
		SDL_Renderer* f_renderer;
		drawable f_board_obj;
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
		void render(int cell_size);
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

	struct textfield {
		std::string text;
		bool active = true;
		drawable textfield_obj;
	};

	class Element_Textfields : Texture_Operator
	{
	public:
		bool init(SDL_Renderer* renderer);
		void render();
		void clean();

		void new_textfield(std::string field_name, std::string text, int x, int y, int w, int h, int size, bool active = true);
		void activate_textfield(std::string field_name);
		void deactivate_textfield(std::string field_name);
		void delete_textfield(std::string field_name);

		textfield* find_textfield(std::string field_name);
    	std::map<std::string, textfield>* get_textfields();

	private:
		std::map<std::string, textfield> f_textfields;
		SDL_Renderer* f_renderer;
	};

	struct color {
		int r, g, b, a;
	};

	class Element_Gameover_Plate {
	public:
		bool init(int x, int y, color color, int cell_size, Element_Textfields* textfields, SDL_Renderer* renderer);
		void render(bool game_over);
		void clean();
	private:
		color f_background_color;
		SDL_Rect f_background;
		Element_Textfields* f_textfields;
		SDL_Renderer* f_renderer;
	};

	class Scene
	{
	public:
		Scene();
		Scene(e_color game_host, ChessEngineProvider* engine, bool game_with_engine);

		bool init_SDL(const char* title, int xpos, int ypos, int width, int hight);
		bool init_objects();
		void handle_engine();
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
		ChessEngineProvider* f_engine;

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
		Element_Textfields f_el_textfields;
		Element_Gameover_Plate f_el_gameover_plate;
	};
}