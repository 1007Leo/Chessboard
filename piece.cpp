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
	int row_d = -sgn(from.row - to.row),
		col_d = -sgn(from.column - to.column);

	if (row_d > 0)
		to.row -= row_d;
	else
		to.row += abs(row_d);

	if (col_d > 0)
		to.column -= col_d;
	else
		to.column += abs(col_d);


	while (from.row != to.row || from.column != to.column)
	{
		from.row += row_d;
		from.column += col_d;
		if (board[from.row][from.column] != nullptr)
			return false;
	}

	return true;
}

std::list<coordinate> Piece::get_all_moves_in_direction(int row_d, int col_d, const std::vector<std::vector<Piece*>> board)
{
	std::list<coordinate> res;
	if (row_d == 0 && col_d == 0)
		return res;
	coordinate cur_pos = this->get_coord();
	e_color this_color = this->get_color();
	cur_pos.row += row_d;
	cur_pos.column += col_d;

	while (cur_pos.row >= 0 && cur_pos.row <= 7 &&
		cur_pos.column >= 0 && cur_pos.column <= 7 &&
		(board[cur_pos.row][cur_pos.column] == nullptr || board[cur_pos.row][cur_pos.column]->get_color() != this_color))
	{
		res.push_back(cur_pos);
		cur_pos.row += row_d;
		cur_pos.column += col_d;
	}
	return res;
}

pieces::King::King() : Piece(e_type::King, white, 0, "a1")
{
	f_moved = false;
	f_is_being_checked = false;
}

pieces::King::King(e_color color, std::string pos) :
	Piece(e_type::King, color, 0, pos)
{
	f_moved = false;
	f_is_being_checked = false;
}

bool pieces::King::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	int row_d = abs(from.row - to.row),
		col_d = abs(from.column - to.column);
	if (row_d < 2 && col_d < 2)
		return true;
	int d_castling = from.column - to.column;
	if (f_moved == false && row_d == 0)
	{
		if (d_castling == 2 && board[from.row][0] != nullptr)
		{
			if (board[from.row][0]->get_type() == e_type::Rook && !board[from.row][0]->is_moved() && !is_being_checked())
				return is_way_blocked(from, coordinate{ from.row, 0 }, board);
		}
		else if (d_castling == -2 && board[from.row][7] != nullptr)
		{

			if (board[from.row][7]->get_type() == e_type::Rook && !board[from.row][7]->is_moved() && !is_being_checked())
				return is_way_blocked(from, coordinate{ from.row, 7 }, board);
		}
	}
	return false;
}

std::list<coordinate> pieces::King::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	std::list<coordinate> available_moves;
	coordinate from = this->get_coord();
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			coordinate to = { from.row + i, from.column + j };
			if (to.row >= 0 && to.row <= 7 &&
				to.column >= 0 && to.column <= 7)
				if (this->movable(from, to, board, last_move))
					available_moves.push_back(to);
		}
		
	coordinate to = { from.row, from.column + 2 };
	if (this->movable(from, to, board, last_move))
		available_moves.push_back(to);
	to = { from.row, from.column - 2 };
	if (this->movable(from, to, board, last_move))
		available_moves.push_back(to);

	return available_moves;
}

bool pieces::King::is_checked_scan(const std::vector<std::vector<Piece*>> board)
{
	for (int row_d = -1; row_d <= 1; row_d++)
		for (int col_d = -1; col_d <= 1; col_d++)
			if (is_checked_from_direction(row_d, col_d, board))
				return true;
	return false;
}

bool pieces::King::is_moved()
{
	return this->f_moved;
}

bool pieces::King::is_being_checked()
{
	return this->f_is_being_checked;
}

void pieces::King::set_movement(bool val)
{
	this->f_moved = val;
}

void pieces::King::set_checking(bool val)
{
	this->f_is_being_checked = val;
}

bool pieces::King::is_checked_from_direction(int row_d, int col_d, const std::vector<std::vector<Piece*>> board)
{
	if (row_d == 0 && col_d == 0)
		return is_checked_by_knight(board);

	coordinate cur_pos = this->get_coord();
	e_color this_color = this->get_color();

	cur_pos.row += row_d;
	cur_pos.column += col_d;

	int i = 0;

	while (cur_pos.row >= 0 && cur_pos.row <= 7 &&
		cur_pos.column >= 0 && cur_pos.column <= 7)
	{
		Piece* cur_piece = board[cur_pos.row][cur_pos.column];
		if (cur_piece != nullptr)
		{
			if (i == 0)
			{
				if (cur_piece->get_color() != this_color &&
					cur_piece->get_type() == e_type::King)
				{
					return true;
				}
				if (cur_piece->get_color() != this_color &&
					cur_piece->get_type() == e_type::Pawn)
				{
					if (this_color == white && ((row_d == -1 && col_d == 1) || (row_d == -1 && col_d == -1)))
						return true;
					if (this_color == black && ((row_d == 1 && col_d == 1) || (row_d == 1 && col_d == -1)))
						return true;
				}
			}

			if (row_d == 0 || col_d == 0)
			{
				if (cur_piece->get_color() != this_color &&
					(cur_piece->get_type() == e_type::Rook || cur_piece->get_type() == e_type::Queen))
				{
					return true;
				}
				return false;
			}
			else
			{
				if (cur_piece->get_color() != this_color &&
					(cur_piece->get_type() == e_type::Bishop || cur_piece->get_type() == e_type::Queen))
				{
					return true;
				}
				return false;
			}
		}
		
		cur_pos.row += row_d;
		cur_pos.column += col_d;
		i++;
	}
	return false;
}

bool pieces::King::is_checked_by_knight(const std::vector<std::vector<Piece*>> board)
{
	coordinate king_pos = this->get_coord();
	e_color enemy_color = (e_color)((this->get_color() + 1) % 2);
	std::vector<coordinate> coords{ {king_pos.row - 2, king_pos.column + 1}, {king_pos.row - 1, king_pos.column + 2},
									{king_pos.row + 1, king_pos.column + 2}, {king_pos.row + 2, king_pos.column + 1},
									{king_pos.row + 2, king_pos.column - 1}, {king_pos.row + 1, king_pos.column - 2},
									{king_pos.row - 1, king_pos.column - 2}, {king_pos.row - 2, king_pos.column - 1} };
	for (auto coord : coords)
	{
		if (coord.row >= 0 && coord.row <= 7 && coord.column >= 0 && coord.column <= 7)
		{
			Piece* piece = board[coord.row][coord.column];
			if (piece != nullptr && piece->get_color() == enemy_color && piece->get_type() == e_type::Knight)
				return true;
		}
	}
	return false;
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

std::list<coordinate> pieces::Rook::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	std::list<coordinate> res;

	for (auto coord : get_all_moves_in_direction(1, 0, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(-1, 0, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(0, 1, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(0, -1, board))
		res.push_back(coord);

	return res;
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

std::list<coordinate> pieces::Queen::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	std::list<coordinate> res;

	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
			for (auto coord : get_all_moves_in_direction(i, j, board))
				res.push_back(coord);

	return res;
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

std::list<coordinate> pieces::Bishop::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	std::list<coordinate> res;

	for (auto coord : get_all_moves_in_direction(1, 1, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(-1, -1, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(-1, 1, board))
		res.push_back(coord);
	for (auto coord : get_all_moves_in_direction(1, -1, board))
		res.push_back(coord);

	return res;
}

pieces::Knight::Knight() :
	Piece(e_type::Knight, white, 3, "a1") {}

pieces::Knight::Knight(e_color color, std::string pos) :
	Piece(e_type::Knight, color, 3, pos) {}

bool pieces::Knight::movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move)
{
	int row_d = abs(from.row - to.row),
		col_d = abs(from.column - to.column);

	if ((row_d == 2 && col_d == 1) || (row_d == 1 && col_d == 2))
		return true;
	return false;
}

std::list<coordinate> pieces::Knight::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	coordinate cur_pos = this->get_coord();
	std::list<coordinate> res;
	std::vector<coordinate> coords{ {cur_pos.row - 2, cur_pos.column + 1}, {cur_pos.row - 1, cur_pos.column + 2},
									{cur_pos.row + 1, cur_pos.column + 2}, {cur_pos.row + 2, cur_pos.column + 1},
									{cur_pos.row + 2, cur_pos.column - 1}, {cur_pos.row + 1, cur_pos.column - 2},
									{cur_pos.row - 1, cur_pos.column - 2}, {cur_pos.row - 2, cur_pos.column - 1} };
	for (auto coord : coords)
		if (coord.row >= 0 && coord.row <= 7 && coord.column >= 0 && coord.column <= 7)
			res.push_back(coord);
	return res;
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
	int row_d = from.row - to.row,
		col_d = from.column - to.column;

	if (this->get_color() == white)
	{
		if (row_d < 0)
			return false;
		if (abs(row_d) == 2 && from.row != 6)
			return false;

	}
	if (this->get_color() == black)
	{
		if (row_d > 0)
			return false;
		if (abs(row_d) == 2 && from.row != 1)
			return false;
		
	}
	if (abs(row_d) > 2 || abs(col_d) > 1)
		return false;
	if (abs(row_d) == 2)
	{
		int tmp_delta = - abs(row_d) / row_d;
		if (abs(col_d) != 0)
			return false;
		if (board[from.row + tmp_delta][from.column] != nullptr)
			return false;

	}
	if (board[to.row][to.column] != nullptr && col_d == 0)
		return false;
	if (abs(col_d) != 0 && board[to.row][to.column] == nullptr)
	{
		move sutable_move = { {to.row - row_d, to.column}, {to.row + row_d, to.column} };
		if (last_move != sutable_move ||
			last_move.to.row == -1 ||
			last_move.to.column == -1 ||
			board[last_move.to.row][last_move.to.column]->get_type() != e_type::Pawn)
			return false;
	}
	return true;
}

std::list<coordinate> pieces::Pawn::get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move)
{
	coordinate cur_pos = this->get_coord();
	int row_d;
	if (this->get_color() == white)
		row_d = -1;
	else
		row_d = 1;
	std::vector<coordinate> moves = { {cur_pos.row + row_d, cur_pos.column},
									  {cur_pos.row + 2 * row_d, cur_pos.column},
									  {cur_pos.row + row_d, cur_pos.column + 1},
									  {cur_pos.row + row_d, cur_pos.column - 1} };

	std::list<coordinate> available_moves;
	for (auto move : moves)
		if (move.row >= 0 && move.row <= 7 &&
			move.column >= 0 && move.column <= 7)
			if (movable(cur_pos, move, board, last_move))
				available_moves.push_back(move);

	return available_moves;
}

bool pieces::Pawn::is_promoting()
{
	return this->f_promoting;
}

void pieces::Pawn::set_promoting(bool val)
{
	this->f_promoting = val;
}

int sgn(int val)
{
	return (0 < val) - (val < 0);
}
