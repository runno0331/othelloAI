#pragma once

#include <random>

#include "board.hpp"
#include "player.hpp"

namespace reversi
{

class SampleComputerPlayer : public Player
{
public:
	SampleComputerPlayer(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;
};

class RandomComputerPlayer : public Player
{
public:
	RandomComputerPlayer(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;
};

class ComputerPlayer1 : public Player
{
public:
	ComputerPlayer1(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	int max_depth;
	int evaluate(const Board& board);
	int ab_search(const Board& board, int depth, int alpha, int beta, int pass, int flag);
};

class ComputerPlayer2 : public Player
{
public:
	ComputerPlayer2(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	int max_depth;
	int evaluate(const Board& board);
	int ab_search(const Board& board, int depth, int alpha, int beta);
};

class MonteCarloPlayer1 : public Player
{
public:
	MonteCarloPlayer1(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	float max_total = 1000;
	bool playout(Board& board);
};

class MonteCarloPlayer2 : public Player
{
public:
	MonteCarloPlayer2(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	int max_total = 100;
	bool playout(Board& board);
};

class BitPlayer1 : public Player
{
public:
	BitPlayer1(Side side) : Player(side) {}

	virtual CellPosition thinkNextMove(const Board& board) override;

private:
	int max_depth;
	int bitcount(unsigned long board);
	unsigned long getAllLegalMoves(unsigned long myBoard, unsigned long oppBoard);
	unsigned long flippingStone(unsigned long myBoard, unsigned long oppBoard, unsigned long move);
	int evaluate(unsigned long myBoard, unsigned long oppBoard);
	int ab_search(unsigned long myBoard, unsigned long oppBoard, int depth, int alpha, int beta, int pass, bool flag);
};

}  // namespace reversi
