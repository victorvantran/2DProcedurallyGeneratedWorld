#pragma once

#include <cmath>
#include <cstdint>

#include "olcPixelGameEngine.h"

class PerlinNoise2D
{
private:
	static constexpr std::int64_t _primeX = 9743347811;
	static constexpr std::int64_t _primeY = 5747932867;
	static constexpr std::int64_t _nOutputWidth = 1024;
	static constexpr std::int64_t _nOutputHeight = 1536;

	std::int64_t seed = 4573457346;
public:
	PerlinNoise2D()
	{

	}

	~PerlinNoise2D()
	{
	}


	static std::int64_t hashFunction( std::int64_t seed, std::int64_t xPrimed, std::int64_t yPrimed )
	{
		std::int64_t hash = seed ^ xPrimed ^ yPrimed;

		hash *= 0x27d4eb2d;
		return hash;
	}


	static float valCoord( std::int64_t seed, std::int64_t xPrimed, std::int64_t yPrimed )
	{
		std::int64_t hash = hashFunction( seed, xPrimed, yPrimed );

		hash *= hash;
		hash ^= hash << 19; // Could be number of 1 bits in seed
		return hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );
	}


	// Getters
	static float getNoiseValue( std::int64_t seed, std::int64_t x, std::int64_t y )
	{
		return std::abs( valCoord( seed, x * _primeX, y * _primeY ) );
	}

	// Setters



	// Methods
	float getPerlinValue( std::int64_t x, std::int64_t y, std::int32_t nOctaves, float ( *functionScalingFactor )( float ) ) const
	{
		float fAccumulativeNoise = 0.0f;
		float fScalingFactor = 1.0f;
		float fAccumulativeScalingFactor = 0.0f;

		for ( std::int32_t octave = 0; octave < nOctaves; octave++ )
		{
			std::int32_t nPitch = this->_nOutputWidth >> octave;
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

			float fSampleT = ( 1.0f - fBlendX ) * ( getNoiseValue( seed, nSampleX1, nSampleY1 ) ) + fBlendX * ( getNoiseValue( seed, nSampleX2, nSampleY1 ) );
			float fSampleB = ( 1.0f - fBlendX ) * ( getNoiseValue( seed, nSampleX1, nSampleY2 ) ) + fBlendX * ( getNoiseValue( seed, nSampleX2, nSampleY2 ) );


			// Interpolate by Y axis
			fAccumulativeNoise += ( fBlendY * ( fSampleB - fSampleT ) + fSampleT ) * fScalingFactor;
			fAccumulativeScalingFactor += fScalingFactor;
			fScalingFactor = ( *functionScalingFactor )( fScalingFactor );
		}

		// Scaled to seed range
		float fNormalizedNoise = fAccumulativeNoise / fAccumulativeScalingFactor;

		return fNormalizedNoise;
	}


	void render( olc::PixelGameEngine& pge, std::int64_t startX, std::int64_t endX, std::int64_t startY, std::int64_t endY, std::int32_t nOctaves, float ( *functionScalingFactor )( float ) ) const
	{

		for ( std::int64_t tileY = startY; tileY < endY; tileY++ )
		{
			for ( std::int64_t tileX = startX; tileX < endX; tileX++ )
			{
				std::int64_t pixel_bw = ( int64_t )( this->getPerlinValue( tileX, tileY, 9, functionScalingFactor ) * 256 );
				const std::int64_t a = pixel_bw;

				if (
					( ( ( int )pixel_bw % 3 == 0 && ( int )pixel_bw % 8 == 0 ) ||
						( ( int )pixel_bw % 12 == 0 && ( int )pixel_bw % 3 == 0 ) ||
						( ( int )pixel_bw % 35 == 0 && ( int )pixel_bw % 3 == 0 )
						)
					||
					(
						( pixel_bw >= 14 && pixel_bw < 16 ) ||
						( pixel_bw >= 29 && pixel_bw < 32 ) ||
						( pixel_bw >= 62 && pixel_bw < 64 ) ||
						( pixel_bw >= 77 && pixel_bw < 80 ) ||

						( pixel_bw >= 125 && pixel_bw < 128 ) ||
						( pixel_bw >= 157 && pixel_bw < 160 ) ||
						( pixel_bw >= 175 && pixel_bw < 176 ) ||

						( pixel_bw >= 234 && pixel_bw < 240 ) ||
						( pixel_bw >= 253 && pixel_bw < 256 )
						)
					)
				{
					pge.Draw( tileX - startX, tileY - startY, olc::Pixel( 0, 0, 0, 255 ) );
				}

			}
		}
		return;

	}

};