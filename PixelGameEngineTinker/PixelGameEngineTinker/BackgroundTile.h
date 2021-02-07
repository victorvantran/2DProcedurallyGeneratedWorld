#pragma once
#include <cstdint>
#include <map>



enum class BackgroundTileBorder
{
	NorthWest = 0,
	North = 1,
	NorthEast = 2,
	East = 3,
	SouthEast = 4,
	South = 5,
	SouthWest = 6,
	West = 7
};

enum class BackgroundTileIdentity // uint64_t MAX
{

	Void = 0,
	Wood = 1,

	count
};


enum class BackgroundTileType
{
	Static = 0,
	Block = 1,
	OneWay = 2,
	count
	// liquid
	// climbable
	// Triangle
};

class BackgroundTile
{
private:
	// const static std::map<std::uint8_t, std::uint8_t> _borderValueToDecalIndex;

	BackgroundTileIdentity _id;
	BackgroundTileType _type;

	std::uint8_t _borders;

	static constexpr std::uint8_t _borderValueToDecalIndex[256] = {
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


public:
	BackgroundTile();
	~BackgroundTile();

	BackgroundTile( BackgroundTileIdentity id, BackgroundTileType type );

	// Constructor that accounts for edges

	void setId( BackgroundTileIdentity id );
	void setType( BackgroundTileType type );

	void setBorders( std::uint8_t borders );
	void setBorder( BackgroundTileBorder border );
	void unsetBorder( BackgroundTileBorder border );


	BackgroundTileIdentity getId() const;
	BackgroundTileType getType() const;
	std::uint8_t getBorders() const;
	bool getBorder( BackgroundTileBorder border ) const;
	std::uint8_t getBordersDecalIndex() const;

	bool exists() const;

	bool isVoid() const;
	bool isStatic() const;
	
	void clear();
};

