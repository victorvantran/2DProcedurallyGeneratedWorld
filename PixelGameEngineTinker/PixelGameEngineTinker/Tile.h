#pragma once

#include "olcPixelGameEngine.h"
#include "TileConsolidated.h"

enum class TileType
{
	Empty = 0,
	Block = 1,
	OneWayPlatform = 2,
	count
	// liquid
	// climbable
	// Triangle
};

class Tile : public TileConsolidated
{
private:
	int _configuration;
	TileType _tileType;
	// bool isDynamic;

	int _edgeId[4];
	bool _edgeExist[4];

public:
	Tile();
	~Tile();

	Tile( int id, const BoundingBox<int>& bounds, bool exist, int configuration, TileType tileType );

	// Constructor that accounts for edges

	int getConfiguration() const;

	int setConfiguration( int configuration );

	bool isEmpty() const;
	bool isBlock() const;
	bool isOneWayPlatform() const;

	bool isObstacle() const;
	bool isGround() const;
	bool isLedge() const;



	void clear();
};

