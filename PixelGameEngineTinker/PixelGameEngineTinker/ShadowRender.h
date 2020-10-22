#pragma once

#include <cstdint>
#include "BoundingBox.h"

class ShadowRender
{
	std::uint8_t _intensity;
	BoundingBox<float> _bounds;
private:

public:
	ShadowRender()
		: _intensity( 0 ), _bounds( BoundingBox<float>() )
	{

	}


	ShadowRender( std::uint8_t intensity, const BoundingBox<float>& bounds )
		: _intensity( intensity ), _bounds( bounds )
	{

	}


	~ShadowRender()
	{

	}


	void setIntensity( std::uint8_t intensity )
	{
		this->_intensity = intensity;
	}


	std::uint8_t getIntensity() const
	{
		return this->_intensity;
	}
};