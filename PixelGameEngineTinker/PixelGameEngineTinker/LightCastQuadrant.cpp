/*
#include "LightCastQuadrant.h"

template <class T>
LightCastQuadrant<T>::LightCastQuadrant( std::int8_t cardinal_, T originX_, T originY_ ) : cardinal( cardinal_ ), origin( originX_, originY_ ) {}

template <class T>
LightCastQuadrant<T>::~LightCastQuadrant() {}


template <class T>
olc::v2d_generic<T> LightCastQuadrant<T>::transform( const olc::v2d_generic<T>& tile )
{

	T col = tile.x;
	T row = tile.y;
	if ( this->cardinal == LightCastQuadrant::NORTH )
	{
		return this->origin + olc::v2d_generic<T>{ col, -row };
	}
	else if ( this->cardinal == LightCastQuadrant::SOUTH )
	{
		return this->origin + olc::v2d_generic<T>{ col, row };
	}
	else if ( this->cardinal == LightCastQuadrant::EAST )
	{
		return this->origin + olc::v2d_generic<T>{ row, col };
	}
	else if ( this->cardinal == LightCastQuadrant::WEST )
	{
		return this->origin + olc::v2d_generic<T>{ -row, col };
	}
	else
	{
		return this->origin;
	}


}

*/