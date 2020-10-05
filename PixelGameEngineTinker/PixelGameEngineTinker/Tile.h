#pragma once
#include <stdint.h>

enum class TileName // uint64_t MAX
{
	Void = 0,
	Water = 1,
	Stone = 2,
	Dirt = 3,
	Sand = 4,
	Gravel = 5,
	Clay = 6,




	Oak_Wood,
	Birch_Wood,
	Aspen_Wood,
	Redwood_Wood,
	Maple_Wood,
	Evergreen_Wood,



	count
};


enum class TileTypez
{
	Empty = 0,
	Block = 1,
	OneWayPlatform = 2,
	count
	// liquid
	// climbable
	// Triangle
};

class Tile
{
private:
	uint64_t _id;

	int _configuration;
	//TileType _tileType;
	// bool isDynamic;

public:
	Tile();
	~Tile();

	Tile( uint64_t id );

	// Constructor that accounts for edges

	void setId( uint64_t id );
	int setConfiguration( int configuration );

	uint64_t getId() const;
	int getConfiguration() const;

	bool isVoid() const;

	bool isEmpty() const;
	bool isBlock() const;
	bool isOneWayPlatform() const;

	bool isObstacle() const;
	bool isGround() const;
	bool isLedge() const;

	void clear();
};

