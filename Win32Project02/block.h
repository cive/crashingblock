#pragma once
#include <memory>
#include "ball.h"
#include "config.h"

enum BlockType
{
	Basic,
	Hard
};
/**
 * @brief Base class for block
 */
class BlockInterface
{
protected:
	int col_pos;
	int row_pos;
	bool isCleared = false;
public:
	/**
	 * @brief �u���b�N�̈ʒu��Ԃ��֐�
	 * @return POINT �u���b�N�̈ʒu
	 */
	POINT getPoint()
	{
		return POINT{ col_pos, row_pos };
	}
	virtual size_t getWidth() = 0;
	virtual size_t getHeight() = 0;
	virtual BlockType getType() = 0;
	/*! �u���b�N���{�[���ƏՓ˂����Ƃ��ɌĂяo���֐� */
	virtual void clear() = 0;
	bool hasCleared()
	{
		return isCleared;
	}
	/**
	 * @brief �Փ˔���֐�
	 * @param ball_position �{�[���̈ʒu
	 * @return �Փ˂��Ă�����true
	 */
	bool isHashed(const lpPosition& ball_position)
	{
		bool isInsideX = ball_position->x > col_pos && ball_position->x < col_pos + getWidth() - BALL_SIZE;
		bool isInsideY = ball_position->y + BALL_SIZE > row_pos && ball_position->y < row_pos + getHeight();
		if (isInsideX && isInsideY) return true;
		return false;
	}
};


/**
* @brief Basic block
*/
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
	void clear();
};

/**
* @brief Hard block
*/
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
	void clear();
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