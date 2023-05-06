#include "piece.hpp"

Piece::Piece()
{
	f_type = Pawn;
	f_color = white;
	f_value = 0;
	f_alive = true;
	f_coord = coordinate{ 0, 0 };
}

Piece::Piece(e_type type, e_color color, int value, std::string pos)
{
	f_type = type;
	f_color = color;
	f_value = value;
	f_alive = true;
	f_coord = notation_to_coord(pos);
}

e_type Piece::get_type()
{
	return this->f_type;
}

e_color Piece::get_color()
{
	return this->f_color;
}

int Piece::get_value()
{
	return this->f_value;
}

coordinate Piece::get_coord()
{
	return f_coord;
}

bool Piece::is_alive()
{
	return f_alive;
}

void Piece::set_coord(coordinate coord)
{
	f_coord = coord;
}

void Piece::set_alive(bool val)
{
	f_alive = val;
}

std::string Piece::coord_to_notation(coordinate coord)
{
	std::string pos = "";
	pos[0] = 'a' + coord.column;
	pos[1] = 8 - coord.row + '0';
	return pos;
}

coordinate Piece::notation_to_coord(std::string pos)
{
	coordinate coord;

	coord.row = 8 - (pos[1] - '0');
	coord.column = pos[0] - 'a';

	return coord;
}

bool Piece::is_way_blocked(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
{
	int d1 = from.row - to.row,
		d2 = from.column - to.column;
	
	if (d1 > 0)
		d1 = -1;
	else if (d1 < 0)
		d1 = 1;

	if (d2 > 0)
		d2 = -1;
	else if (d2 < 0)
		d2 = 1;

	if (d1 > 0)
		to.row -= d1;
	else
		to.row += abs(d1);

	if (d2 > 0)
		to.column -= d2;
	else
		to.column += abs(d2);


	while (from.row != to.row || from.column != to.column)
	{
		from.row += d1;
		from.column += d2;
		if (board[from.row][from.column] != nullptr)
			return false;
	}

	return true;
}

pieces::King::King() : Piece(e_type::King, white, 0, "a1")
{
	f_moved = false;
	f_being_checked = false;
}

pieces::King::King(e_color color, std::string pos) :
	Piece(e_type::King, color, 0, pos)
{
	f_moved = false;
	f_being_checked = false;
}

bool pieces::King::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	int d1 = abs(from.row - to.row),
		d2 = abs(from.column - to.column);
	if (d1 < 2 && d2 < 2)
		return true;
	int d_castling = from.column - to.column;
	if (f_moved == false && d1 == 0)
	{
		if (d_castling == 2 && board[from.row][0] != nullptr)
		{
			if (board[from.row][0]->get_type() == e_type::Rook && !board[from.row][0]->is_moved())
				return is_way_blocked(from, coordinate{ from.row, 0 }, board);
		}
		else if (d_castling == -2 && board[from.row][7] != nullptr)
		{
			if (board[from.row][7]->get_type() == e_type::Rook && !board[from.row][7]->is_moved())
				return is_way_blocked(from, coordinate{ from.row, 7 }, board);
		}
	}
	return false;
}

bool pieces::King::is_moved()
{
	return this->f_moved;
}

bool pieces::King::is_being_checked()
{
	return this->f_being_checked;
}

void pieces::King::set_movement(bool val)
{
	this->f_moved = val;
}

void pieces::King::set_checking(bool val)
{
	this->f_being_checked = val;
}

pieces::Rook::Rook() :
	Piece(e_type::Rook, white, 4, "a1")
{
	f_moved = false;
}

pieces::Rook::Rook(e_color color, std::string pos) :
	Piece(e_type::Rook, color, 4, pos)
{
	f_moved = false;
}

bool pieces::Rook::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	if (from.row != to.row && from.column != to.column)
		return false;

	return is_way_blocked(from, to, board);
}

bool pieces::Rook::is_moved()
{
	return this->f_moved;
}

void pieces::Rook::set_movement(bool val)
{
	this->f_moved = val;
}

pieces::Queen::Queen() :
	Piece(e_type::Queen, white, 9, "a1") {}

pieces::Queen::Queen(e_color color, std::string pos) :
	Piece(e_type::Queen, color, 9, pos) {}

bool pieces::Queen::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	if (abs(from.row - to.row) != abs(from.column - to.column) &&
		(from.row != to.row && from.column != to.column))
		return false;
	return is_way_blocked(from, to, board);
}

pieces::Bishop::Bishop() :
	Piece(e_type::Bishop, white, 3, "a1") {}

pieces::Bishop::Bishop(e_color color, std::string pos) :
	Piece(e_type::Bishop, color, 3, pos) {}

bool pieces::Bishop::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	if (abs(from.row - to.row) != abs(from.column - to.column))
		return false;
	return is_way_blocked(from, to, board);
}

pieces::Knight::Knight() :
	Piece(e_type::Knight, white, 3, "a1") {}

pieces::Knight::Knight(e_color color, std::string pos) :
	Piece(e_type::Knight, color, 3, pos) {}

bool pieces::Knight::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	int d1 = abs(from.row - to.row),
		d2 = abs(from.column - to.column);

	if ((d1 == 2 && d2 == 1) || (d1 == 1 && d2 == 2))
		return true;
	return false;
}

pieces::Pawn::Pawn() :
	Piece(e_type::Pawn, white, 1, "a1")
{
	f_promoting = false;
}

pieces::Pawn::Pawn(e_color color, std::string pos) :
	Piece(e_type::Pawn, color, 1, pos)
{
	f_promoting = false;
}

bool pieces::Pawn::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	int d1 = from.row - to.row,
		d2 = from.column - to.column;

	if (this->get_color() == white)
	{
		if (d1 < 0)
			return false;
		if (abs(d1) == 2 && from.row != 6)
			return false;

	}
	if (this->get_color() == black)
	{
		if (d1 > 0)
			return false;
		if (abs(d1) == 2 && from.row != 1)
			return false;
		
	}
	if (abs(d1) > 2 || abs(d2) > 1)
		return false;
	if (abs(d1) != 1 && abs(d2) != 0)
		return false;
	if (abs(d2) != 0 && board[to.row][to.column] == nullptr)
	{
		move sutable_move = { {to.row - d1, to.column}, {to.row + d1, to.column} };
		if (last_move != sutable_move ||
			last_move.to.row == -1 ||
			last_move.to.column == -1 ||
			board[last_move.to.row][last_move.to.column]->get_type() != e_type::Pawn)
			return false;
	}
	return true;
}

bool pieces::Pawn::is_promoting()
{
	return this->f_promoting;
}

void pieces::Pawn::set_promoting(bool val)
{
	this->f_promoting = val;
}
