#pragma once
#include <vector>
#include "piece.hpp"

const enum e_mate_type { none = 0, checkmate = 1, stalemate = 2};

class Board {
public:
	Board();
	//~Board();

	void new_game();
	bool make_move(coordinate from, coordinate to);

	const std::vector< std::vector< Piece* > >& get_board();
	e_color get_current_turn();
	move get_last_move();
	std::list<coordinate> get_available_moves(Piece* piece);
	Piece* initiate_promotion(e_type piece_type);

	bool game_over();
	void finish_game();

	Piece* get_piece_at(coordinate coord);
	bool able_to_move(coordinate from, coordinate to);

	// Debug
	void print_board();

private:
	void set_piece(Piece* piece);

	void move_rook_if_castling(coordinate from, coordinate to);
	void remove_pawn_if_enPassant(coordinate from, coordinate to);
	void change_king_state_if_check(coordinate from, coordinate to);
	void change_pawn_state_if_promoting(coordinate to);
	e_mate_type is_checkmate_or_stalemate();

	bool move_causing_self_check(coordinate from, coordinate to);
	Piece* get_piece_by_criteria(e_type type, e_color color, bool is_promoting = false, unsigned int entry = 0);
	Piece* temp_move_piece(coordinate from, coordinate to, Piece* pcs_at_first_cord);

	int f_width;
	int f_length;
	e_color f_current_turn;
	move f_last_move;
	bool f_game_over;

	std::list<std::unique_ptr<Piece>> f_pieces;
	std::vector< std::vector< Piece * > > f_game_field;
};