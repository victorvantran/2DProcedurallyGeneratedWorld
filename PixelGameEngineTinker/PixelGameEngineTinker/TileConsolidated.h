#pragma once
#include "Cell.h"

/*
Existence implies consolidated
*/

class TileConsolidated : public Cell
{
protected:


public:
	TileConsolidated();
	~TileConsolidated();


	TileConsolidated( int id, const BoundingBox<int>& bounds, bool exist );

};