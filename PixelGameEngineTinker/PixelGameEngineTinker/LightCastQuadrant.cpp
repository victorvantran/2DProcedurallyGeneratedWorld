#include "LightCastQuadrant.h"

LightCastQuadrant::LightCastQuadrant( std::int8_t cardinal_, long double originX_, long double originY_ ) : cardinal( cardinal_ ), origin( originX_, originY_ ) {}


LightCastQuadrant::~LightCastQuadrant() {}


olc::v2d_generic<long double> LightCastQuadrant::transform( const olc::v2d_generic<long double>& tile )
{
	long double col = tile.x;
	long double row = tile.y;
	if ( this->cardinal == LightCastQuadrant::NORTH )
	{
		return this->origin + olc::v2d_generic<long double>{ col, -row };
	}
	else if ( this->cardinal == LightCastQuadrant::SOUTH )
	{
		return this->origin + olc::v2d_generic<long double>{ col, row };
	}
	else if ( this->cardinal == LightCastQuadrant::EAST )
	{
		return this->origin + olc::v2d_generic<long double>{ row, col };
	}
	else if ( this->cardinal == LightCastQuadrant::WEST )
	{
		return this->origin + olc::v2d_generic<long double>{ -row, col };
	}
	else
	{
		return this->origin;
	}
}

