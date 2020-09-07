#pragma once

#include "olcPixelGameEngine.h"
#include "Atlas.h"


template <typename T>
class Layer : public olc::PGEX
{
private:
	olc::vi2d _layerDimension;

	Atlas _atlas;
	T* _pCells;
		
public:
	Layer();
	~Layer();

	Layer( olc::vi2d layerDimension );
	Layer( int layerWidth, int layerHeight );
		
	void create( olc::vi2d layerDimension );
	void create( int layerWidth, int layerHeight );

	void generateRandomness( Atlas* atlas );
	void copyMapping( std::vector<std::tuple<int,bool>> mapping );

	olc::rcode loadFromFile( std::string filename );
	olc::rcode saveToFile( std::string filename );

	T* getCell( int x, int y );
	T* getCell( olc::vi2d position );

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
Layer<T>::Layer( olc::vi2d layerDimension )
{
	/// Constructs a layer of given dimension filled with non-existent cells
	this->create( layerDimension );
}



template<typename T>
Layer<T>::Layer( int layerX, int layerY )
{
	/// Constructs a layer of given dimension filled with non-existent cells
	this->create( layerX, layerY );
}


template<typename T>
void Layer<T>::create( olc::vi2d layerDimension )
{
	/// Establishes essentially a blank layer canvas given the layer dimension of cells [col, row] and cell resolution
	/// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the cell resolution)
	this->_layerDimension = layerDimension;

	this->_pCells = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		this->_pCells[i].id = 0;
		this->_pCells[i].exist = false;
	}

	return;
}


template<typename T>
void Layer<T>::create( int layerX, int layerY )
{
	/// Establishes essentially a blank layer canvas given the layer dimension of cells [col, row] and cell resolution
	/// Example: (64 cells by 64 cells for the layer matrix dimension, 64 pixel by 64 pixel for the cell resolution)
	this->_layerDimension = olc::vi2d{ layerX, layerY };

	this->_pCells = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		this->_pCells[i].id = 0;
		this->_pCells[i].exist = false;
	}

	return;
}


template<typename T>
void Layer<T>::generateRandomness( Atlas* atlas )
{
	/// Generate pure random array of tiles given an atlas to choose from
	olc::vi2d atlasDimension = atlas->getAtlasDimension();
	int numTiles = atlasDimension.x * atlasDimension.y;

	this->_pCells = new T[this->_layerDimension.x * this->_layerDimension.y];
	for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
	{
		/// In this case, rand() [short int] is from 0 to 32,767, so ideally the atlas should not have over 32,768 unique tiles
		this->_pCells[i].id = rand() % numTiles;
		this->_pCells[i].exist = true;
	}

	return;
}


template<typename T>
void Layer<T>::copyMapping( std::vector<std::tuple<int, bool>> mapping )
{
	/// Given a vector of tuple(int, bool), copy it into the mapping
	///		1) id
	///		2) existence
	if ( mapping.size() == this->_layerDimension.x * this->_layerDimension.y )
	{
		for ( int i = 0; i < this->_layerDimension.x * this->_layerDimension.y; i++ )
		{
			this->_pCells[i].id = std::get<0>( mapping[i] );
			this->_pCells[i].exist = std::get<1>( mapping[i] );
		}
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
T* Layer<T>::getCell( int x, int y )
{
	/// Returns a pointer to a cell given a indices [col, row] on the layer's matrix map of cells
	if ( x < 0 || x >= this->_layerDimension.x || y < 0 || y >= this->_layerDimension.y )
	{
		return nullptr;
	}
	else
	{
	/// Transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
		return &this->_pCells[y*this->_layerDimension.x + x];
	}
}


template<typename T>
T* Layer<T>::getCell( olc::vi2d indicies )
{
	/// Returns a pointer to a cell given a indicies [col, row] on the layer's matrix map of cells
	if ( indicies.x < 0 || indicies.x >= this->_layerDimension.x || indicies.y < 0 || indicies.y >= this->_layerDimension.y )
	{
		return nullptr;
	}
	else
	{
	/// Transposing a "2d matrix" to an array (finding the correct row (y), then column (x))
		return &this->_pCells[indicies.y * this->_layerDimension.x + indicies.x];
	}
}



template<typename T>
olc::vi2d Layer<T>::getLayerDimension()
{
	/// Returns the dimension of the layer matrix
	return this->_layerDimension;
}



