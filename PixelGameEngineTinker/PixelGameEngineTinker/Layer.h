#pragma once

#include "olcPixelGameEngine.h"

#include "Tile.h"
#include "Atlas.h"

namespace olc
{
	template <class T>
	class Layer : public olc::PGEX
	{
	private:
		vi2d _layerDimension;
		vi2d _tileDimension;

		Atlas _atlas;
		T* _pTiles;
		
	public:
		Layer();
		~Layer();
		
		void create( vi2d layerSize, vi2d tileSize );
		void create(int layerWidth, int layerHeight, int tileWidth, int tileHeight);

		rcode loadFromFile(std::string filename);
		rcode saveToFile(std::string filename);

		T* getTile(int x, int y);
		T* getTile(vi2d position);

		vi2d getLayerDimension();
		vi2d getTileDimension();



	};



}