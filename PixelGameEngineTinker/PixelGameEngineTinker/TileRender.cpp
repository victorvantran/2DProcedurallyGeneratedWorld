#include "TileRender.h"

TileRender::TileRender()
	: Cell( BoundingBox<std::int64_t>() ), _bordersDecalIndex( 0 ), _id( TileIdentity::Void )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( TileIdentity id, const BoundingBox<std::int64_t>& bounds )
	: Cell( bounds ), _bordersDecalIndex( 0 ), _id( id )
{

}


TileIdentity TileRender::getId() const
{
	return this->_id;
}


TileIdentity TileRender::setId( TileIdentity id )
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
	return this->_id != TileIdentity::Void;
}



void TileRender::clear()
{
	this->_id = TileIdentity::Void;
	return;
}

