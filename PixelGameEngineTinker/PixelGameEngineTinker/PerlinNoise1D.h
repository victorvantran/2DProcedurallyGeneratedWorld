#pragma once

// Special thanks to Auburn's FastNoiseLite for hashing algorithm

#include <cmath>
#include <cstdint>
#include "olcPixelGameEngine.h"

class PerlinNoise1D
{
private:
	static constexpr std::int64_t _primeX = 9743347811;
	static constexpr std::int64_t _primeY = 5747932867;
	static constexpr std::int64_t _nOutputSize = 1024;

	std::int64_t seed = 4573457346;
public:
	PerlinNoise1D()
	{
	}

	~PerlinNoise1D()
	{
	}


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
		hash ^= hash << 19; // Could be number of 1 bits in seed
		return hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );
	}


	// Getters
	static float getNoiseValue( std::int64_t seed, std::int64_t x )
	{
		return std::abs( valCoord( seed, x * _primeX ) );
	}


	float getPerlinValue( std::int64_t x, std::int32_t nOctaves, float ( *functionScalingFactor )( float ) ) const
	{
		float fAccumulativeNoise = 0.0f;
		float fScalingFactor = 1.0f;
		float fAccumulativeScalingFactor = 0.0f;

		for ( std::int32_t octave = 0; octave < nOctaves; octave++ )
		{
			std::int32_t nPitch = this->_nOutputSize >> octave;
			if ( nPitch == 0 ) return 0.0f;

			std::int64_t nSample1 = x / nPitch;
			std::int64_t remainder = x % nPitch;
			if ( remainder < 0 ) nSample1 -= 1;
			nSample1 *= nPitch;
			std::int64_t nSample2 = ( nSample1 + nPitch );

			// Linear interpolation
			float fBlend = ( float )( x - nSample1 ) / ( float )nPitch; // How far into the pitch are we
			float fSample = ( 1.0f - fBlend ) * ( getNoiseValue( seed, ( std::int64_t )nSample1 ) ) + fBlend * ( getNoiseValue( seed, ( std::int64_t )nSample2 ) );

			fAccumulativeNoise += fSample * fScalingFactor;
			fAccumulativeScalingFactor += fScalingFactor;

			fScalingFactor = ( *functionScalingFactor )( fScalingFactor );
		}

		// Scaled to seed range
		float fNormalizedNoise = fAccumulativeNoise / fAccumulativeScalingFactor;

		return fNormalizedNoise;
	}


	// Methods
	void render( olc::PixelGameEngine& pge, std::int64_t startX, std::int64_t endX, std::int32_t nOctaves, float ( *functionScalingFactor )( float ) ) const
	{
		for ( std::int32_t x = startX; x < endX; x++ )
		{

			// Scaled to seed range
			float fNormalizedNoise = this->getPerlinValue( x, nOctaves, functionScalingFactor );
			int32_t y = -( fNormalizedNoise * ( pge.ScreenHeight() ) ) + ( float )( pge.ScreenHeight() );

			for ( std::int32_t tileY = y; tileY < pge.ScreenHeight(); tileY++ )
			{
				pge.SetPixelMode( olc::Pixel::NORMAL );
				pge.Draw( x - startX, tileY, olc::GREEN );
			}
		}



		return;
	}

};