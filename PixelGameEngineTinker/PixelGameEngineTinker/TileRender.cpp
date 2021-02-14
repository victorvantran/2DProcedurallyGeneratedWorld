#include "TileRender.h"

TileRender::TileRender()
	: Cell( BoundingBox<std::int64_t>() ), _id( TileIdentity::Void ), _borders( 0 ), _consolidatable( false ), _tileBlobMapIndex( 0 )
{

}


TileRender::~TileRender()
{

}


TileRender::TileRender( TileIdentity id, bool consolidatable, std::uint8_t borders, std::uint8_t tileBlobMapIndex, const BoundingBox<std::int64_t>& bounds )
	: Cell( bounds ), _id( id ), _consolidatable( consolidatable ), _borders( borders ), _tileBlobMapIndex( tileBlobMapIndex )
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


void TileRender::setConsolidatable( bool consolidatable )
{
	this->_consolidatable = consolidatable;
	return;
}


void TileRender::setTileBlobMapIndex( std::uint8_t tileBlobMapIndex )
{
	this->_tileBlobMapIndex = tileBlobMapIndex;
	return;
}



bool TileRender::getConsolidatable() const
{
	return this->_consolidatable;
}


void TileRender::setBorders( std::uint8_t borders )
{
	this->_borders = borders;
	return;
}


std::uint8_t TileRender::getBorders() const
{
	return this->_borders;
}


std::uint8_t TileRender::getBordersDecalIndex() const
{
	return TileRender::_tileBlobMaps[this->_tileBlobMapIndex][this->_borders];
}


std::uint8_t TileRender::getTileBlobMapIndex() const
{
	return this->_tileBlobMapIndex;
}


olc::vf2d TileRender::getTileBlobPartition() const
{
	if ( this->_tileBlobMapIndex == 0 )
	{
		return olc::vf2d{ ( float )( this->getBordersDecalIndex() % 8 ), ( float )( this->getBordersDecalIndex() / 8 ) };
	}
	else if ( this->_tileBlobMapIndex == 1 )
	{
		return olc::vf2d{ ( float )( this->getBordersDecalIndex() % 16 ), ( float )( this->getBordersDecalIndex() / 16 ) };
	}
	else
	{
		return olc::vf2d{ ( float )( this->getBordersDecalIndex() % 8 ), ( float )( this->getBordersDecalIndex() / 8 ) };
	}
}


bool TileRender::exists() const
{
	return this->_id != TileIdentity::Void;
}



void TileRender::clear()
{
	this->_id = TileIdentity::Void;
	this->_consolidatable = false;
	this->_borders = 0;
	this->_tileBlobMapIndex = 0;
	return;
}

