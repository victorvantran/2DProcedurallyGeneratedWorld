#pragma once

#include "olcPixelGameEngine.h"


enum class TileType
{
	Empty = 0,
	Block = 1,
	OneWayPlatform = 2,
	count
};

struct Tile
{
	int id = 0;
	bool exist = true;
	// bool isDynamic;
	TileType tileType;

	int edgeId[4];
	int edgeExist[4];


	Tile();
	~Tile();

	bool isEmpty();
	bool isBlock();
	bool isOneWayPlatform();
	bool isGround();

	bool exists();
};

