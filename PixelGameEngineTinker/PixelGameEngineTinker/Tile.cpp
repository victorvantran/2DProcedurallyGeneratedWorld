#include "Tile.h"

Tile::Tile()
	: _id( TileIdentity::Void ), _type( TileType::Empty ), _consolidatable( true ), _opaque( false ), _complementary( true ), _borders( 0 ), _tileBlobMapIndex( 0 )
{
	// setType
}


Tile::~Tile()
{
}


Tile::Tile( TileIdentity id, TileType type, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex )
	: _id( id ), _type( type ), _consolidatable( consolidatable ), _opaque( opaque ), _complementary( complementary ), _borders( borders ), _tileBlobMapIndex( tileBlobMapIndex )
{
}


void Tile::setId( TileIdentity id )
{
	this->_id = id;
	return;
}


void Tile::setType( TileType type )
{
	this->_type = type;
	return;
}


void Tile::setConsolidatable( bool consolidatable )
{
	this->_consolidatable = consolidatable;
	return;
}


void Tile::setOpaque( bool opaque )
{
	this->_opaque = opaque;
	return;
}


void Tile::setComplementary( bool complementary )
{
	this->_complementary = complementary;
	return;
}


void Tile::setBorders( std::uint8_t borders )
{
	this->_borders = borders;
	return;
}


void Tile::setBorder( TileBorder border )
{
	this->_borders |= 1 << ( std::uint8_t )border;
	return;
}


void Tile::unsetBorder( TileBorder border )
{
	this->_borders &= ~( 1 << ( std::uint8_t )border );
	return;
}


void Tile::setTileBlobMapIndex( std::uint8_t tileBlobMapIndex )
{
	this->_tileBlobMapIndex = tileBlobMapIndex;
	return;
}



TileIdentity Tile::getId() const
{
	return this->_id;
}


TileType Tile::getType() const
{
	return this->_type;
}


bool Tile::getConsolidatable() const
{
	return this->_consolidatable;
}


bool Tile::getOpaque() const
{
	return this->_opaque;
}


bool Tile::getComplementary() const
{
	return this->_complementary;
}


std::uint8_t Tile::getBorders() const
{
	return this->_borders;
}


bool Tile::getBorder( TileBorder border ) const
{
	return ( ( this->_borders ) >> ( std::uint8_t )border ) & 1;
}


std::uint8_t Tile::getTileBlobMapIndex() const
{
	return this->_tileBlobMapIndex;
}


bool Tile::exists() const
{
	return this->_id != TileIdentity::Void;
}


bool Tile::isVoid() const
{
	return this->_id == TileIdentity::Void;
}


bool Tile::isEmpty() const
{
	return this->_type == TileType::Empty;
}



bool Tile::isObstacle() const
{
	return this->_type == TileType::Block;
}


bool Tile::isGround() const
{
	return this->_type == TileType::Block || this->_type == TileType::OneWay;
}


bool Tile::isOneWayPlatform() const
{
	return this->_type == TileType::OneWay;
}


bool Tile::isLedge() const
{
	return false;
}


void Tile::clear()
{
	this->_id = TileIdentity::Void;
	this->_type = TileType::Empty;
	this->_consolidatable = false;
	this->_opaque = false;
	this->_borders = 0;
	this->_tileBlobMapIndex = 0;
	return;
}
