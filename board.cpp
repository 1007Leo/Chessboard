#include <iostream>
#include "board.hpp"

Board::Board()
{
	f_length = 8;
	f_width = 8;
	f_game_field = std::vector < std::vector< Piece* > >(f_length, std::vector< Piece* >(f_width, nullptr));
	f_current_turn = white;
	f_last_move = { {-1, -1}, {-1, -1} };
}

void Board::set_piece(Piece* piece)
{
	f_game_field[piece->get_coord().row][piece->get_coord().column] = piece;
}

void Board::move_rook_if_castling(coordinate from, coordinate to)
{
	if (get_piece_at(from)->get_type() == King && abs(from.column - to.column) == 2)
	{
		if (from.column - to.column == 2)
		{
			f_game_field[from.row][0]->set_movement(true);
			f_game_field[from.row][0]->set_coord(coordinate{ from.row, to.column + 1 });
			std::swap(f_game_field[from.row][0], f_game_field[from.row][to.column + 1]);

		}
		else if (from.column - to.column == -2)
		{
			f_game_field[from.row][7]->set_movement(true);
			f_game_field[from.row][7]->set_coord(coordinate{ from.row, to.column - 1 });
			std::swap(f_game_field[from.row][7], f_game_field[from.row][to.column - 1]);
		}
	}
}

void Board::remove_pawn_if_enPassant(coordinate from, coordinate to)
{
	if (f_last_move == move{ {-1, -1}, {-1, -1} })
		return;

	int row_d = from.row - to.row,
		col_d = from.column - to.column;
	if (get_piece_at(from)->get_type() == e_type::Pawn &&
		get_piece_at(to) == nullptr && col_d != 0) 
	{
		f_game_field[to.row + row_d][to.column]->set_alive(false);
		f_game_field[to.row + row_d][to.column] = nullptr;
	}
		
}

Piece* Board::get_piece_by_criteria(e_type type, e_color color, unsigned int entry)
{
	for (auto& row : f_game_field)
	{
		for (auto& el : row)
		{
			if (el != nullptr && el->get_type() == type && el->get_color() == color)
			{
				if (entry == 0)
					return el;
				else
					entry--;
			}
		}
	}
	return nullptr;
}

bool Board::move_causing_self_check(coordinate from, coordinate to)
{
	Piece* ally_king = this->get_piece_by_criteria(King, this->f_current_turn);
	
	Piece* saved_piece = this->temp_move_piece(from, to, nullptr);
	bool result = ally_king->is_checked_scan(this->f_game_field);
	this->temp_move_piece(to, from, saved_piece);

	if (abs(from.column - to.column) == 2 && this->get_piece_at(from)->get_type() == King)
	{
		coordinate mid_pos = { from.row, from.column - sgn(from.column - to.column) };
		saved_piece = this->temp_move_piece(from, mid_pos, nullptr);
		result = result || ally_king->is_checked_scan(this->f_game_field);
		this->temp_move_piece(mid_pos, from, saved_piece);
	}

	return result;
}

Piece* Board::temp_move_piece(coordinate from, coordinate to, Piece* pcs_at_first_coord)
{
	Piece* saved_piece = this->get_piece_at(to);
	this->f_game_field[to.row][to.column] = this->f_game_field[from.row][from.column];
	this->get_piece_at(to)->set_coord(to);
	this->f_game_field[from.row][from.column] = pcs_at_first_coord;
	return saved_piece;
}

void Board::new_game()
{
	for (auto& row : f_game_field)
		for (auto& cell : row)
		{
			delete cell;
		}	

	set_piece(new pieces::King(white, "e1"));
	set_piece(new pieces::King(black, "e8"));

	set_piece(new pieces::Queen(white, "d1"));
	set_piece(new pieces::Queen(black, "d8"));

	set_piece(new pieces::Rook(white, "a1"));
	set_piece(new pieces::Rook(white, "h1"));
	set_piece(new pieces::Rook(black, "a8"));
	set_piece(new pieces::Rook(black, "h8"));

	set_piece(new pieces::Bishop(white, "c1"));
	set_piece(new pieces::Bishop(white, "f1"));
	set_piece(new pieces::Bishop(black, "c8"));
	set_piece(new pieces::Bishop(black, "f8"));

	set_piece(new pieces::Knight(white, "b1"));
	set_piece(new pieces::Knight(white, "g1"));
	set_piece(new pieces::Knight(black, "b8"));
	set_piece(new pieces::Knight(black, "g8"));

	std::string pos = "a2";
	for (int i = 0; i < f_width; i++) {
		set_piece(new pieces::Pawn(white, pos));
		pos[1] += 5;
		set_piece(new pieces::Pawn(black, pos));
		pos[0]++;
		pos[1] -= 5;
	}
}

Piece* Board::get_piece_at(coordinate coord)
{
	return f_game_field[coord.row][coord.column];
}

bool Board::able_to_move(coordinate from, coordinate to)
{
	if (to.column == -1 || to.row == -1)
		return false;

	if (from == to) 
		return false;
	
	Piece* from_piece = get_piece_at(from);
	Piece* to_piece = get_piece_at(to);

	if (from_piece != nullptr && to_piece != nullptr)
		if (from_piece->get_color() == to_piece->get_color()) 
			return false;

	if (!from_piece->movable(from, to, this->get_board(), this->f_last_move))
		return false;

	return !move_causing_self_check(from, to);
}

bool Board::make_move(coordinate from, coordinate to)
{
	if (able_to_move(from, to))
	{
		remove_pawn_if_enPassant(from, to);
		move_rook_if_castling(from, to);
		
		if (f_game_field[to.row][to.column] != nullptr)
		{
			f_game_field[to.row][to.column]->set_alive(false);
			f_game_field[to.row][to.column] = nullptr;
		}

		f_game_field[from.row][from.column]->set_coord(to);
		f_game_field[from.row][from.column]->set_movement(true);

		std::swap(f_game_field[from.row][from.column], f_game_field[to.row][to.column]);

		f_current_turn = (e_color)((f_current_turn + 1) % 2);
		f_last_move = {from, to};

		//print_board();

		return true;
	}
	return false;
}

const std::vector<std::vector<Piece*>>& Board::get_board()
{
	return f_game_field;
}

e_color Board::get_current_turn()
{
	return f_current_turn;
}

void Board::print_board()
{
	for (int i = 0; i < f_length; i++) {
		for (int j = 0; j < f_width; j++) {
			if (f_game_field[i][j] == nullptr)
				if ((i+j)%2 == 0)
					std::cout << "  ";
				else
					std::cout << "::";
			else {
				switch (f_game_field[i][j]->get_type())
				{
				case King:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wK";
					else
						std::cout << "bK";
					break;
				case Queen:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wQ";
					else
						std::cout << "bQ";
					break;
				case Bishop:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wB";
					else
						std::cout << "bB";
					break;
				case Knight:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wk";
					else
						std::cout << "bk";
					break;
				case Rook:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wR";
					else
						std::cout << "bR";
					break;
				case Pawn:
					if (f_game_field[i][j]->get_color() == white)
						std::cout << "wP";
					else
						std::cout << "bP";
					break;
				default:
					break;
				}
			}
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}
