#include "TileRender.h"

TileRender::TileRender()
	: Cell( 0, BoundingBox<int>() ), _bordersDecalIndex( 0 )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( int id, const BoundingBox<int>& bounds )
	: Cell( id, bounds ), _bordersDecalIndex( 0 )
{

}


void TileRender::setBordersDecalIndex( std::uint8_t bordersDecalIndex )
{
	this->_bordersDecalIndex = bordersDecalIndex;
}


std::uint8_t TileRender::getBordersDecalIndex() const
{
	return this->_bordersDecalIndex;
}


bool TileRender::exists() const
{
	return this->_id != 0;
}

