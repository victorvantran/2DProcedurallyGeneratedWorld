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
	Sand = 4,

	ElmBark = 5,
	ElmLeaves = 6,
	MapleBark = 7,
	MapleLeaves = 8,

	Torch = 9,

	Oak_Wood,
	Birch_Wood,
	Aspen_Wood,
	Redwood_Wood,
	Maple_Wood,
	Evergreen_Wood,


	count


	/*
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
	*/
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
	// const static std::map<std::uint8_t, std::uint8_t> _borderValueToDecalIndex;

	TileIdentity _id;
	bool _opaque;

	std::uint8_t _borders;


	const static std::uint8_t _borderValueToDecalIndex[256];

	/*
	std::uint8_t _borderValueToDecalIndex[256] = {
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		38, 38, 37, 37, 38, 38, 37, 37, 40, 40, 41, 41, 40, 40, 39, 39,
		38, 38, 37, 37, 38, 38, 37, 37, 35, 35, 36, 36, 35, 35, 34, 34,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		38, 38, 37, 37, 38, 38, 37, 37, 40, 40, 41, 41, 40, 40, 39, 39,
		38, 38, 37, 37, 38, 38, 37, 37, 35, 35, 36, 36, 35, 35, 34, 34,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		20, 20, 18, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		28, 28, 27, 26, 28, 28, 27, 26, 31, 31, 33, 32, 31, 31, 30, 29,
		28, 28, 27, 26, 28, 28, 27, 26, 23, 23, 25, 24, 23, 23, 22, 21,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		7 , 7 , 6 , 5 , 7 , 7 , 6 , 5 , 10, 10, 12, 11, 10, 10, 9 , 8 ,
		7 , 7 , 6 , 5 , 7 , 7 , 6 , 5 , 2 , 2 , 4 , 3 , 2 , 2 , 1 , 0
	};
	*/
	//TileType _tileType;
	// bool isDynamic;

public:
	Tile();
	~Tile();

	Tile( TileIdentity id, bool opaque );

	// Constructor that accounts for edges

	void setId( TileIdentity id );
	void setOpaque( bool opaque );

	void setBorders( std::uint8_t borders );
	void setBorder( TileBorder border );
	void unsetBorder( TileBorder border );


	TileIdentity getId() const;
	bool getOpaque() const;
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

