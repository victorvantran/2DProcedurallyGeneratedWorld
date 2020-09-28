#include "Tile.h"
#include "BoundingBox.h"

Tile::Tile()
	: TileConsolidated( -1, BoundingBox<int>(), false ), _configuration( -1 ), _tileType( TileType::Empty )
{
	for ( int i = 0; i < sizeof( this->_edgeId ) / sizeof( this->_edgeId[0] ); i++ )
	{
		this->_edgeId[i] = -1;
		this->_edgeExist[i] = false;
	}
}

Tile::~Tile()
{
	//delete[] this->_edgeId;
	//delete[] this->_edgeExist;
}


Tile::Tile( int id, const BoundingBox<int>& bounds, bool exist, int configuration, TileType tileType )
	: TileConsolidated( id, bounds, exist ), _configuration( configuration ), _tileType( tileType )
{
	for ( int i = 0; i < sizeof( this->_edgeId ) / sizeof( this->_edgeId[0] ); i++ )
	{
		this->_edgeId[i] = 0;
		this->_edgeExist[i] = true;
	}
}



int Tile::getConfiguration() const
{
	return this->_configuration;
}



int Tile::setConfiguration( int configuration )
{
	this->_configuration = configuration;
	return this->_configuration;
}


bool Tile::isEmpty() const
{
	return this->_exist &&
		this->_tileType == TileType::Empty;
}


bool Tile::isBlock() const
{
	return this->_exist &&
		this->_tileType == TileType::Block;
}


bool Tile::isOneWayPlatform() const
{
	return this->_exist &&
		this->_tileType == TileType::OneWayPlatform;
}


bool Tile::isObstacle() const
{
	return this->_exist &&
		this->_tileType == TileType::Block || this->_tileType == TileType::OneWayPlatform;
}


bool Tile::isGround() const
{
	return this->_exist &&
		this->_tileType == TileType::Block || this->_tileType == TileType::OneWayPlatform;
}


bool Tile::isLedge() const
{
	return this->_exist &&
		this->_tileType == TileType::Block || this->_tileType == TileType::OneWayPlatform;
}



void Tile::clear()
{
	this->_id = -1;
	this->_bounds.clear();
	this->_exist = false;

	this->_configuration = -1;
	this->_tileType = TileType::Empty;
	//this->_isDynamic = false;


	for ( int i = 0; i < sizeof( this->_edgeId ) / sizeof( this->_edgeId[0] ); i++ )
	{
		this->_edgeId[i] = -1;
		this->_edgeExist[i] = false;
	}
}