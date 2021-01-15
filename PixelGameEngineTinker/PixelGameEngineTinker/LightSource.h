#pragma once

#include "Tile.h"
#include <cstdint>

class LightSource
{
private:
	TileIdentity _tileId;


	std::int16_t _r;
	std::int16_t _g;
	std::int16_t _b;
	std::int16_t _a;

	std::int16_t _radius;

public:
	LightSource()
		: _tileId( TileIdentity::Void ), _r( 0 ), _g( 0 ), _b( 0 ), _a( 0 ), _radius( 0 ) {}

	LightSource( TileIdentity tileId, std::int16_t r_, std::int16_t g_, std::int16_t b_, std::int16_t a_, std::int16_t radius_ )
		: _tileId( tileId ), _r( r_ ), _g( g_ ), _b( b_ ), _a( a_ ), _radius( radius_ ) {}


	// Getters
	TileIdentity getTileId() const
	{
		return this->_tileId;
	}

	std::int16_t getRed() const
	{
		return this->_r;
	}


	std::int16_t getGreen() const
	{
		return this->_g;
	}


	std::int16_t getBlue() const
	{
		return this->_b;
	}


	std::int16_t getAlpha() const
	{
		return this->_a;
	}


	std::int16_t getRadius() const
	{
		return this->_radius;
	}


	// Setters
	void setRed( std::int16_t r_ )
	{
		this->_r = r_;
		return;
	}


	void setGreen( std::int16_t g_ )
	{
		this->_g = g_;
		return;
	}


	void setBlue( std::int16_t b_ )
	{
		this->_b = b_;
		return;
	}


	void setAlpha( std::int16_t a_ )
	{
		this->_a = a_;
		return;
	}


	void setRadius( std::int16_t radius_ )
	{
		this->_radius = radius_;
		return;
	}
};

