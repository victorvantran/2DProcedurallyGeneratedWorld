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


	static float valCoord( std::int64_t seed, std::int64_t xPrimed )
	{
		std::int64_t hash = hashFunction( seed, xPrimed );

		hash *= hash;
		hash ^= hash << 19;
		return hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );
	}


	static float getSeedValue( std::int64_t seed, std::int64_t primeX, std::int64_t x )
	{
		return std::abs( valCoord( seed, x * primeX ) );
	}


	float functionScalingFactor( float scalingFactor ) const
	{
		return scalingFactor / 2.0f;
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