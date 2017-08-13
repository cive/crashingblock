#pragma once
#include <memory>
#include "ball.h"
#include "config.h"

enum BlockType
{
	Basic,
	Hard
};

class BlockInterface
{
protected:
	int col_pos;
	int row_pos;
	bool isCleared = false;
public:
	POINT getPoint()
	{
		return POINT{ col_pos, row_pos };
	}
	virtual size_t getWidth() = 0;
	virtual size_t getHeight() = 0;
	virtual BlockType getType() = 0;
	virtual bool clear() = 0;
	bool hasCleared()
	{
		return isCleared;
	}
	bool isHashed(const lpPosition& ball_position)
	{
		bool isInsideX = ball_position->x > col_pos && ball_position->x < col_pos + getWidth() - BALL_SIZE;
		bool isInsideY = ball_position->y + BALL_SIZE > row_pos && ball_position->y < row_pos + getHeight();
		if (isInsideX && isInsideY) return true;
		return false;
	}
};

class BasicBlock : public BlockInterface
{
public:
	BasicBlock(int col_pos, int row_pos)
	{
		this->col_pos = col_pos * getWidth() + BLOCK_X_OFFSET;
		this->row_pos = row_pos * getHeight() + BLOCK_Y_OFFSET;
	}
	~BasicBlock() {};
	const size_t kstWidth  = 32;
	const size_t kstHeight = 12;
	size_t getWidth();
	size_t getHeight();
	BlockType getType();
	bool clear();
};

class HardBlock : public BlockInterface
{
private:
	int count = 3;
public:
	HardBlock(int col_pos, int row_pos)
	{
		this->col_pos = col_pos * getWidth() + BLOCK_X_OFFSET;
		this->row_pos = row_pos * getHeight() + BLOCK_Y_OFFSET;
	};
	~HardBlock() {};
	const size_t kstWidth  = 32;
	const size_t kstHeight = 12;
	size_t getWidth();
	size_t getHeight();
	BlockType getType();
	bool clear();
};

static class BlockFactory
{
public:
	static BlockInterface* getBlock(BlockType type, int col_pos, int row_pos)
	{
		switch (type)
		{
		case BlockType::Basic:
			return new BasicBlock(col_pos, row_pos);
		case BlockType::Hard:
			return new HardBlock(col_pos, row_pos);
		default:
			break;
		}
	}
};