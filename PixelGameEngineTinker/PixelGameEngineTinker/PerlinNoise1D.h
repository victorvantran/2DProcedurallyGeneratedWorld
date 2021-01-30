#pragma once

// Purposefully abstain from abstract class for micro-optimization since getPerlinValue will be called many times

#include <cmath>
#include <cstdint>

#include "olcPixelGameEngine.h"

class PerlinNoise1D
{
protected:
	std::int64_t _seed;
	std::int64_t _primeX;
	std::int32_t _octave;
	std::int64_t _width;

	static std::int64_t hashFunction( std::int64_t seed, std::int64_t xPrimed )
	{
		std::int64_t hash = seed ^ xPrimed;
		hash *= 0x27d4eb2d;
		return hash;
	}


	static long double valCoord( std::int64_t seed, std::int64_t xPrimed )
	{
		std::int64_t hash = hashFunction( seed, xPrimed );

		hash *= hash;
		hash ^= hash << 19;
		return hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );
	}


	static long double getSeedValue( std::int64_t seed, std::int64_t primeX, std::int64_t x )
	{
		return std::abs( valCoord( seed, x * primeX ) );
	}


	long double functionScalingFactor( long double scalingFactor ) const
	{
		return scalingFactor / 2.0;
	}

public:
	PerlinNoise1D()
		: _seed( 0 ), _primeX( 0 ), _octave( 0 ), _width( 0 )
	{

	}

	PerlinNoise1D( std::int64_t seed, std::int64_t primeX, std::int32_t octave, std::int64_t outputWidth )
		: _seed( seed ), _primeX( primeX ), _octave( octave ), _width( outputWidth )
	{

	}

	~PerlinNoise1D()
	{
	}


	std::int64_t getSeed() const
	{
		return this->_seed;
	}


	std::int64_t getPrimeX() const
	{
		return this->_primeX;
	}


	std::int64_t getWidth() const
	{
		// Returns width in cells
		return this->_width;
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