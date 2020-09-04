#pragma once

#include "olcPixelGameEngine.h"

namespace olc
{
	class Atlas
	{
	private:
		Sprite *_spriteTileSheet;
		Decal* _decalTileSheet;

	public:
		std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> mapping;

		Atlas();
		~Atlas();

		void create( std::string sheetPath );
		void create(Sprite *spriteSheet);
		void create( Decal *decalSheet );
		void create( Sprite *spriteSheet, Decal* decalSheet );

		rcode loadFromFile(std::string filename);
		rcode saveToFile(std::string filename);

		Sprite* getSpriteTileSheet();
		Decal* getDecalTileSheet();

		std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> getMapping();
	};
}