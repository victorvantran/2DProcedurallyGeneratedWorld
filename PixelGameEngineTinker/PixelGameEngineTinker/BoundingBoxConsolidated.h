#pragma once

#include "BoundingBox.h"

template<typename T>
struct BoundingBoxConsolidated : public BoundingBox<T>
{
	int id = -1;
	int consolidateLevel = 0; // [!] derived from size, so not neccessary

	BoundingBoxConsolidated<T>* = nullptr; // BoundingBoxConsolidated<T>[4]

	BoundingBoxConsolidated( T x = -1, T y = -1, T width = -1, T height = -1, int id = -1, int consolidateLevel = 0 );
	~BoundingBoxConsolidated();

	void clear();
};


template<typename T>
BoundingBoxConsolidated<T>::BoundingBoxConsolidated( T x, T y, T width, T height, int id, int consolidateLevel )
	: BoundingBox<T>( x, y, width, height ), id( id ), consolidateLevel( consolidateLevel )
{

}


template<typename T>
BoundingBoxConsolidated<T>::~BoundingBoxConsolidated()
{

}



template<typename T>
void BoundingBoxConsolidated<T>::clear()
{
	this->x = -1;
	this->y = -1;
	this->width = -1;
	this->height = -1;
	this->id = -1;
	return;
}