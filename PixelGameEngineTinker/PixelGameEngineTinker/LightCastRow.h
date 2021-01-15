#pragma once

#include <vector>
#include "olcPixelGameEngine.h"

struct LightCastRow
{
	std::int64_t depth;
	long double startSlope;
	long double endSlope;

	LightCastRow( std::int64_t depth_, long double startSlope_, long double endSlope_ );
	~LightCastRow();

	std::vector< olc::v2d_generic<long double>> getTiles();
	LightCastRow getNext();
};
