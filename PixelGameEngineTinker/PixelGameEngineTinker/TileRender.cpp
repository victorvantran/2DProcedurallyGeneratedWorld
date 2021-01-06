#include "TileRender.h"

TileRender::TileRender()
	: Cell( 0, BoundingBox<std::int64_t>() ), _bordersDecalIndex( 0 )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( std::uint64_t id, const BoundingBox<std::int64_t>& bounds )
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

