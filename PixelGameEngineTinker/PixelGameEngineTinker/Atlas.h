#pragma once

#include "olcPixelGameEngine.h"


class Atlas
{
private:
	olc::Sprite *_spriteTileSheet;
	olc::Decal* _decalTileSheet;
	olc::vi2d _atlasDimension;
	olc::vi2d _tileDimension;

public:
	std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> mapping;

	Atlas();
	~Atlas();

	Atlas( olc::Sprite* spriteSheet, olc::Decal* decalSheet, olc::vi2d atlasDimension, olc::vi2d tileDimension );

	void create( std::string sheetPath );
	void create( olc::Sprite *spriteSheet);
	void create( olc::Decal *decalSheet );
	void create( olc::Sprite *spriteSheet, olc::Decal* decalSheet );

	void fillMapping();

	olc::rcode loadFromFile(std::string filename);
	olc::rcode saveToFile(std::string filename);

	olc::Sprite* getSpriteTileSheet();
	olc::Decal* getDecalTileSheet();

	olc::vi2d getAtlasDimension();
	olc::vi2d getTileDimension();

	std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> getMapping();
};
