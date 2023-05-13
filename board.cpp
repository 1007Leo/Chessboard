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
	if (get_piece_at(from)->get_type() == King)
	{
		coordinate mid_square = to;
		mid_square.column += sgn(from.column - to.column);

		for (int row_d = -1; row_d <= 1; row_d++)
			for (int col_d = -1; col_d <= 1; col_d++)
			{
				if (direction_scan(row_d, col_d, to, from, to))
					return true;
				if (abs(from.column - to.column) == 2) 
				{
					if (direction_scan(row_d, col_d, mid_square, from, to))
						return true;
				}
			}

		bool knight_scan_res = false;
		if (abs(from.column - to.column) == 2)
			knight_scan_res = knight_scan(mid_square, get_piece_at(from)->get_color());

		return (knight_scan_res || knight_scan(to, get_piece_at(from)->get_color()));
	}
	
	Piece cur_king = *get_piece_by_criteria(King, f_current_turn);
	

	int row_d = cur_king.get_coord().row - from.row,
		col_d = cur_king.get_coord().column - from.column;

	if ((abs(row_d) - abs(col_d) != 0) && row_d != 0 && col_d != 0)
		return false;

	return direction_scan(-sgn(row_d), -sgn(col_d), cur_king.get_coord(), from, to);
}

bool Board::direction_scan(int row_d, int col_d, coordinate king_pos, coordinate ignore, coordinate dest)
{
	if (row_d == 0 && col_d == 0)
		return false;

	king_pos.row += row_d;
	king_pos.column += col_d;

	int i = 0;

	while (king_pos.row >= 0 && king_pos.row <= 7 &&
		king_pos.column >= 0 && king_pos.column <= 7)
	{
		//std::cout << row_d << ' ' << col_d << '\n';
		Piece* cur_piece = get_piece_at(king_pos);
		if (cur_piece != nullptr && cur_piece->get_coord() != ignore)
		{
			if (i == 0)
			{
				if (cur_piece->get_color() != get_piece_at(ignore)->get_color() &&
					(cur_piece->get_type() == Pawn || cur_piece->get_type() == King))
				{
					return true;
				}
			}

			if (row_d == 0 || col_d == 0)
			{
				if (cur_piece->get_color() != get_piece_at(ignore)->get_color() &&
					(cur_piece->get_type() == Rook || cur_piece->get_type() == Queen))
				{
					return true;
				}
				return false;
			}
			else
			{
				if (cur_piece->get_color() != get_piece_at(ignore)->get_color() &&
					(cur_piece->get_type() == Bishop || cur_piece->get_type() == Queen))
				{
					return true;
				}
				return false;
			}
		}
		else if (cur_piece == nullptr && king_pos == dest)
		{
			return false;
		}
		king_pos.row += row_d;
		king_pos.column += col_d;
		i++;
	}
	return false;
}

bool Board::knight_scan(coordinate king_pos, e_color friendly_color)
{
	e_color enemy_color = (e_color)((friendly_color + 1) % 2);

	std::vector<coordinate> coords{ {king_pos.row - 2, king_pos.column + 1}, {king_pos.row - 1, king_pos.column + 2},
									{king_pos.row + 1, king_pos.column + 2}, {king_pos.row + 2, king_pos.column + 1},
									{king_pos.row + 2, king_pos.column - 1}, {king_pos.row + 1, king_pos.column - 2},
									{king_pos.row - 1, king_pos.column - 2}, {king_pos.row - 2, king_pos.column - 1} };
	for (auto coord : coords)
	{
		if (coord.row >= 0 && coord.row <= 7 && coord.column >= 0 && coord.column <= 7)
		{
			Piece* piece = get_piece_at(coord);
			if (piece != nullptr && piece->get_color() == enemy_color && piece->get_type() == Knight)
				return true;
		}
	}
	return false;
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
