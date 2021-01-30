#pragma once

#include "PerlinNoise2D.h"

class TemperatureMap : public PerlinNoise2D
{
private:

public:
	TemperatureMap()
		: PerlinNoise2D()
	{

	}

	TemperatureMap( std::int64_t seed, std::int64_t primeX, std::int64_t primeY, std::int32_t octave, std::int64_t width, std::int64_t height )
		: PerlinNoise2D( seed, primeX, primeY, octave, width, height )
	{

	}


	~TemperatureMap()
	{

	}


	long double getPerlinValue( std::int64_t x, std::int64_t y ) const
	{
		long double accumulativeNoise = 0.0;
		long double scalingFactor = 1.0;
		long double accumulativeScalingFactor = 0.0;

		for ( std::int32_t octave = 0; octave < this->_octave; octave++ )
		{
			std::int32_t pitch = this->_width >> octave;
			if ( pitch == 0 ) return 0.0;

			std::int64_t sampleX1 = x / pitch;
			std::int64_t remainderX = x % pitch;
			if ( remainderX < 0 ) sampleX1 -= 1;
			sampleX1 *= pitch;

			std::int64_t sampleY1 = y / pitch;
			std::int64_t remainderY = y % pitch;
			if ( remainderY < 0 ) sampleY1 -= 1;
			sampleY1 *= pitch;

			std::int64_t sampleX2 = ( sampleX1 + pitch );
			std::int64_t sampleY2 = ( sampleY1 + pitch );

			// Linear interpolation
			long double blendX = ( long double )( x - sampleX1 ) / ( long double )pitch;
			long double blendY = ( long double )( y - sampleY1 ) / ( long double )pitch;

			long double sampleT = ( 1.0 - blendX ) * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, sampleX1, sampleY1 ) ) +
				blendX * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, sampleX2, sampleY1 ) );
			long double sampleB = ( 1.0 - blendX ) * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, sampleX1, sampleY2 ) ) +
				blendX * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, sampleX2, sampleY2 ) );

			// Interpolate by Y axis
			accumulativeNoise += ( blendY * ( sampleB - sampleT ) + sampleT ) * scalingFactor;
			accumulativeScalingFactor += scalingFactor;
			scalingFactor = this->functionScalingFactor( scalingFactor );
		}

		// Scaled to seed range
		long double normalizedNoise = accumulativeNoise / accumulativeScalingFactor;

		return normalizedNoise;
	}
};