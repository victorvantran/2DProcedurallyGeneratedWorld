#pragma once

#include <cstdint>

class Light
{
private:
	std::int32_t _r;
	std::int32_t _g;
	std::int32_t _b;
	std::int32_t _a;

public:
	// Constructors
	Light() : _r( 0 ), _g( 0 ), _b( 0 ), _a( 0 ) {}
	Light( std::int32_t r_, std::int32_t g_, std::int32_t b_ ) : _r( r_ ), _g( g_ ), _b( b_ ), _a( 0 ) {}
	Light( std::int32_t r_, std::int32_t g_, std::int32_t b_, std::int32_t a_ ) : _r( r_ ), _g( g_ ), _b( b_ ), _a( a_ ) {}


	// Getters
	std::int32_t getRed() const
	{
		return this->_r;
	}

	std::int32_t getGreen() const
	{
		return this->_g;
	}

	std::int32_t getBlue() const
	{
		return this->_b;
	}

	std::int32_t getAlpha() const
	{
		return this->_a;
	}


	// Setters
	void setRed( std::int32_t r_ )
	{
		this->_r = r_;
		return;
	}

	void setGreen( std::int32_t g_ )
	{
		this->_g = g_;
		return;
	}

	void setBlue( std::int32_t b_ )
	{
		this->_b = b_;
		return;
	}

	void setAlpha( std::int32_t a_ )
	{
		this->_a = a_;
		return;
	}

	void addRed( std::int32_t r_ )
	{
		this->_r += r_;
		return;
	}

	void addGreen( std::int32_t g_ )
	{
		this->_g += g_;
		return;
	}

	void addBlue( std::int32_t b_ )
	{
		this->_b += b_;
		return;
	}

	void addAlpha( std::int32_t a_ )
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
