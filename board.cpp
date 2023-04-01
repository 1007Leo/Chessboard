#include <iostream>
#include "board.hpp"

Board::Board()
{
	f_length = 8;
	f_width = 8;
	f_game_field = std::vector < std::vector< Piece* > >(f_length, std::vector< Piece* >(f_width, nullptr));
	f_current_turn = white;
}

void Board::set_piece(Piece* piece, std::string pos)
{
	coordinate coord = notation_to_coord(pos);
	f_game_field[coord.row][coord.column] = piece;
}

void Board::new_game()
{
	for (auto& row : f_game_field)
		for (auto& cell : row)
		{
			delete cell;
		}	

	set_piece(new pieces::King(white), "e1");
	set_piece(new pieces::King(black), "e8");

	set_piece(new pieces::Queen(white), "d1");
	set_piece(new pieces::Queen(black), "d8");

	set_piece(new pieces::Rook(white), "a1");
	set_piece(new pieces::Rook(white), "h1");
	set_piece(new pieces::Rook(black), "a8");
	set_piece(new pieces::Rook(black), "h8");

	set_piece(new pieces::Bishop(white), "c1");
	set_piece(new pieces::Bishop(white), "f1");
	set_piece(new pieces::Bishop(black), "c8");
	set_piece(new pieces::Bishop(black), "f8");

	set_piece(new pieces::Knight(white), "b1");
	set_piece(new pieces::Knight(white), "g1");
	set_piece(new pieces::Knight(black), "b8");
	set_piece(new pieces::Knight(black), "g8");

	std::string pos = "a2";
	for (int i = 0; i < f_width; i++) {
		set_piece(new pieces::Pawn(white), pos);
		pos[1] += 5;
		set_piece(new pieces::Pawn(black), pos);
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

	if (from.column == to.column && from.row == to.row) 
		return false;
	
	Piece* from_piece = get_piece_at(from);
	Piece* to_piece = get_piece_at(to);

	//if (f_current_turn != from_piece->get_color())
		//return false;

	if (from_piece != nullptr && to_piece != nullptr)
		if (from_piece->get_color() == to_piece->get_color()) 
			return false;
	return from_piece->movable(from, to, this->get_board());
}

bool Board::make_move(coordinate from, coordinate to)
{
	if ( able_to_move(from, to) )
	{
		if (f_game_field[to.row][to.column] != nullptr)
		{
			f_game_field[to.row][to.column] = nullptr;
		}
		std::swap(f_game_field[from.row][from.column], f_game_field[to.row][to.column]);
		f_current_turn = (e_color)((f_current_turn + 1) % 2);
		//print_board();

		return true;
	}
	return false;
}

std::string Board::coord_to_notation(coordinate coord)
{
	std::string pos = "";
	pos[0] = 'a' + coord.column;
	pos[1] = 8 - coord.row + '0';
	return pos;
}

coordinate Board::notation_to_coord(std::string pos)
{
	coordinate coord;

	coord.row = 8 - (pos[1] - '0');
	coord.column = pos[0] - 'a';

	return coord;
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
