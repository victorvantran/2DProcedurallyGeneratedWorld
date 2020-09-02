/*
#include "Screen.h"



namespace olc {
	Screen::Screen()
	{

	}


	Screen::~Screen()
	{

	}


	void Screen::test()
	{
		return;
	}

	template<typename T>
	static void Screen::drawLayer( Layer<T>& layer, Atlas& atlas, vf2d cameraPosition, vi2d tileMatrixDimension, int scale )
	{
		/// renders a layer (there may be mutliple layers in a screen)

		vf2d offset = cameraPosition - vi2d{ cameraPosition }; // get the decimal point offset

		for ( int x = 0; x < tileMatrixDimension.x; x++ )
		{
			for ( int y = 0; y < tileMatrixDimension.y; y++ )
			{
				/// render out a subsection the matrix (offset by where the camera is)
				/// therefore, we only need to draw out what the camera can see and not the entire matrix every time
				Tile* tile = layer.getTile( x + ( int )cameraPosition.x, y + ( int )cameraPosition.y );
				if ( tile != nullptr && tile->exist )
				{
					/// get the float Position of the tile based on its matrix index and slight decimal point offset
					/// the slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen
					
					vf2d tilePosition = vf2d
					{ 
						vi2d
						{ 
							( (float)x - offset.x ) * (float)layer->getLayerDimension.x,
							( (float)y - offset.y ) * (float)layer->getLayerDimension.y
						}, 
					};

					// render the individual tile
					//pge->DrawPartialDecal
				}
			}
		}

		return;
	}


	template<typename T>
	static Pixel Screen::getLayerPixel( Layer<T>& layer, Atlas& atlas, vf2d pixelPosition )
	{
		/// return the olc::Pixel based on the position on the screen
		return BLANK;
	}


	template<typename T>
	static std::vector<Edge> Screen::extractEdgesFromLayer( Layer<T>& layer, vi2d tileIndex, vi2d tileSize )
	{
		/// returns the edges of an individual tile pinpointed by what tile index we are in (and size of the tile)
		std::vector<Edge> edges;
		return edges;
	}

}


*/