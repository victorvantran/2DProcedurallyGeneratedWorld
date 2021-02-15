#pragma once
#include <cstdint>
#include <map>

#include "TileBorder.h"



/*
enum class TileIdentity
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

	MossDirt = 10,
	MossStone = 11,
	Mull = 12,
	Mor = 13,
	LightClay = 14,
	DarkClay = 15,
	Gravel = 16,
	Charcoal = 17,
	Snow = 18,
	Permafrost = 19,
	Podzol = 20,
	BleachedSand = 21,
	Shale = 22,
	IronOxide = 23,
	AluminiumOxide = 24,
	Laterite = 25,
	Aridsol = 26,
	Entisol = 27,
	Saltstone = 28,
	Quartz = 29,

	Alfisol = 30,
	YellowClay = 31,
	RedClay = 32,

	Cambisol = 33,
	Silt = 34,

	IronOre = 35,
	AluminiumOre = 36,


	MapleLog = 37,
	CrimsonMapleLeaves = 38,
	PumpkinMapleLeaves = 39,
	RajahMapleLeaves = 40,

	
	//Oak_Wood = 10,
	//Birch_Wood = 11,
	//Aspen_Wood = 12,
	//Redwood_Wood = 13,
	//Maple_Wood = 14,
	//Evergreen_Wood = 15,
	

	
	count

};
*/




enum class TileIdentity // uint64_t MAX
{

	Void,
	Water,
	Stone,
	Dirt,
	Sand,

	Torch,

	MossDirt,
	MossStone ,
	Mull,
	Mor,
	LightClay,
	DarkClay,
	Gravel,
	Charcoal,
	Snow,
	Permafrost,
	Podzol,
	BleachedSand,
	Shale,
	IronOxide,
	AluminiumOxide,
	Laterite,
	Aridsol,
	Entisol,
	Saltstone,
	Quartz,

	Alfisol,
	YellowClay,
	RedClay,

	Cambisol,
	Silt,

	IronOre,
	AluminiumOre,


	MapleLog,
	CrimsonMapleLeaves,
	PumpkinMapleLeaves,
	GambogeMapleLeaves,



	count

};


enum class TileType
{
	Empty = 0,
	Block = 1,
	OneWay = 2,
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
	TileType _type;
	bool _consolidatable;
	bool _opaque;
	bool _complementary;

	std::uint8_t _borders;
	std::uint8_t _tileBlobMapIndex;

public:
	Tile();
	~Tile();

	Tile( TileIdentity id, TileType type, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t _tileBlobMapIndex );

	// Constructor that accounts for edges

	void setId( TileIdentity id );
	void setType( TileType type );
	void setConsolidatable( bool consolidatable );
	void setOpaque( bool opaque );
	void setComplementary( bool complementary );
	void setTileBlobMapIndex( std::uint8_t tileBlobMapIndex );

	void setBorders( std::uint8_t borders );
	void setBorder( TileBorder border );
	void unsetBorder( TileBorder border );

	TileIdentity getId() const;
	TileType getType() const;
	bool getConsolidatable() const;
	bool getOpaque() const;
	bool getComplementary() const;
	std::uint8_t getBorders() const;
	bool getBorder( TileBorder border ) const;
	std::uint8_t getTileBlobMapIndex() const;

	bool exists() const;

	bool isVoid() const;
	bool isEmpty() const;
	bool isObstacle() const;
	bool isGround() const;
	bool isOneWayPlatform() const;
	bool isLedge() const;

	void clear();

};

