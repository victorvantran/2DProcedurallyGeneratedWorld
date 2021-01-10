#pragma once

#include <vector>
#include "olcPixelGameEngine.h"

struct LightCastRow
{
	long double depth;
	long double startSlope;
	long double endSlope;

	LightCastRow( long double depth_, long double startSlope_, long double endSlope_ );
	~LightCastRow();

	std::vector< olc::v2d_generic<long double>> getTiles();
	LightCastRow getNext();
};
