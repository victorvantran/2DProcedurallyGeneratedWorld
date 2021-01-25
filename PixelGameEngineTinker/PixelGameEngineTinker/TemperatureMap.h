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


	float getPerlinValue( std::int64_t x, std::int64_t y ) const
	{
		float fAccumulativeNoise = 0.0f;
		float fScalingFactor = 1.0f;
		float fAccumulativeScalingFactor = 0.0f;

		for ( std::int32_t octave = 0; octave < this->_octave; octave++ )
		{
			std::int32_t nPitch = this->_width >> octave;
			if ( nPitch == 0 ) return 0.0f;

			std::int64_t nSampleX1 = x / nPitch;
			std::int64_t remainderX = x % nPitch;
			if ( remainderX < 0 ) nSampleX1 -= 1;
			nSampleX1 *= nPitch;

			std::int64_t nSampleY1 = y / nPitch;
			std::int64_t remainderY = y % nPitch;
			if ( remainderY < 0 ) nSampleY1 -= 1;
			nSampleY1 *= nPitch;

			std::int64_t nSampleX2 = ( nSampleX1 + nPitch );
			std::int64_t nSampleY2 = ( nSampleY1 + nPitch );

			// Linear interpolation
			float fBlendX = ( float )( x - nSampleX1 ) / ( float )nPitch;
			float fBlendY = ( float )( y - nSampleY1 ) / ( float )nPitch;

			float fSampleT = ( 1.0f - fBlendX ) * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, nSampleX1, nSampleY1 ) ) +
				fBlendX * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, nSampleX2, nSampleY1 ) );
			float fSampleB = ( 1.0f - fBlendX ) * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, nSampleX1, nSampleY2 ) ) +
				fBlendX * ( this->getNoiseValue( this->_seed, this->_primeX, this->_primeY, nSampleX2, nSampleY2 ) );

			// Interpolate by Y axis
			fAccumulativeNoise += ( fBlendY * ( fSampleB - fSampleT ) + fSampleT ) * fScalingFactor;
			fAccumulativeScalingFactor += fScalingFactor;
			fScalingFactor = this->functionScalingFactor( fScalingFactor );
		}

		// Scaled to seed range
		float fNormalizedNoise = fAccumulativeNoise / fAccumulativeScalingFactor;

		return fNormalizedNoise;
	}


};