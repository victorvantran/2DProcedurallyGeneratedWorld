#pragma once
#include "LightRender.h"

#include <string.h>

LightRender::LightRender()
	: corner0( 0 ), corner1( 0 ), corner2( 0 ), corner3( 0 ), exist( false ), Cell( BoundingBox<std::int64_t>() )
{
}


LightRender::LightRender( std::uint32_t corner0_, std::uint32_t corner1_, std::uint32_t corner2_, std::uint32_t corner3_, bool exist_, const BoundingBox<std::int64_t>& bounds )
	: corner0( corner0_ ), corner1( corner1_ ), corner2( corner2_ ), corner3( corner3_ ), exist( exist_ ), Cell( bounds )
{
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
	return corner0 == corner1 && corner1 == corner2 && corner2 == corner3;
}



