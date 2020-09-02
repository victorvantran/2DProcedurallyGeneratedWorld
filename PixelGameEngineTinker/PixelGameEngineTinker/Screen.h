#pragma once
#include "olcPixelGameEngine.h"
#include "Layer.h"
#include "Atlas.h"
#include "Tile.h"
#include "Edge.h"

namespace olc
{
	class Screen
	{
	private:

	public:
		Screen();
		~Screen();

		template<typename T>
		static void drawLayer( Layer<T> &layer, Atlas &atlas, vf2d cameraPosition, vi2d tileMatrixDimension, int scale = 1  );


		template<typename T>
		static Pixel getLayerPixel( Layer<T>& layer, Atlas& atlas, vf2d pixelPosition );


		template<typename T>
		static std::vector<Edge> extractEdgesFromLayer( Layer<T>& layer, vi2d tileIndex , vi2d tileSize );
	};
}
