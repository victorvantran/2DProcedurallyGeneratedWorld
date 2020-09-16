#pragma once
#include "olcPixelGameEngine.h"
#include "Layer.h"
#include "Atlas.h"
#include "Tile.h"
#include "Edge.h"
#include "World.h"
#include "WorldChunk.h"
#include "Character.h"
#include "MarchingSquares.h"

class Screen : public olc::PGEX
{
private:
	olc::vi2d _screenDimension;
	olc::vi2d _tileDimension;
public:
	Screen();
	~Screen();

	//template<typename T>
	//static void drawWorldChunk( WorldChunk* worldChunk, olc::vf2d cameraPosition, olc::vi2d tileMatrixDimension, float scale = 1.0f );

	template<typename T>
	static void drawLayer( Layer<T>* layer, Atlas* atlas, olc::vf2d cameraPosition, olc::vi2d tileMatrixDimension, float scale = 1.0f);


	static void drawWorldChunkBackground( WorldChunk& worldChunk, olc::vf2d cameraPosition, float scale = 1.0f )
	{
		// Draws the background of a world chunk
		// [!] Temporary: Need to improve so that it pans with the camera
		Atlas worldChunkAtlas = worldChunk.getAtlas();

		if ( worldChunkAtlas.getDecalBackground() != nullptr )
		{
			pge->DrawDecal
			(
				worldChunk.getPosition() * settings::RESOLUTION::TILE_DIMENSION, // + cameraPosition
				worldChunkAtlas.getDecalBackground(),
				olc::vf2d{ scale*0.5f, scale*0.5f }
			);
			
			olc::vf2d offset = cameraPosition - olc::vi2d{ cameraPosition };
		}

		return;
	}



	static void drawWorld( World& world, olc::vf2d cameraPosition, olc::vi2d scopeDimension, float scale = 1.0f )
	{
		/// Renders all world chunks
		/// Camera position is in matrix world [col, row]
		/// scopeDimension is the matrix area of visible tiles [col, row]


		// [!] Temporoary: Draw background of the worldchunk
		// Can detect all world chunks and draw all background by checking all 4 points of the camera with cameraPosition and scopeDimension, but for now wait until we overhaul worldChunk generation
		olc::vi2d worldChunkIndex = olc::vi2d{ ( int )cameraPosition.x, ( int )cameraPosition.y };
		WorldChunk* worldChunk = world.getWorldChunkFromIndex( worldChunkIndex );

		if ( worldChunk != nullptr )
		{
			Screen::drawWorldChunkBackground( *worldChunk, cameraPosition, scale );
		}



		olc::vf2d offset = cameraPosition - olc::vi2d{ cameraPosition };
		for ( int row = -1; row < scopeDimension.y + 1; row++ )
		{
			for ( int column = -1; column < scopeDimension.x + 1; column++ )
			{

				olc::vi2d tileIndex = olc::vi2d{ column + ( int )cameraPosition.x, row + ( int )cameraPosition.y };
				//olc::vi2d tilePixelPosition = olc::vi2d{ tileIndex.x * world.getTileDimension().x, tileIndex.y * world.getTileDimension().y };

				/// Search for the WorldChunk that holds the given tile in the tileIndex coordinate 
				WorldChunk* worldChunk = world.getWorldChunkFromIndex( tileIndex );

				/// WorldChunk cannot be found (perhaps does not exist or not loaded) for a certain tile index; so skip
				if ( worldChunk == nullptr )
				{
					continue;
				}

				Atlas worldChunkAtlas = worldChunk->getAtlas();

				Tile* tile = worldChunk->getTileFromIndex( tileIndex );

				if ( tile != nullptr && tile->exist )
				{
					/// Get the float Position of the tile based on: its matrix index and slight decimal point offset 
					/// The slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen
					olc::vf2d tilePosition = olc::vf2d
					{
						olc::vi2d
						{
							( int )( ( ( float )column - offset.x ) * ( float )settings::RESOLUTION::TILE_DIMENSION.x ),
							( int )( ( ( float )row - offset.y ) * ( float )settings::RESOLUTION::TILE_DIMENSION.y )
						},
					};


					int blockFamilyColumns = ( int )( worldChunkAtlas.getSpriteTileSheet()->width ) / ( settings::RESOLUTION::TILE_DIMENSION.x * 4 );


					int blockFamilyX = ( tile->id % blockFamilyColumns ) * 4;
					int blockFamilyY = ( tile->id / blockFamilyColumns ) * 4;

					int blockFamilyMemberX = 15 % 4;
					int blockFamilyMemberY = 15 / 4;

					int blockX = blockFamilyX + blockFamilyMemberX;
					int blockY = blockFamilyY + blockFamilyMemberY;

					int linearizedIndex = blockY * settings::RESOLUTION::TILE_DIMENSION.x + blockX;


					/// Render the individual tile
					pge->DrawPartialDecal(
						olc::vf2d{ tilePosition.x + 0.5f - ( tilePosition.x < 0.0f ), tilePosition.y + 0.5f - ( tilePosition.y < 0.0f ) },
						//olc::vf2d{ tilePosition.x, tilePosition.y },
						worldChunkAtlas.getDecalTileSheet(),
						//olc::vf2d{ ( float )std::get<0>( worldChunkAtlas.mapping[tile->id] ), ( float )std::get<1>( worldChunkAtlas.mapping[tile->id] ) },
						olc::vf2d{ ( float )std::get<0>( worldChunkAtlas.mapping[linearizedIndex] ), ( float )std::get<1>( worldChunkAtlas.mapping[linearizedIndex] ) },
						olc::vf2d{ settings::RESOLUTION::TILE_DIMENSION }, // can shorten the size of the mapping if we know that all the blocks are same size (no redundant emplacing]
						//olc::vf2d{ ( float )std::get<2>( atlas->mapping[tile->id] ), ( float )std::get<3>( atlas->mapping[tile->id] ) },
						olc::vf2d{ scale, scale }
					);

				}
			}
		}
		return;
	}



	static void drawWorldMarchinSquares( World& world, olc::vf2d cameraPosition, olc::vi2d scopeDimension, float scale = 1.0f )
	{
		/// Renders all world chunks
		/// Camera position is in matrix world [col, row]
		/// scopeDimension is the matrix area of visible tiles [col, row]


		// [!] Temporoary: Draw background of the worldchunk
		// Can detect all world chunks and draw all background by checking all 4 points of the camera with cameraPosition and scopeDimension, but for now wait until we overhaul worldChunk generation
		olc::vi2d worldChunkIndex = olc::vi2d{ ( int )cameraPosition.x, ( int )cameraPosition.y };
		WorldChunk* worldChunk = world.getWorldChunkFromIndex( worldChunkIndex );
		
		if ( worldChunk != nullptr )
		{
			Screen::drawWorldChunkBackground( *worldChunk, cameraPosition, scale );
		}

		

		olc::vf2d offset = cameraPosition - olc::vi2d{ cameraPosition };
		for ( int row = -1; row < scopeDimension.y + 1; row++ )
		{
			for ( int column = -1; column < scopeDimension.x + 1; column++ )
			{

				olc::vi2d tileIndex = olc::vi2d{ column + ( int )cameraPosition.x, row + ( int )cameraPosition.y };
				//olc::vi2d tilePixelPosition = olc::vi2d{ tileIndex.x * world.getTileDimension().x, tileIndex.y * world.getTileDimension().y };

				/// Search for the WorldChunk that holds the given tile in the tileIndex coordinate 
				WorldChunk* worldChunk = world.getWorldChunkFromIndex( tileIndex );

				/// WorldChunk cannot be found (perhaps does not exist or not loaded) for a certain tile index; so skip
				if ( worldChunk == nullptr )
				{
					continue;
				}

				Atlas worldChunkAtlas = worldChunk->getAtlas();

				Tile* tile = worldChunk->getTileFromIndex( tileIndex );
				
				if ( tile != nullptr && tile->exist )
				{
					/// Get the float Position of the tile based on: its matrix index and slight decimal point offset 
					/// The slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen
					olc::vf2d tilePosition = olc::vf2d
					{
						olc::vi2d
						{
							( int )( ( ( float )column - offset.x ) * ( float )settings::RESOLUTION::TILE_DIMENSION.x ),
							( int )( ( ( float )row - offset.y ) * ( float )settings::RESOLUTION::TILE_DIMENSION.y )
						},
					};

					
					int blockFamilyColumns = (int)( worldChunkAtlas.getSpriteTileSheet()->width ) / ( settings::RESOLUTION::TILE_DIMENSION.x * 4 );
					

					int blockFamilyX = ( tile->id % blockFamilyColumns ) * 4;
					int blockFamilyY = ( tile->id / blockFamilyColumns ) * 4;

					int blockFamilyMemberX = tile->configuration % 4;
					int blockFamilyMemberY = tile->configuration / 4;

					int blockX = blockFamilyX + blockFamilyMemberX;
					int blockY = blockFamilyY + blockFamilyMemberY;

					int linearizedIndex = blockY * settings::RESOLUTION::TILE_DIMENSION.x + blockX;


					linearizedIndex = blockFamilyMemberY * settings::RESOLUTION::TILE_DIMENSION.x + blockFamilyMemberX;

					
					/// Render the individual tile
					pge->DrawPartialDecal(
						 olc::vf2d{ tilePosition.x + 0.5f - ( tilePosition.x < 0.0f ), tilePosition.y + 0.5f - ( tilePosition.y < 0.0f ) },
						//olc::vf2d{ tilePosition.x, tilePosition.y },
						worldChunkAtlas.getDecalTileSheet(),
						//olc::vf2d{ ( float )std::get<0>( worldChunkAtlas.mapping[tile->id] ), ( float )std::get<1>( worldChunkAtlas.mapping[tile->id] ) },
						olc::vf2d{ ( float )std::get<0>( worldChunkAtlas.mapping[linearizedIndex] ), ( float )std::get<1>( worldChunkAtlas.mapping[linearizedIndex] ) },
						olc::vf2d{ settings::RESOLUTION::TILE_DIMENSION }, // can shorten the size of the mapping if we know that all the blocks are same size (no redundant emplacing]
						//olc::vf2d{ ( float )std::get<2>( atlas->mapping[tile->id] ), ( float )std::get<3>( atlas->mapping[tile->id] ) },
						olc::vf2d{ scale, scale }
					);

				}
			}
		}
		return;
	}


	// [!] may not need
	static void drawWorldChunk( WorldChunk& worldChunk, olc::vf2d cameraPosition, olc::vi2d scopeDimension, float scale = 1.0f )
	{
		/// Renders a world chunk
		/// Camera position is in matrix world [col, row]
		/// scopeDimension is the matrix area of visible tiles [col, row]


		Layer<Tile> worldChunkLayer = worldChunk.getLayer();
		Atlas worldChunkAtlas = worldChunk.getAtlas();


		olc::vf2d offset = cameraPosition - olc::vi2d{ cameraPosition }; // get the decimal point offset
		for ( int row = 0; row < scopeDimension.y + 1; row++ )
		{
			for ( int column = 0; column < scopeDimension.x + 1; column++ )
			{
				/// Render out a subsection the matrix (offset by where the camera is)
				/// Therefore, we only need to draw out what the camera can see and not the entire matrix every time (added +1 to the for loops to add a little buffer)

				Tile* tile = worldChunk.getTileFromIndex( column + ( int )cameraPosition.x, row + ( int )cameraPosition.y );
				if ( tile != nullptr && tile->exist )
				{
					/// Get the float Position of the tile based on: its matrix index and slight decimal point offset 
					/// The slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen
					olc::vf2d tilePosition = olc::vf2d
					{
						olc::vi2d
						{
							( int )( ( ( float )column - offset.x ) * ( float )settings::RESOLUTION::TILE_DIMENSION.x ),
							( int )( ( ( float )row - offset.y ) * ( float )settings::RESOLUTION::TILE_DIMENSION.y )
						},
					};

					/// Render the individual tile
					pge->DrawPartialDecal(
						olc::vf2d{ tilePosition.x + 0.5f - ( tilePosition.x < 0.0f ), tilePosition.y + 0.5f - ( tilePosition.y < 0.0f ) },
						worldChunkAtlas.getDecalTileSheet(),
						olc::vf2d{ ( float )std::get<0>( worldChunkAtlas.mapping[tile->id] ), ( float )std::get<1>( worldChunkAtlas.mapping[tile->id] ) },
						olc::vf2d{ settings::RESOLUTION::TILE_DIMENSION },
						olc::vf2d{ scale, scale }
					);

				}
			}
		}

		return;
	}


	static void drawCharacter( Character& character, olc::vf2d cameraPosition, float scale = 1.0f )
	{
		/// Renders the character
		/// [!] Temporary use original tile size; need to add member variables for screen: TileDimension...
		pge->DrawRect( ( character.getCurrPosition() - character.getHalfSize() - cameraPosition ) * settings::ATLAS::TILE_DIMENSION, character.getHalfSize() * 2.0f * settings::ATLAS::TILE_DIMENSION * scale, olc::WHITE );

		pge->DrawDecal(
			olc::vf2d{ olc::vi2d{ ( character.getCurrPosition() - character.getHalfSize() - cameraPosition ) * settings::ATLAS::TILE_DIMENSION } } + olc::vf2d{ 0.5f, 0.5f }, // added 0.5f offset due to decal being off, not the collision detection
			character.getDecal(),
			olc::vf2d{ ( 8.0f / character.getDecal()->sprite->width ) * character.getHalfSize().x * 2 * scale, ( 8.0f / character.getDecal()->sprite->height ) * character.getHalfSize().y * 2 * scale }
		);
		
		return;
	}


	template<typename T>
	static olc::Pixel getLayerPixel( Layer<T> &layer, Atlas& atlas, olc::vf2d pixelPosition );

	template<typename T>
	static std::vector<Edge> extractEdgesFromLayer( Layer<T> &layer, olc::vi2d tileIndex , olc::vi2d tileSize );

};



Screen::Screen()
{

}


Screen::~Screen()
{

}




template<typename T>
static void Screen::drawLayer( Layer<T>* layer, Atlas* atlas, olc::vf2d cameraPosition, olc::vi2d tileMatrixDimension, float scale )
{
	/// Renders a layer (there may be mutliple layers in a screen)
	/// Camera position is in matrix world [col, row]
	/// TileMatrixDimension is the matrix area of visible tiles [col, row]

	olc::vf2d offset = cameraPosition - olc::vi2d{ cameraPosition }; // get the decimal point offset
	for ( int row = 0; row < tileMatrixDimension.y + 1; row++ )
	{
		for ( int column = 0; column < tileMatrixDimension.x + 1; column++ )
		{
			/// Render out a subsection the matrix (offset by where the camera is)
			/// Therefore, we only need to draw out what the camera can see and not the entire matrix every time (added +1 to the for loops to add a little buffer)

			Tile* tile = layer->getCell( column + ( int )cameraPosition.x, row + ( int )cameraPosition.y );
			if ( tile != nullptr && tile->exist )
			{
				/// Get the float Position of the tile based on: its matrix index and slight decimal point offset 
				/// The slight decimal point is to make sure we do not miss rendering tiles that are slightly off screen
				olc::vf2d tilePosition = olc::vf2d
				{
					olc::vi2d
					{
						( int )( ( ( float )column - offset.x ) * ( float )settings::RESOLUTION::TILE_DIMENSION.x),
						( int )( ( ( float )row - offset.y ) * ( float )settings::RESOLUTION::TILE_DIMENSION.y )
					},
				};
				
				/// Render the individual tile
				pge->DrawPartialDecal(
					olc::vf2d{ tilePosition.x + 0.5f - ( tilePosition.x < 0.0f ), tilePosition.y + 0.5f - ( tilePosition.y < 0.0f ) },
					atlas->getDecalTileSheet(),
					olc::vf2d{ ( float )std::get<0>( atlas->mapping[tile->id] ), ( float )std::get<1>( atlas->mapping[tile->id] ) },
					olc::vf2d{ settings::RESOLUTION::TILE_DIMENSION },
					olc::vf2d{ scale, scale }
				);

			}
		}
	}

	return;
}



template<typename T>
static olc::Pixel Screen::getLayerPixel( Layer<T>& layer, Atlas& atlas, olc::vf2d pixelPosition )
{
	/// Return the olc::Pixel based on the position on the screen
	return olc::BLANK;
}


template<typename T>
static std::vector<Edge> Screen::extractEdgesFromLayer( Layer<T>& layer, olc::vi2d tileIndex, olc::vi2d tileSize )
{
	/// Returns the edges of an individual tile pinpointed by what tile index we are in (and size of the tile)
	std::vector<Edge> edges;
	return edges;
}
