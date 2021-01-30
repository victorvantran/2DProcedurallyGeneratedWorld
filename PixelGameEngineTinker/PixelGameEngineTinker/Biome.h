#pragma once

#include "BorealForest.h"
#include "SubtropicalDesert.h"
#include "TemperateGrassland.h"
#include "TemperateRainforest.h"
#include "TemperateSeasonalForest.h"
#include "TropicalRainforest.h"
#include "TropicalSeasonalForest.h"
#include "Tundra.h"
#include "Woodland.h"

enum class Biome
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