#include "Layer.h"
/*
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
	void Layer<T>::test()
	{
		return;
	}


	template<typename T>
	void Layer<T>::create( vi2d layerDimension, vi2d tileDimension )
	{
	// Establishes essentially a blank layer canvas
		this->_layerDimension = layerDimension;
		this->_tileDimension = tileDimension;

		this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
		for ( int i = 0; i < sizeof( this->_pTiles ) / sizeof( this->_pTiles[0] ); i++ )
		{
			this->_pTiles[i].id = 0;
		}

		return;
	}

	template<typename T>
	void Layer<T>::create( int layerX, int layerY, int tileX, int tileY )
	{
	// Establishes essentially a blank layer canvas
		this->_layerDimension = vi2d{ layerX, layerY };
		this->_tileDimension = vi2d{ tileX, tileY };

		this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
		for ( int i = 0; i < sizeof( this->_pTiles ) / sizeof( this->_pTiles[0] ); i++ )
		{
			this->_pTiles[i].id = 0;
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
	// returns a pointer to a tile given a indices [row, col] on the layer's matrix map of tiles
		if ( x < 0 || x >= this->_layerDimension.x || y < 0 || y > this->_layerDimension.y )
		{
			return nullptr;
		}
		else
		{
			// transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
			return &this->_pTiles[y*this->_layerDimension.y + x];
		}
	}

	template<typename T>
	T* Layer<T>::getTile( vi2d indicies )
	{
	// returns a pointer to a tile given a indicies [row, col] on the layer's matrix map of tiles
		if ( indicies.x < 0 || indicies.x >= this->_layerDimension.x || indicies.y < 0 || indicies.y > this->_layerDimension.y )
		{
			return nullptr;
		}
		else
		{
			// transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
			return &this->_pTiles[indicies.y * this->_layerDimension.y + indicies.x];
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
*/