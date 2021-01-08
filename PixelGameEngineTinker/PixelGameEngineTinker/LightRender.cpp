#pragma once
#include "LightRender.h"

#include <string.h>

LightRender::LightRender()
	: exist( false ), Cell( BoundingBox<std::int64_t>() )
{

}


LightRender::LightRender( std::uint8_t* corner0_, std::uint8_t* corner1_, std::uint8_t* corner2_, std::uint8_t* corner3_, bool exist_, const BoundingBox<std::int64_t>& bounds )
	: exist( exist_ ), Cell( bounds )
{
	std::memcpy( this->corner0, corner0_, 4 );
	std::memcpy( this->corner1, corner1_, 4 );
	std::memcpy( this->corner2, corner2_, 4 );
	std::memcpy( this->corner3, corner3_, 4 );
}


LightRender::~LightRender()
{

}



bool LightRender::exists() const
{
	return this->exist;
}

bool LightRender::isSolid() const
{
	return corner0[0] == corner0[1] && corner0[1] == corner0[2] && corner0[2] == corner0[3] &&
		corner1[0] == corner1[1] && corner1[1] == corner1[2] && corner1[2] == corner1[3] &&
		corner2[0] == corner2[1] && corner2[1] == corner2[2] && corner2[2] == corner2[3] &&
		corner3[0] == corner3[1] && corner3[1] == corner3[2] && corner3[2] == corner3[3];
}


