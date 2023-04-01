#include "piece.hpp"

Piece::Piece()
{
	f_type = Pawn;
	f_color = white;
	f_value = 0;
}

Piece::Piece(e_type type, e_color color, int value)
{
	f_type = type;
	f_color = color;
	f_value = value;
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

pieces::King::King() : Piece(e_type::King, white, 0)
{
	f_moved = false;
	f_being_checked = false;
}

pieces::King::King(e_color color) :
	Piece(e_type::King, color, 0)
{
	f_moved = false;
	f_being_checked = false;
}

bool pieces::King::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
{
	int d1 = abs(from.row - to.row),
		d2 = abs(from.column - to.column);
	if (d1 < 2 && d2 < 2)
		return true;
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
	Piece(e_type::Rook, white, 4)
{
	f_moved = false;
}

pieces::Rook::Rook(e_color color) :
	Piece(e_type::Rook, color, 4)
{
	f_moved = false;
}

bool pieces::Rook::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
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
	Piece(e_type::Queen, white, 9) {}

pieces::Queen::Queen(e_color color) :
	Piece(e_type::Queen, color, 9) {}

bool pieces::Queen::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
{
	if (abs(from.row - to.row) != abs(from.column - to.column) &&
		(from.row != to.row && from.column != to.column))
		return false;
	return is_way_blocked(from, to, board);
}

pieces::Bishop::Bishop() :
	Piece(e_type::Bishop, white, 3) {}

pieces::Bishop::Bishop(e_color color) :
	Piece(e_type::Bishop, color, 3) {}

bool pieces::Bishop::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
{
	if (abs(from.row - to.row) != abs(from.column - to.column))
		return false;
	return is_way_blocked(from, to, board);
}

pieces::Knight::Knight() :
	Piece(e_type::Knight, white, 3) {}

pieces::Knight::Knight(e_color color) :
	Piece(e_type::Knight, color, 3) {}

bool pieces::Knight::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
{
	int d1 = abs(from.row - to.row),
		d2 = abs(from.column - to.column);

	if ((d1 == 2 && d2 == 1) || (d1 == 1 && d2 == 2))
		return true;
	return false;
}

pieces::Pawn::Pawn() :
	Piece(e_type::Pawn, white, 1) 
{
	f_promoting = false;
}

pieces::Pawn::Pawn(e_color color) :
	Piece(e_type::Pawn, color, 1) 
{
	f_promoting = false;
}

bool pieces::Pawn::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board)
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
		return false;
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
