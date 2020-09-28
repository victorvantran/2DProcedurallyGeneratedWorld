#include "TileConsolidated.h"

TileConsolidated::TileConsolidated()
	: Cell( -1, BoundingBox<int>(), false )
{

}


TileConsolidated::~TileConsolidated()
{

}


TileConsolidated::TileConsolidated( int id, const BoundingBox<int>& bounds, bool exist )
	: Cell( id, bounds, exist )
{

}


