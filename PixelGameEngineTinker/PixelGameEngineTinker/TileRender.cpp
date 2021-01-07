#include "TileRender.h"

TileRender::TileRender()
	: Cell( BoundingBox<std::int64_t>() ), _bordersDecalIndex( 0 ), _id( 0 )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( std::uint64_t id, const BoundingBox<std::int64_t>& bounds )
	: Cell( bounds ), _bordersDecalIndex( 0 ), _id( id )
{

}


std::uint64_t TileRender::getId() const
{
	return this->_id;
}


std::uint64_t TileRender::setId( std::uint64_t id )
{
	this->_id = id;
	return this->_id;
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

