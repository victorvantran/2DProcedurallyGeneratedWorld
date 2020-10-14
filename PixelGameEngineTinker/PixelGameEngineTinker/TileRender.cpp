#include "TileRender.h"

TileRender::TileRender()
	: Cell( 0, BoundingBox<int>() )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( int id, const BoundingBox<int>& bounds )
	: Cell( id, bounds )
{

}


