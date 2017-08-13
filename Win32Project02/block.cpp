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
void BasicBlock::clear()
{
	if (!isCleared) isCleared = true;
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
void HardBlock::clear()
{
	if (count > 0) --count;
	if (count == 0) isCleared = true;
}