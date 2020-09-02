#include "olcPixelGameEngine.h"
#include "Layer.h"

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
	void Layer<T>::create( vi2d layerSize, vi2d tileSize )
	{
	//
	}

	template<typename T>
	void Layer<T>::create( int layerWidth, int layerHeight, int tileWidth, int tileHeight )
	{
	//
	}

	template<typename T>
	rcode Layer<T>::loadFromFile( std::string filename )
	{
	//
	}

	template<typename T>
	rcode Layer<T>::saveToFile( std::string filename )
	{
	//
	}

	template<typename T>
	T* Layer<T>::getTile( int x, int y )
	{
	// returns a pointer to a tile given a position (row, col) on the layer's matrix map of tiles

	}

	template<typename T>
	T* Layer<T>::getTile( vi2d position )
	{
	// returns a pointer to a tile given a position (row, col) on the layer's matrix map of tiles

	}

}