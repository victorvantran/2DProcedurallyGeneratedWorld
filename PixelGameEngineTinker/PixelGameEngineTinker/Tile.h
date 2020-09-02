#pragma once

#include "olcPixelGameEngine.h"


enum class TILE_TYPE
{
	BASIC,
};

struct Tile
{
	int id = 0;
	bool exist = true;
	TILE_TYPE tileType = TILE_TYPE::BASIC;

	int edgeId[4];
	int edgeExist[4];


	Tile();
	~Tile();
};