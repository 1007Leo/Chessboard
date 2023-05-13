#pragma once

#include <string>
#include <vector>
#include <list>

enum e_type { King = 0, Queen = 1, Bishop = 2, Knight = 3, Rook = 4, Pawn = 5 };
enum e_color { white = 0, black = 1 };

int sgn(int val);

struct coordinate {
	int row;
	int column;

	friend bool operator==(const coordinate& l, const coordinate& r) {
		return ((l.row == r.row) && (l.column == r.column));
	}
	friend bool operator!=(const coordinate& l, const coordinate& r) {
		return ((l.row != r.row) || (l.column != r.column));
	}
};

struct move {
	coordinate from;
	coordinate to;

	friend bool operator==(const move& l, const move& r) {
		return ((l.from == r.from) && (l.to == r.to));
	}
	friend bool operator!=(const move& l, const move& r) {
		return ((l.from != r.from) || (l.to != r.to));
	}
};

class Piece {
public:
	Piece();
	Piece(e_type type, e_color color, int value, std::string pos);

	e_type get_type();
	e_color get_color();
	int get_value();
	coordinate get_coord();
	bool is_alive();

	void set_coord(coordinate coord);
	void set_alive(bool val);

	static std::string coord_to_notation(coordinate coord);
	static coordinate notation_to_coord(std::string pos);

	virtual bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) { return false; }
	virtual bool is_checked_scan(const std::vector<std::vector<Piece*>> board) { return false; };
	virtual std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) { return std::list<coordinate>(); }

	virtual bool is_moved() { return false; }
	virtual bool is_being_checked() { return false; }
	virtual bool is_promoting() { return false; }

	virtual void set_movement(bool val) {};
	virtual void set_checking(bool val) {};
	virtual void set_promoting(bool val) {};

protected:
	bool is_way_blocked(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board);
	std::list<coordinate> get_all_moves_in_direction(int row_d, int col_d, const std::vector<std::vector<Piece*>> board);

private:
	e_type f_type;
	e_color f_color;
	int f_value;
	coordinate f_coord;
	bool f_alive;
};

namespace pieces {
	class King : public Piece {
	public:
		King();
		King(e_color color, std::string notation);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;
		bool is_checked_scan(const std::vector<std::vector<Piece*>> board) override;

		bool is_moved() override;
		bool is_being_checked() override;
		
		void set_movement(bool val) override;
		void set_checking(bool val) override;
	private:
		bool is_checked_from_direction(int row_d, int col_d, const std::vector<std::vector<Piece*>> board);
		bool is_checked_by_knight(const std::vector<std::vector<Piece*>> board);

		bool f_moved;
		bool f_is_being_checked;
	};

	class Rook : public Piece {
	public:
		Rook();
		Rook(e_color color, std::string pos);
	
		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;

		bool is_moved() override;

		void set_movement(bool val) override;
	private:
		bool f_moved;
	};

	class Queen : public Piece {
	public:
		Queen();
		Queen(e_color color, std::string pos);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;
	};

	class Bishop : public Piece { 
	public:
		Bishop();
		Bishop(e_color color, std::string pos);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;
	};

	class Knight : public Piece {
	public:
		Knight();
		Knight(e_color color, std::string pos);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;
	};

	class Pawn : public Piece {
	public:
		Pawn();
		Pawn(e_color color, std::string pos);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board, move last_move) override;
		std::list<coordinate> get_all_moves(const std::vector<std::vector<Piece*>> board, move last_move) override;

		bool is_promoting() override;
		void set_promoting(bool val) override;
	private:
		bool f_promoting;
	};
}