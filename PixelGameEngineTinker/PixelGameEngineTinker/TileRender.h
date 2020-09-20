#pragma once
#include "Cell.h"

/*
Existence implies consolidated
*/

class TileRender : public Cell
{
protected:


public:
	TileRender();
	~TileRender();


	TileRender( int id, const BoundingBox<int>& bounds, bool exist );

};