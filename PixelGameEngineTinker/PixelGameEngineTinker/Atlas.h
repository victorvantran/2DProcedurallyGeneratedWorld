#pragma once

#include "olcPixelGameEngine.h"

namespace olc
{
	class Atlas
	{
	private:
		Sprite *_spriteTileSheet;

	public:
		std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> mapping;

		Atlas();
		~Atlas();

		void create(Sprite *tileSheet);
		rcode loadFromFile(std::string filename);
		rcode saveToFile(std::string filename);

		Sprite* getTileSheet();
		std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> getMapping();
	};
}