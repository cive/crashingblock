#include "block.h"


size_t BasicBlock::getWidth()
{
	return kstWidth;
}
size_t BasicBlock::getHeight()
{
	return kstHeight;
}
BlockType BasicBlock::getType()
{
	return BlockType::Basic;
}
bool BasicBlock::clear()
{
	if (!isCleared) isCleared = true;
	return isCleared;
}
size_t HardBlock::getWidth()
{
	return kstWidth;
}
size_t HardBlock::getHeight()
{
	return kstHeight;
}
BlockType HardBlock::getType()
{
	return BlockType::Basic;
}
bool HardBlock::clear()
{
	if (count > 0) --count;
	if (count == 0) isCleared = true;
	return isCleared;
}