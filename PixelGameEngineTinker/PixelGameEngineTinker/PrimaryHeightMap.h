#pragma once

#include "PerlinNoise1D.h"

class PrimaryHeightMap : public PerlinNoise1D
{
private:
	float functionScalingFactor( float scalingFactor ) const
	{
		return scalingFactor / 2.0f;
	}
public:
	PrimaryHeightMap()
		: PerlinNoise1D()
	{

	}

	PrimaryHeightMap( std::int64_t seed, std::int64_t primeX, std::int32_t octave, std::int64_t width )
		: PerlinNoise1D( seed, primeX, octave, width )
	{

	}

	~PrimaryHeightMap()
	{
	}


	float getPerlinValue( std::int64_t x ) const
	{
		float fAccumulativeNoise = 0.0f;
		float fScalingFactor = 1.0f;
		float fAccumulativeScalingFactor = 0.0f;

		for ( std::int32_t octave = 0; octave < this->_octave; octave++ )
		{
			std::int32_t nPitch = this->_width >> octave;
			if ( nPitch == 0 ) return 0.0f;

			std::int64_t nSample1 = x / nPitch;
			std::int64_t remainder = x % nPitch;
			if ( remainder < 0 ) nSample1 -= 1;
			nSample1 *= nPitch;
			std::int64_t nSample2 = ( nSample1 + nPitch );

			// Linear interpolation
			float fBlend = ( float )( x - nSample1 ) / ( float )nPitch; // How far into the pitch are we
			float fSample = ( 1.0f - fBlend ) * ( this->getSeedValue( this->_seed, this->_primeX, ( std::int64_t )nSample1 ) ) +
				( fBlend ) * ( this->getSeedValue( this->_seed, this->_primeX, ( std::int64_t )nSample2 ) );

			fAccumulativeNoise += fSample * fScalingFactor;
			fAccumulativeScalingFactor += fScalingFactor;

			fScalingFactor = this->functionScalingFactor( fScalingFactor );
		}

		// Scaled to seed range
		float fNormalizedNoise = fAccumulativeNoise / fAccumulativeScalingFactor;

		return fNormalizedNoise;
	}

};