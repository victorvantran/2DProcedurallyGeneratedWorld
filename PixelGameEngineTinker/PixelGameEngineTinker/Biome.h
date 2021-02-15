#pragma once



#include <utility>
#include "Tile.h"
#include "Foliage.h"
#include "FoliageMap.h"

enum class BiomeIdentity
{
	TemperateSeasonalForest,
	TropicalSeasonalForest,
	TropicalRainforest,
	Woodland,
	TemperateGrassland,
	SubtropicalDesert,
	BorealForest,
	TemperateRainforest,
	Tundra,
	Unknown,
	count
};




class Biome
{
private:
protected:
	FoliageMap _foliageMap;
	std::pair<long double, long double> _temperatureRange;
	std::pair<long double, long double> _precipitationRange;

	static std::uint32_t randomHash32( std::uint32_t x ) {
		x = ( ( x >> 16 ) ^ x ) * 0x45d9f3b;
		x = ( ( x >> 16 ) ^ x ) * 0x45d9f3b;
		x = ( x >> 16 ) ^ x;
		return x;
	}
public:
	Biome() {}
	Biome( const FoliageMap& foliageMap, const std::pair<long double, long double>& temperatureRange, const std::pair<long double, long double>& precipitationRange ) :
		_foliageMap( foliageMap ), _temperatureRange( temperatureRange ), _precipitationRange( precipitationRange ) {}
	~Biome() {}

	static long double normalizeHistogram( long double value )
	{
		if ( value <= 0.324 )
		{
			return 6.15421 * ( value * value * value ) - 1.70354 * ( value * value ) + 0.215019f * ( value );
		}
		else if ( value >= 0.324 && value <= 0.676 )
		{
			return -16.6757 * ( value * value * value ) + 25.0136 * ( value * value ) - 9.71832 * ( value )+1.19033;
		}
		else // if ( value >= 0.676 )
		{
			return 6.15421 * ( value * value * value ) - 16.7591 * ( value * value ) + 15.2706 * ( value )-3.66557;
		}
	}

	std::pair<long double, long double> getTemperatureRange()
	{
		return this->_temperatureRange;
	}

	std::pair<long double, long double> getPrecipiationRange()
	{
		return this->_precipitationRange;
	}


	long double getTemperatureLocalNormalizedValue( long double temperatureNormalizedValue )
	{
		return ( this->_temperatureRange.first - temperatureNormalizedValue ) / ( this->_temperatureRange.first - this->_temperatureRange.second );
	}


	long double getPrecipitationLocalNormalizedValue( long double precipitationNormalizedValue )
	{
		// std::cout << "( " << this->_precipitationRange.first << ", " << this->_precipitationRange.second << ")" << std::endl;
		return ( this->_precipitationRange.first - precipitationNormalizedValue ) / ( this->_precipitationRange.first - this->_precipitationRange.second );
	}


	FoliageIdentity getFoliage( std::int64_t tileX )
	{
		return FoliageIdentity::Void;
	}

};


