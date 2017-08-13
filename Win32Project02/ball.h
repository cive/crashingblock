#pragma once

#include <Windows.h>
#include <memory>

typedef struct Velocity
{
	INT_PTR x;
	INT_PTR y;
}Velocity;

typedef struct Position
{
	UINT16 x;
	UINT16 y;
}Position;
typedef std::unique_ptr<Velocity> lpVelocity;
typedef std::unique_ptr<Position> lpPosition;