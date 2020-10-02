#include "TileRender.h"

TileRender::TileRender()
	: Cell( -1, BoundingBox<int>(), false )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( int id, const BoundingBox<int>& bounds, bool exist )
	: Cell( id, bounds, exist )
{

}


