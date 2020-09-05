#pragma once

#include "olcPixelGameEngine.h"
#include "Tile.h"
#include "Atlas.h"


template <typename T>
class Layer : public olc::PGEX
{
private:
	olc::vi2d _layerDimension;

	Atlas _atlas;
	T* _pTiles;
		
public:
	Layer();
	~Layer();
		
	void create( olc::vi2d layerSize );
	void create( int layerWidth, int layerHeight );

	void generateRandomness( Atlas* atlas );

	olc::rcode loadFromFile(std::string filename);
	olc::rcode saveToFile(std::string filename);

	T* getTile(int x, int y);
	T* getTile(olc::vi2d position);

	olc::vi2d getLayerDimension();

};



/// [!] Template class implementation need to be visible to all translation units that use them. Move the implementations in the .cpp to the header file. No need for a .cpp file


template<typename T>
Layer<T>::Layer()
{

}

template<typename T>
Layer<T>::~Layer()
{

}


template<typename T>
void Layer<T>::create( olc::vi2d layerDimension )
{
/// Establishes essentially a blank layer canvas given the layer dimension of tiles [col, row] and tile resolution
/// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the tile resolution)
	this->_layerDimension = layerDimension;

	this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		this->_pTiles[i].id = i;
		this->_pTiles[i].exist = true;
	}

	return;
}


template<typename T>
void Layer<T>::create( int layerX, int layerY )
{
/// Establishes essentially a blank layer canvas given the layer dimension of tiles [col, row] and tile resolution
/// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the tile resolution)
	this->_layerDimension = olc::vi2d{ layerX, layerY };

	this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		this->_pTiles[i].id = i;
		this->_pTiles[i].exist = true;
	}

	return;
}


template<typename T>
void Layer<T>::generateRandomness( Atlas* atlas )
{
/// 
	olc::vi2d atlasDimension = atlas->getAtlasDimension();
	int numTiles = atlasDimension.x * atlasDimension.y;

	this->_pTiles = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		/// In this case, rand() [short int] is from 0 to 32,767, so ideally the atlas should not have over 32,768 unique tiles
		this->_pTiles[i].id = rand() % numTiles;
		this->_pTiles[i].exist = true;
	}

	return;
}





template<typename T>
olc::rcode Layer<T>::loadFromFile( std::string filename )
{
///
	return olc::FAIL;
}


template<typename T>
olc::rcode Layer<T>::saveToFile( std::string filename )
{
	///
	return olc::FAIL;
}


template<typename T>
T* Layer<T>::getTile( int x, int y )
{
/// Returns a pointer to a tile given a indices [col, row] on the layer's matrix map of tiles
	if ( x < 0 || x >= this->_layerDimension.x || y < 0 || y >= this->_layerDimension.y )
	{
		return nullptr;
	}
	else
	{
	/// Transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
		return &this->_pTiles[y*this->_layerDimension.x + x];
	}
}


template<typename T>
T* Layer<T>::getTile( olc::vi2d indicies )
{
/// Returns a pointer to a tile given a indicies [col, row] on the layer's matrix map of tiles
	if ( indicies.x < 0 || indicies.x >= this->_layerDimension.x || indicies.y < 0 || indicies.y >= this->_layerDimension.y )
	{
		return nullptr;
	}
	else
	{
	/// Transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
		return &this->_pTiles[indicies.y * this->_layerDimension.x + indicies.x];
	}
}


template<typename T>
olc::vi2d Layer<T>::getLayerDimension()
{
/// Returns the dimension of the layer matrix
	return this->_layerDimension;
}



