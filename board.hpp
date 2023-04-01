#pragma once
#include <vector>
#include "piece.hpp"

class Board {
public:
	Board();

	void new_game();
	bool make_move(coordinate from, coordinate to);
	const std::vector< std::vector< Piece* > >& get_board();
	e_color get_current_turn();

	Piece* get_piece_at(coordinate coord);
	std::string coord_to_notation(coordinate coord);
	coordinate notation_to_coord(std::string pos);

	// Debug
	void print_board();

private:
	bool able_to_move(coordinate from, coordinate to);
	void set_piece(Piece* piece, std::string pos);

	int f_width;
	int f_length;
	e_color f_current_turn;

	std::vector< std::vector< Piece * > > f_game_field;
};