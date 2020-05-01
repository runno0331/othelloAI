#pragma once

#include <tuple>
#include <random>

#include "board.hpp"

namespace reversi
{

class Player
{
public:
	Player(Side side) : m_side(side) {}
	virtual ~Player() = default;

	Side getSide() const
	{
		return m_side;
	}

	// make random seed generator;
	std::random_device rnd;

	virtual CellPosition thinkNextMove(const Board& board) = 0;

private:
	Side m_side;
};

}  // namespace reversi
