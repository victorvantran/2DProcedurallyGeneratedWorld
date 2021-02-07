#include "BackgroundTile.h"

BackgroundTile::BackgroundTile()
	: _id( BackgroundTileIdentity::Void ), _type( BackgroundTileType::Static ), _borders( 0 )
{
	// setType
}


BackgroundTile::~BackgroundTile()
{
}


BackgroundTile::BackgroundTile( BackgroundTileIdentity id, BackgroundTileType type )
	: _id( id ), _type( type ), _borders( 0 )
{
}


void BackgroundTile::setId( BackgroundTileIdentity id )
{
	this->_id = id;
	return;
}


void BackgroundTile::setType( BackgroundTileType type )
{
	this->_type = type;
	return;
}




void BackgroundTile::setBorders( std::uint8_t borders )
{
	this->_borders = borders;
}


void BackgroundTile::setBorder( BackgroundTileBorder border )
{
	this->_borders |= 1 << ( std::uint8_t )border;
}


void BackgroundTile::unsetBorder( BackgroundTileBorder border )
{
	this->_borders &= ~( 1 << ( std::uint8_t )border );
}


BackgroundTileIdentity BackgroundTile::getId() const
{
	return this->_id;
}


BackgroundTileType BackgroundTile::getType() const
{
	return this->_type;
}


std::uint8_t BackgroundTile::getBorders() const
{
	return this->_borders;
}


bool BackgroundTile::getBorder( BackgroundTileBorder border ) const
{
	return ( ( this->_borders ) >> ( std::uint8_t )border ) & 1;
}


std::uint8_t BackgroundTile::getBordersDecalIndex() const
{
	return BackgroundTile::_borderValueToDecalIndex[( long long unsigned )this->_borders];
}


bool BackgroundTile::exists() const
{
	return this->_id != BackgroundTileIdentity::Void;
}


bool BackgroundTile::isVoid() const
{
	return this->_id == BackgroundTileIdentity::Void;
}


bool BackgroundTile::isStatic() const
{
	return this->_type == BackgroundTileType::Static;
}



void BackgroundTile::clear()
{
	this->_id = BackgroundTileIdentity::Void;
	this->_type = BackgroundTileType::Static;
	this->_borders = 0;
	return;
}


/*
const std::uint8_t BackgroundTile::_borderValueToDecalIndex[256] = {
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		38, 38, 37, 37, 38, 38, 37, 37, 40, 40, 41, 41, 40, 40, 39, 39,
		38, 38, 37, 37, 38, 38, 37, 37, 35, 35, 36, 36, 35, 35, 34, 34,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		46, 46, 45, 45, 46, 46, 45, 45, 43, 43, 44, 44, 43, 43, 42, 42,
		38, 38, 37, 37, 38, 38, 37, 37, 40, 40, 41, 41, 40, 40, 39, 39,
		38, 38, 37, 37, 38, 38, 37, 37, 35, 35, 36, 36, 35, 35, 34, 34,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		20, 20, 18, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		28, 28, 27, 26, 28, 28, 27, 26, 31, 31, 33, 32, 31, 31, 30, 29,
		28, 28, 27, 26, 28, 28, 27, 26, 23, 23, 25, 24, 23, 23, 22, 21,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		20, 20, 19, 18, 20, 20, 19, 18, 15, 15, 17, 16, 15, 15, 14, 13,
		7 , 7 , 6 , 5 , 7 , 7 , 6 , 5 , 10, 10, 12, 11, 10, 10, 9 , 8 ,
		7 , 7 , 6 , 5 , 7 , 7 , 6 , 5 , 2 , 2 , 4 , 3 , 2 , 2 , 1 , 0
};

*/