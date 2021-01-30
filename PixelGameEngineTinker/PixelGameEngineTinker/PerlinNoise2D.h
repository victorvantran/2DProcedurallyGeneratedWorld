#pragma once

// Purposefully abstain from abstract class for micro-optimization since getPerlinValue will be called many times

#include <cmath>
#include <cstdint>

#include "olcPixelGameEngine.h"

class PerlinNoise2D
{
protected:
	std::int64_t _seed;
	std::int64_t _primeX; //32701125943;
	std::int64_t _primeY; //323536891;

	std::int64_t _octave;
	std::int64_t _width; // 1024
	std::int64_t _height; // 1536


	static std::int64_t hashFunction( std::int64_t seed, std::int64_t xPrimed, std::int64_t yPrimed )
	{
		std::int64_t hash = seed ^ xPrimed ^ yPrimed;
		hash *= 0x27d4eb2f;
		return hash;
	}


	static long double valCoord( std::int64_t seed, std::int64_t xPrimed, std::int64_t yPrimed )
	{
		std::int64_t hash = hashFunction( seed, xPrimed, yPrimed );

		hash *= hash;
		hash ^= hash << 19;
		return hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );
	}


	static long double getNoiseValue( std::int64_t seed, std::int64_t primeX, std::int64_t primeY, std::int64_t x, std::int64_t y )
	{
		return std::abs( valCoord( seed, x * primeX, y * primeY ) );
	}


	long double functionScalingFactor( long double scalingFactor ) const
	{
		return scalingFactor / 2.0;
	}
public:
	PerlinNoise2D()
		: _seed( 0 ), _primeX( 0 ), _primeY( 0 ),
		_octave( 0 ), _width( 0 ), _height( 0 )
	{

	}

	PerlinNoise2D( std::int64_t seed, std::int64_t primeX, std::int64_t primeY, std::int32_t octave, std::int64_t width, std::int64_t height )
		: _seed( seed ), _primeX( primeX ), _primeY( primeY ),
		_octave( octave ), _width( width ), _height( height )
	{

	}


	~PerlinNoise2D()
	{

	}


	long double getPerlinValue( std::int64_t x, std::int64_t y ) const
	{
		long double accumulativeNoise = 0.0f;
		long double scalingFactor = 1.0f;
		long double accumulativeScalingFactor = 0.0f;

		for ( std::int32_t octave = 0; octave < this->_octave; octave++ )
		{
			std::int32_t pitch = this->_width >> octave;
			if ( pitch == 0 ) return 0.0f;

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