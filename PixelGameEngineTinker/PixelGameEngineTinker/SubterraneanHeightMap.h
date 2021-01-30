#pragma once

#include "PerlinNoise1D.h"

class SubterraneanHeightMap : public PerlinNoise1D
{
private:
	long double functionScalingFactor( long double scalingFactor ) const
	{
		return scalingFactor / 2.0;
	}
public:
	SubterraneanHeightMap()
		: PerlinNoise1D()
	{

	}

	SubterraneanHeightMap( std::int64_t seed, std::int64_t primeX, std::int32_t octave, std::int64_t width )
		: PerlinNoise1D( seed, primeX, octave, width )
	{

	}

	~SubterraneanHeightMap()
	{
	}


	long double getPerlinValue( std::int64_t x ) const
	{
		long double accumulativeNoise = 0.0;
		long double scalingFactor = 1.0;
		long double accumulativeScalingFactor = 0.0;

		for ( std::int32_t octave = 0; octave < this->_octave; octave++ )
		{
			std::int32_t pitch = this->_width >> octave;
			if ( pitch == 0 ) return 0.0;

			std::int64_t nSample1 = x / pitch;
			std::int64_t remainder = x % pitch;
			if ( remainder < 0 ) nSample1 -= 1;
			nSample1 *= pitch;
			std::int64_t nSample2 = ( nSample1 + pitch );

			// Linear interpolation
			long double fBlend = ( long double )( x - nSample1 ) / ( long double )pitch; // How far into the pitch are we
			long double fSample = ( 1.0 - fBlend ) * ( this->getSeedValue( this->_seed, this->_primeX, ( std::int64_t )nSample1 ) ) +
				( fBlend ) * ( this->getSeedValue( this->_seed, this->_primeX, ( std::int64_t )nSample2 ) );

			accumulativeNoise += fSample * scalingFactor;
			accumulativeScalingFactor += scalingFactor;

			scalingFactor = this->functionScalingFactor( scalingFactor );
		}

		// Scaled to seed range
		long double normalizedNoise = accumulativeNoise / accumulativeScalingFactor;

		return normalizedNoise;
	}
};