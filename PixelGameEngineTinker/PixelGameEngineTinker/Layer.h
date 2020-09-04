#pragma once

#include "olcPixelGameEngine.h"
#include "Tile.h"
#include "Atlas.h"

namespace olc
{
	template <typename T>
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


// [!] Template class implementation need to be visible to all translation units that use them. Move the implementations in the .cpp to the header file. No need for a .cpp file

namespace olc {
	template<typename T>
	Layer<T>::Layer()
	{

	}

	template<typename T>
	Layer<T>::~Layer()
	{

	}


	template<typename T>
	void Layer<T>::create( vi2d layerDimension, vi2d tileResolution )
	{
	// Establishes essentially a blank layer canvas given the layer dimension of tiles [col, row] and tile resolution
	// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the tile resolution)
		this->_layerDimension = layerDimension;
		this->_tileDimension = tileResolution;

		this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
		for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
		{
			this->_pTiles[i].id = i;
			this->_pTiles[i].exist = true;
		}

		return;
	}


	template<typename T>
	void Layer<T>::create( int layerX, int layerY, int tileX, int tileY )
	{
	// Establishes essentially a blank layer canvas given the layer dimension of tiles [col, row] and tile resolution
	// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the tile resolution)
		this->_layerDimension = vi2d{ layerX, layerY };
		this->_tileDimension = vi2d{ tileX, tileY };

		this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
		for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
		{
			this->_pTiles[i].id = i;
			this->_pTiles[i].exist = true;
		}

		return;
	}


	template<typename T>
	rcode Layer<T>::loadFromFile( std::string filename )
	{
	//
		return FAIL;
	}


	template<typename T>
	rcode Layer<T>::saveToFile( std::string filename )
	{
	//
		return FAIL;
	}


	template<typename T>
	T* Layer<T>::getTile( int x, int y )
	{
	// returns a pointer to a tile given a indices [col, row] on the layer's matrix map of tiles
		if ( x < 0 || x >= this->_layerDimension.x || y < 0 || y >= this->_layerDimension.y )
		{
			return nullptr;
		}
		else
		{
			// transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
			return &this->_pTiles[y*this->_layerDimension.x + x];
		}
	}


	template<typename T>
	T* Layer<T>::getTile( vi2d indicies )
	{
	// returns a pointer to a tile given a indicies [col, row] on the layer's matrix map of tiles
		if ( indicies.x < 0 || indicies.x >= this->_layerDimension.x || indicies.y < 0 || indicies.y >= this->_layerDimension.y )
		{
			return nullptr;
		}
		else
		{
			// transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
			return &this->_pTiles[indicies.y * this->_layerDimension.x + indicies.x];
		}
	}


	template<typename T>
	vi2d Layer<T>::getLayerDimension()
	{
	// returns the dimension of the layer matrix
		return this->_layerDimension;
	}


	template<typename T>
	vi2d Layer<T>::getTileDimension()
	{
	// returns the dimension of the tile
		return this->_tileDimension;
	}
}
