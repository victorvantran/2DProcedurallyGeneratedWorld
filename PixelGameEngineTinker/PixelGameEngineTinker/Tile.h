#pragma once
#include <cstdint>
#include <map>

#include "TileBorder.h"




enum class TileIdentity // uint64_t MAX
{
	Void = 0,
	Water = 1,
	Stone = 2,
	Dirt = 3,
	Torch = 4,
	RedTorch = 5,
	GreenTorch = 6,
	BlueTorth = 7,

	Sand = 8,
	Gravel = 9,
	Clay = 10,




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
	const static std::map<std::uint8_t, std::uint8_t> _borderValueToDecalIndex;

	std::uint64_t _id;
	std::uint8_t _borders;

	//TileType _tileType;
	// bool isDynamic;

public:
	Tile();
	~Tile();

	Tile( std::uint64_t id );
	Tile( TileIdentity identity );

	// Constructor that accounts for edges

	void setId( std::uint64_t id );
	void setIdentity( TileIdentity identity );

	void setBorders( std::uint8_t borders );
	void setBorder( TileBorder border );
	void unsetBorder( TileBorder border );


	std::uint64_t getId() const;
	TileIdentity getIdentity() const;
	std::uint8_t getBorders() const;
	bool getBorder( TileBorder border ) const;
	std::uint8_t getBordersDecalIndex() const;

	bool exists() const;

	bool isVoid() const;

	bool isEmpty() const;
	bool isBlock() const;
	bool isOneWayPlatform() const;

	bool isObstacle() const;
	bool isGround() const;
	bool isLedge() const;

	void clear();
};

