#include "Tile.h"


Tile::Tile()
{
	// Establishes essentially a blank, non-existent tile
	this->id = 0;
	this->exist = false;
	this->tileType = TILE_TYPE::BASIC;

	for ( int i = 0; i < sizeof(this->edgeId)/sizeof(this->edgeId[0]); i++ )
	{
		this->edgeId[i] = 0;
		this->edgeExist[i] = false;
	}
}

Tile::~Tile()
{

}
