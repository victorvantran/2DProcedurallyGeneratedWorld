#pragma once
#include "olcPixelGameEngine.h"
#include "Layer.h"
#include "Atlas.h"
#include "Tile.h"
#include "Edge.h"

namespace olc
{
	class Screen : public olc::PGEX
	{
	private:

	public:
		Screen();
		~Screen();


		template<typename T>
		static void drawLayer( Layer<T> &layer, Atlas &atlas, vf2d cameraPosition, vi2d tileMatrixDimension, int scale = 1);


		template<typename T>
		static Pixel getLayerPixel( Layer<T> &layer, Atlas& atlas, vf2d pixelPosition );


		template<typename T>
		static std::vector<Edge> extractEdgesFromLayer( Layer<T> &layer, vi2d tileIndex , vi2d tileSize );

	};
}

namespace olc {
	Screen::Screen()
	{

	}


	Screen::~Screen()
	{

	}


	template<typename T>
	static void Screen::drawLayer( Layer<T>& layer, Atlas& atlas, vf2d cameraPosition, vi2d tileMatrixDimension, int scale )
	{
		/// Renders a layer (there may be mutliple layers in a screen)
		/// Camera position is in matrix world [col, row]
		/// TileMatrixDimension is the matrix area of visible tiles [col, row]

		vf2d offset = cameraPosition - vi2d{ cameraPosition }; // get the decimal point offset

		for ( int row = 0; row < tileMatrixDimension.y + 1; row++ )
		{
			for ( int column = 0; column < tileMatrixDimension.x + 1; column++ )
			{
				/// Render out a subsection the matrix (offset by where the camera is)
				/// Therefore, we only need to draw out what the camera can see and not the entire matrix every time (added +1 to the for loops to add a little buffer)

				Tile* tile = layer.getTile( column + ( int )cameraPosition.x, row + ( int )cameraPosition.y );
				if ( tile != nullptr && tile->exist )
				{
					/// Get the float Position of the tile based on: its matrix index and slight decimal point offset 
					/// The slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen

					vf2d tilePosition = vf2d
					{
						vi2d
						{
							(int) (( ( float )column - offset.x ) * ( float )layer.getTileDimension().x),
							( int )( ( ( float )row - offset.y ) * ( float )layer.getTileDimension().y )
						},
					};

					/// Render the individual tile
					//pge->DrawPartialDecal
					pge->DrawPartialSprite(
						tilePosition.x + 0.5f - ( tilePosition.x < 0.0f ),
						tilePosition.y + 0.5f - ( tilePosition.y < 0.0f ),
						atlas.getTileSheet(),
						std::get<0>( atlas.mapping[tile->id] ),
						std::get<1>( atlas.mapping[tile->id] ),
						std::get<2>( atlas.mapping[tile->id] ),
						std::get<3>( atlas.mapping[tile->id] ),
						scale
					);
				}
			}
		}

		return;
	}


	template<typename T>
	static Pixel Screen::getLayerPixel( Layer<T>& layer, Atlas& atlas, vf2d pixelPosition )
	{
		/// Return the olc::Pixel based on the position on the screen
		return BLANK;
	}


	template<typename T>
	static std::vector<Edge> Screen::extractEdgesFromLayer( Layer<T>& layer, vi2d tileIndex, vi2d tileSize )
	{
		/// Returns the edges of an individual tile pinpointed by what tile index we are in (and size of the tile)
		std::vector<Edge> edges;
		return edges;
	}

}