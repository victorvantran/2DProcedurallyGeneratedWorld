#pragma once
#include <cstdint>
#include "Tile.h"




void insertWater( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{

	return;
}


void insertStone( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{

	return;
}


void insertDirt( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{

	return;
}


void insertTorch( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{

	return;
}


typedef void ( *func_type )( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );

static func_type insertFunctions[(unsigned long long)TileIdentity::count]{ &insertWater, &insertStone, &insertDirt, &insertTorch };



void insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	(*insertFunctions[( unsigned long long )tileId])( tileId, x, y, width, height );
}