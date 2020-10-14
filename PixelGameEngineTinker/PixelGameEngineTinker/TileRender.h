#pragma once
#include "Cell.h"

class TileRender : public Cell
{
protected:


public:
	TileRender();
	~TileRender();


	TileRender( int id, const BoundingBox<int>& bounds );

};