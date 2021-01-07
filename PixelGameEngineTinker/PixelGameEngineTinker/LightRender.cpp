#pragma once
#include "LightRender.h"

#include <string.h>

LightRender::LightRender()
	: Cell( BoundingBox<std::int64_t>() )
{

}


LightRender::LightRender( std::uint8_t* corner0_, std::uint8_t* corner1_, std::uint8_t* corner2_, std::uint8_t* corner3_, const BoundingBox<std::int64_t>& bounds )
	: Cell( bounds )
{
	std::memcpy( this->corner0, corner0_, 4 );
	std::memcpy( this->corner1, corner1_, 4 );
	std::memcpy( this->corner2, corner2_, 4 );
	std::memcpy( this->corner3, corner3_, 4 );
}


LightRender::~LightRender()
{

}


