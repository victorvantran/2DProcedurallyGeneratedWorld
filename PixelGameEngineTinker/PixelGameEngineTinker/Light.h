#pragma once

#include <cstdint>

class Light
{
private:
	std::int16_t _r;
	std::int16_t _g;
	std::int16_t _b;
	std::int16_t _a;

public:
	// Constructors
	Light() : _r( 0 ), _g( 0 ), _b( 0 ), _a( 0 ) {}
	Light( std::int16_t r_, std::int16_t g_, std::int16_t b_ ) : _r( r_ ), _g( g_ ), _b( b_ ), _a( 0 ) {}
	Light( std::int16_t r_, std::int16_t g_, std::int16_t b_, std::int16_t a_ ) : _r( r_ ), _g( g_ ), _b( b_ ), _a( a_ ) {}


	// Getters
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

	void addRed( std::int16_t r_ )
	{
		this->_r += r_;
		return;
	}

	void addGreen( std::int16_t g_ )
	{
		this->_g += g_;
		return;
	}

	void addBlue( std::int16_t b_ )
	{
		this->_b += b_;
		return;
	}

	void addAlpha( std::int16_t a_ )
	{
		this->_a += a_;
		return;
	}

	void reset()
	{
		this->_r = 0;
		this->_g = 0;
		this->_b = 0;
		this->_a = 0;

		return;
	}

	void blacken()
	{
		this->_r = 0;
		this->_g = 0;
		this->_b = 0;
		this->_a = 255;

		return;
	}

	void whiten()
	{
		this->_r = 255;
		this->_g = 255;
		this->_b = 255;
		this->_a = 255;

		return;
	}
};
