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

	// Debug
	void print_board();

private:
	bool able_to_move(coordinate from, coordinate to);
	void set_piece(Piece* piece);
	void move_rook_if_castling(coordinate from, coordinate to);
	void remove_pawn_if_enPassant(coordinate from, coordinate to);
	bool direction_scan(int row_d, int col_d, coordinate king_pos, coordinate ignore, coordinate dest);
	bool knight_scan(coordinate kingPos, e_color enemy_color);
	Piece* get_piece_by_criteria(e_type type, e_color color, unsigned int entry = 0);
	bool move_causing_self_check(coordinate from, coordinate to);

	int f_width;
	int f_length;
	e_color f_current_turn;
	move f_last_move;

	std::vector< std::vector< Piece * > > f_game_field;
};