#pragma once

#include <string>
#include <vector>

//typedef std::vector<std::vector<Piece*>> board_matr_type;

enum e_type { King = 0, Queen = 1, Bishop = 2, Knight = 3, Rook = 4, Pawn = 5 };
enum e_color { white = 0, black = 1 };

struct coordinate {
	int row;
	int column;
};

class Piece {
public:
	Piece();
	Piece(e_type type, e_color color, int value);

	e_type get_type();
	e_color get_color();
	int get_value();

	virtual bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) { return false; }

	virtual bool is_moved() { return false; }
	virtual bool is_being_checked() { return false; }
	virtual bool is_promoting() { return false; }

	virtual void set_movement(bool val) {};
	virtual void set_checking(bool val) {};
	virtual void set_promoting(bool val) {};

protected:
	bool is_way_blocked(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board);

private:
	e_type f_type;
	e_color f_color;
	int f_value;
};

namespace pieces {
	class King : public Piece {
	public:
		King();
		King(e_color color);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;

		bool is_moved() override;
		bool is_being_checked() override;
		
		void set_movement(bool val) override;
		void set_checking(bool val) override;
	private:
		bool f_moved;
		bool f_being_checked;
	};

	class Rook : public Piece {
	public:
		Rook();
		Rook(e_color color);
	
		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;

		bool is_moved() override;

		void set_movement(bool val) override;
	private:
		bool f_moved;
	};

	class Queen : public Piece {
	public:
		Queen();
		Queen(e_color color);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;
	};

	class Bishop : public Piece { 
	public:
		Bishop();
		Bishop(e_color color);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;
	};

	class Knight : public Piece {
	public:
		Knight();
		Knight(e_color color);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;
	};

	class Pawn : public Piece {
	public:
		Pawn();
		Pawn(e_color color);

		bool movable(coordinate from, coordinate to, const std::vector<std::vector<Piece*>> board) override;

		bool is_promoting() override;
		void set_promoting(bool val) override;
	private:
		bool f_promoting;
	};
}