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



/*
bool LightRender::isSolid() const
{
	std::uint8_t r = ( std::uint8_t )( ( corner0 & 0xff000000 ) >> 24 );
	std::uint8_t g = ( std::uint8_t )( ( corner0 & 0x00ff0000 ) >> 16 );
	std::uint8_t b = ( std::uint8_t )( ( corner0 & 0x0000ff00 ) >> 8 );
	std::uint8_t a = ( std::uint8_t )( ( corner0 & 0x000000ff ) );

	return r == g && g == b &&
		corner0 == corner1 && corner1 == corner2 && corner2 == corner3;
	
	//return r == g && g == b && a == 255 &&
	//	corner0 == corner1 && corner1 == corner2 && corner2 == corner3;
		
}
*/



