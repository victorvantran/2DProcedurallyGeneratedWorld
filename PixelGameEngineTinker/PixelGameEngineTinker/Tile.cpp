#include "Tile.h"

Tile::Tile()
	: _id( 0 )
{
	// setType
}

Tile::~Tile()
{
}


Tile::Tile( uint64_t id )
	: _id( id )
{
}


uint64_t Tile::getId() const
{
	return this->_id;
}


int Tile::getConfiguration() const
{
	return this->_configuration;
}


void Tile::setId( uint64_t id )
{
	this->_id = id;
	return;
}


int Tile::setConfiguration( int configuration )
{
	this->_configuration = configuration;
	return this->_configuration;
}


bool Tile::isVoid() const
{
	return this->_id == 0;
}


bool Tile::isEmpty() const
{
	return false;
}


bool Tile::isBlock() const
{
	return false;
}


bool Tile::isOneWayPlatform() const
{
	return false;
}


bool Tile::isObstacle() const
{
	return false;
}


bool Tile::isGround() const
{
	return false;
}


bool Tile::isLedge() const
{
	return false;
}



void Tile::clear()
{
	this->_id = -1;
	//this->_TileType = TileType::Empty;
	return;
}