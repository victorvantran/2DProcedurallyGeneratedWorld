#include "Tile.h"


Tile::Tile()
{
	// Establishes essentially a blank, non-existent tile
	this->id = 0;
	this->exist = true;
	this->tileType = TileType::Block;

	for ( int i = 0; i < sizeof(this->edgeId)/sizeof(this->edgeId[0]); i++ )
	{
		this->edgeId[i] = 0;
		this->edgeExist[i] = true;
	}
}

Tile::~Tile()
{
	delete[] this->edgeId;
	delete[] this->edgeExist;
}


bool Tile::isEmpty()
{
	return this->tileType == TileType::Empty;
}


bool Tile::isBlock()
{
	return this->tileType == TileType::Block;
}


bool Tile::isOneWay()
{
	return this->tileType == TileType::OneWay;
}


bool Tile::isGround()
{
	return this->tileType == TileType::Block || this->tileType == TileType::OneWay;
}