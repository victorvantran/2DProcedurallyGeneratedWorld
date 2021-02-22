#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "Assets.h"
#include "World.h"
#include "Tile.h"
#include "TileRender.h"
#include "Camera.h"
#include "Player.h"
#include "Zombie.h"


#include <cmath> // std::floorf

#include <thread>
#include <future>


class Example : public olc::PixelGameEngine
{
public:
	const static std::uint16_t pixelSize = Settings::Screen::PIXEL_SIZE;
	const static std::uint16_t screenWidth = Settings::Screen::SCREEN_PIXEL_WIDTH;
	const static std::uint16_t screenHeight = Settings::Screen::SCREEN_PIXEL_HEIGHT;
	const static std::uint16_t cellSize = Settings::Screen::CELL_PIXEL_SIZE;


public:
	World* world = nullptr;
	Player* player = nullptr;
	DynamicObject* enemy1 = nullptr;
	DynamicObject* enemy2 = nullptr;
	DynamicObject* enemy3 = nullptr;
	Camera* camera = nullptr;

	olc::vi2d decalGridDimension;

	Tile* tiles; // exist, id, configuration
	int* configurations;

	olc::vi2d gridDimension;


	TileIdentity tileId = TileIdentity::Stone;



	//
	olc::v2d_generic<std::int64_t> topLeftScan;
	olc::v2d_generic<std::int64_t> bottomRightScan;




	// Temporary render system [!]
	olc::Sprite* daySprite;
	olc::Decal* dayDecal;

	olc::Sprite* nightSprite;
	olc::Decal* nightDecal;

	olc::Sprite* sunSprite;
	olc::Decal* sunDecal;

	olc::Sprite* landscapeSprite;
	olc::Decal* landscapeDecal;



	olc::Sprite* caveBackgroundSprite;
	olc::Decal*	caveBackgroundDecal;



	olc::Sprite* alphaSprite;
	olc::Decal* alphaDecal;
private:


public:
	Example()
	{
		sAppName = "Example";
	}

	~Example()
	{
		delete world;
		delete camera;
		delete player;


		// Delete temporary renders [!]
		delete this->daySprite;
		delete this->dayDecal;

		delete this->nightSprite;
		delete this->nightDecal;


		delete this->sunSprite;
		delete this->sunDecal;


		delete this->landscapeSprite;
		delete this->landscapeDecal;


		delete this->caveBackgroundSprite;
		delete this->caveBackgroundDecal;

		delete this->alphaSprite;
		delete this->alphaDecal;
	}

public:
	bool OnUserCreate() override
	{
		// Create Decal on the render thread with the openGL context. Can dynamically create decals on update PGE engine with a more current OpenGL version
		this->daySprite = new olc::Sprite( "./background_sky.png" );
		this->dayDecal = new olc::Decal( this->daySprite );

		this->nightSprite = new olc::Sprite( "./background_night_sky.png" );
		this->nightDecal = new olc::Decal( this->nightSprite );

		this->landscapeSprite = new olc::Sprite( "./background_mountain.png" );
		this->landscapeDecal = new olc::Decal( this->landscapeSprite );

		this->sunSprite = new olc::Sprite( "./sun.png" );
		this->sunDecal = new olc::Decal( this->sunSprite );


		this->caveBackgroundSprite = new olc::Sprite( "./cave_background.png" );
		this->caveBackgroundDecal = new olc::Decal( this->caveBackgroundSprite );


		this->alphaSprite = new olc::Sprite( "./alpha_spritesheet.png" );
		this->alphaDecal = new olc::Decal( this->alphaSprite );

		loadAssets();
		createWorld();
		createPlayer();
		createEnemy();
		return true;
	}


	bool OnUserUpdate( float fElapsedTime ) override
	{
		// Camera Debug
		long double mouseX = ( long double )GetMouseX();
		long double mouseY = ( long double )GetMouseY();


		//long double panSpeed = 20.0f;
		long double panSpeed = 20.0f;


		if ( GetKey( olc::Key::UP ).bPressed || GetKey( olc::Key::UP ).bHeld )
		{
			camera->panY( -panSpeed * (long double)fElapsedTime );
		}
		if ( GetKey( olc::Key::DOWN ).bPressed || GetKey( olc::Key::DOWN ).bHeld )
		{
			camera->panY( panSpeed * ( long double )fElapsedTime );
		}
		if ( GetKey( olc::Key::LEFT ).bPressed || GetKey( olc::Key::LEFT ).bHeld )
		{
			camera->panX( -panSpeed * ( long double )fElapsedTime );
		}
		if ( GetKey( olc::Key::RIGHT ).bPressed || GetKey( olc::Key::RIGHT ).bHeld )
		{
			camera->panX( panSpeed * ( long double )fElapsedTime );
		}
		if ( GetKey( olc::Key::T ).bPressed || GetKey( olc::Key::T ).bHeld )
		{
			camera->pan( panSpeed * ( long double )fElapsedTime, panSpeed * ( long double )fElapsedTime );
		}

		/*
		if ( GetKey( olc::Key::W ).bPressed )
		{
			camera->panY( -1.0f );
		}
		if ( GetKey( olc::Key::S ).bPressed )
		{
			camera->panY( 1.0f );
		}
		if ( GetKey( olc::Key::D ).bPressed )
		{
			camera->panY( 1.0f );
		}
		if ( GetKey( olc::Key::A ).bPressed )
		{
			camera->panY( 1.0f );
		}
		*/

		if ( GetKey( olc::Key::F1 ).bPressed )
		{
			camera->panY( -( panSpeed * 100 ) );
		}
		if ( GetKey( olc::Key::F2 ).bPressed )
		{
			camera->panY( ( panSpeed * 100 ) );
		}
		if ( GetKey( olc::Key::F3 ).bPressed )
		{
			camera->panX( -( panSpeed * 100 ) );
		}
		if ( GetKey( olc::Key::F4 ).bPressed )
		{
			camera->panX( ( panSpeed * 100 ) );
		}

		// Zoom in
		if ( GetKey( olc::Key::Z ).bPressed || GetKey( olc::Key::Z ).bHeld )
		{
			long double zoomScale = 1.0 + ( 1.0 * fElapsedTime );
			this->camera->zoom( zoomScale );
		}

		// Zoom out
		if ( GetKey( olc::Key::X ).bPressed || GetKey( olc::Key::X ).bHeld )
		{
			long double zoomScale = 1.0 - ( 1.0 * fElapsedTime );
			this->camera->zoom( zoomScale );
		}

		// Reset camera
		if ( GetKey( olc::Key::C ).bPressed || GetKey( olc::Key::C ).bHeld )
		{
			this->camera->setZoom( 1.0 );
		}



		if ( GetKey( olc::Key::K1 ).bPressed )
		{
			tileId = TileIdentity::Dirt;
		}
		else if ( GetKey( olc::Key::K2 ).bPressed )
		{
			tileId = TileIdentity::Stone;
		}
		else if ( GetKey( olc::Key::K3 ).bPressed )
		{
			tileId = TileIdentity::Sand;
		}
		else if ( GetKey( olc::Key::K4 ).bPressed )
		{
			tileId = TileIdentity::Torch;
		}
		else if ( GetKey( olc::Key::K5 ).bPressed )
		{
			tileId = TileIdentity::MossDirt;
		}
		else if ( GetKey( olc::Key::K6 ).bPressed )
		{
			tileId = TileIdentity::Permafrost;
		}
		else if ( GetKey( olc::Key::K7 ).bPressed )
		{
			tileId = TileIdentity::MossStone;
		}
		else if ( GetKey( olc::Key::K8 ).bPressed )
		{
			tileId = TileIdentity::Gravel;
		}
		else if ( GetKey( olc::Key::K9 ).bPressed )
		{
			tileId = TileIdentity::CrimsonMapleLeaves;
		}
		else if ( GetKey( olc::Key::K0 ).bPressed )
		{
			tileId = TileIdentity::MapleLog;
		}

		/*
		if ( GetKey( olc::Key::K1 ).bPressed )
		{
			tileId = TileIdentity::Water;
		}
		else if ( GetKey( olc::Key::K2 ).bPressed )
		{
			tileId = TileIdentity::Stone;
		}
		else if ( GetKey( olc::Key::K3 ).bPressed )
		{
			tileId = TileIdentity::Dirt;
		}
		else if ( GetKey( olc::Key::K4 ).bPressed )
		{
			tileId = TileIdentity::Sand;
		}
		else if ( GetKey( olc::Key::K5 ).bPressed )
		{
			tileId = TileIdentity::ElmBark;
		}
		else if ( GetKey( olc::Key::K6 ).bPressed )
		{
			tileId = TileIdentity::ElmLeaves;
		}
		else if ( GetKey( olc::Key::K7 ).bPressed )
		{
			tileId = TileIdentity::MapleBark;
		}
		else if ( GetKey( olc::Key::K8 ).bPressed )
		{
			tileId = TileIdentity::MapleLeaves;
		}
		else if ( GetKey( olc::Key::K9 ).bPressed )
		{
			tileId = TileIdentity::Torch;
		}
		*/



		// Insert to world Debug 
		long double tilePositionX;
		long double tilePositionY;

		this->camera->screenToWorld( GetMouseX(), GetMouseY(), tilePositionX, tilePositionY );

		olc::v2d_generic<std::int64_t> tileIndex = olc::v2d_generic<std::int64_t>{
			( std::int64_t )( std::floor( tilePositionX ) ),
			( std::int64_t )( std::floor( tilePositionY ) )
		};



		if ( GetKey( olc::Key::P ).bPressed || GetKey( olc::Key::P ).bHeld )
		{
			world->insert( static_cast< TileIdentity >( tileId ), tileIndex.x, tileIndex.y, 5, 5 );
		}


		if ( GetKey( olc::Key::O ).bPressed || GetKey( olc::Key::O ).bHeld )
		{
			Tile* rTile = this->world->getTile( tilePositionX, tilePositionY );
			if ( rTile != nullptr )
			{
				world->remove( ( rTile->getId() ), tileIndex.x, tileIndex.y, 5, 5 );
			}
			//world->remove( static_cast< TileIdentity >( tileId ), tileIndex.x, tileIndex.y, 5, 5 );
		}

		if ( GetKey( olc::Key::L ).bPressed )
		{

			//const Tile* tile = this->world->getTile( tileIndex.x, tileIndex.y );
			//std::cout << (int)tile->getType() << std::endl;
			const Tile* tile = this->world->getTile( tilePositionX, tilePositionY );
			std::cout << (int)tile->getTileBlobMapIndex() << std::endl;

			//std::cout << tileIndex.x << ", " << tileIndex.y << std::endl;
			//world->insertTorch( tileIndex.x, tileIndex.y, tileId );
			// insert -> insertTile
			//		  -> insertTorch
			//		  -> insert...

			// emit static light... iterate through all worldChunks for all lightSources. See if lightsource radial distance are within camera view. emit
		}


		if ( GetKey( olc::Key::R ).bPressed || GetKey( olc::Key::R ).bHeld )
		{
			std::cout << this->player->getCharacter().getAABB().getCenter().x << ", " << this->player->getCharacter().getAABB().getCenter().y << std::endl;

		}


		if ( GetKey( olc::Key::H ).bPressed || GetKey( olc::Key::B ).bHeld )
		{
			std::cout << (int)world->getTile( tileIndex.x, tileIndex.y )->getBorders() << std::endl;
			//std::cout << (int)world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::North ) << std::endl;
			//std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getId() << std::endl;
			//world->insertLightTile( tileIndex.x, tileIndex.y, r, g, b, 255, 20 );
		}



		if ( GetKey( olc::Key::V ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorders() << std::endl;
		}


		if ( GetKey( olc::Key::N ).bPressed )
		{
			this->topLeftScan = olc::v2d_generic<std::int64_t>{ tileIndex.x, tileIndex.y };
			std::cout << "( " << this->topLeftScan.x << ", " << this->topLeftScan.y << " )" << std::endl;
		}

		if ( GetKey( olc::Key::M ).bPressed )
		{
			this->bottomRightScan = olc::v2d_generic<std::int64_t>{ tileIndex.x, tileIndex.y };
			std::cout << "( " << this->bottomRightScan.x << ", " << this->bottomRightScan.y << " )" << std::endl;
		}


		if ( GetKey( olc::Key::B ).bPressed )
		{
			// std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorders() << std::endl;

			/*
			for ( int row = 0; row < 32; row++ )
			{
				for ( int col = 0; col < 32; col++ )
				{
					std::cout << "[" << ( int )world->getTile( tileIndex.x + col, tileIndex.y + row )->getId() << "]" << ", ";
				}
				std::cout << std::endl;
			}
			*/

			for ( std::int64_t y = this->topLeftScan.y; y <= bottomRightScan.y; y++ )
			{
				for ( std::int64_t x = this->topLeftScan.x; x <= bottomRightScan.x; x++ )
				{
					std::cout << "[" << ( int )world->getTile( x, y )->getId() << "]" << ", ";
				}
				std::cout << std::endl;
			}

		}



		if ( GetKey( olc::Key::NP7 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::NorthWest ) << std::endl;
		}
		if ( GetKey( olc::Key::NP8 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::North ) << std::endl;
		}
		if ( GetKey( olc::Key::NP9 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::NorthEast ) << std::endl;
		}
		if ( GetKey( olc::Key::NP6 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::East ) << std::endl;
		}
		if ( GetKey( olc::Key::NP3 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::SouthEast ) << std::endl;
		}
		if ( GetKey( olc::Key::NP2 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::South ) << std::endl;
		}
		if ( GetKey( olc::Key::NP1 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::SouthWest ) << std::endl;
		}
		if ( GetKey( olc::Key::NP4 ).bPressed )
		{
			std::cout << ( int )world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::West ) << std::endl;
		}



		if ( GetMouse( 0 ).bPressed || GetMouse( 0 ).bHeld )
			//if ( GetMouse( 0 ).bPressed )
		{
			world->insert( static_cast< TileIdentity >( tileId ), tileIndex.x, tileIndex.y, 1, 1 );

		}

		if ( GetMouse( 1 ).bPressed || GetMouse( 1 ).bHeld )
			//if ( GetMouse( 1 ).bPressed )
		{
			world->remove( static_cast< TileIdentity >( tileId ), tileIndex.x, tileIndex.y, 1, 1 );
		}

		
		// Update Assets
		this->world->updateDecals();

		// Update Game Logic
		this->world->tick( fElapsedTime );
		this->player->update( fElapsedTime, *this ); // [~!]



		/*
		// Collision Detection [!] Need to put this on another thread with conditional variable
		this->world->getSpatialPartition().updateSpaces( &this->player->getCharacter() );
		//this->world->getSpatialPartition().updateSpaces( this->enemy1 );
		//this->world->getSpatialPartition().updateSpaces( this->enemy2 );
		//this->world->getSpatialPartition().updateSpaces( this->enemy3 );


		this->world->getSpatialPartition().checkCollisions();


		this->player->getCharacter().updateDynamicPhysics( this->world, fElapsedTime );
		//this->enemy1->updateDynamicPhysics( this->world, fElapsedTime );
		//this->enemy2->updateDynamicPhysics( this->world, fElapsedTime );
		//this->enemy3->updateDynamicPhysics( this->world, fElapsedTime );
		*/


		// Camera
		camera->setPosition( this->player->getCharacter().getAABB().getCenter().x - 32/2, this->player->getCharacter().getAABB().getCenter().y - 32/2 ); //[~!]


		// Render Enemies
		//this->camera->renderPlayer( *player ); 
		//this->camera->renderDynamicObject( *enemy1 );
		//this->camera->renderDynamicObject( *enemy2 );
		//this->camera->renderDynamicObject( *enemy3 );



		Clear( olc::BLACK );

		// Draw background
		//DrawDecal( olc::vf2d{ 0, 0 }, this->backgroundDecal, olc::vf2d{1.0, 1.0} );
		//DrawDecal( olc::vf2d{ 0, 0 }, this->backgroundDecal, olc::vf2d{1.0, 1.0}, this->transitionColor( this->world->getSecond() ) );
		


		/*
		float worldSecond = this->world->getSecond();
		olc::Pixel sunAmbienceLighting = this->transitionColor( worldSecond );
		DrawDecal( olc::vf2d{ 0, 0 }, this->backgroundSkyDecal, olc::vf2d{1.0, 1.0}, sunAmbienceLighting );
		DrawDecal( olc::vf2d{ 0, 0 }, this->backgroundNightSkyDecal, olc::vf2d{1.0, 1.0}, olc::Pixel( 255 - sunAmbienceLighting.r, 255 - sunAmbienceLighting.g, 255 - sunAmbienceLighting.b, 255 - sunAmbienceLighting.b ) );
		this->renderSun( this->world->getSecond() );
		DrawDecal( olc::vf2d{ 0, 0 }, this->backgroundTerrainDecal, olc::vf2d{1.0, 1.0}, sunAmbienceLighting );
		*/




		// Draw Cave background
		static const std::int64_t OVERWORLD_HEIGHT = 1536;

		long double playerX = this->player->getCharacter().getCurrPosition().x;
		long double playerY = this->player->getCharacter().getCurrPosition().y;

		long double terraneanHeightPerlinVal = this->world->getTerraneanHeightMap().getPerlinValue( playerX );
		std::int64_t worldYTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT );
		long double attenuate = std::min( ( long double )1, std::max( ( long double )0, ( ( -worldYTerranean + playerY ) / ( -worldYTerranean / 4 ) ) ) );


		DrawDecal( olc::vf2d{ 0, 0 }, this->caveBackgroundDecal, olc::vf2d{1.0, 1.0}, olc::Pixel( 255, 255, 255, attenuate * 255 ) );

		this->camera->renderWorldBackground();
		this->camera->renderPlayer( *player ); //[~!]
		this->world->synchRender(); // renderWorldForeground





		// DEBUG
		drawTileIndexString( tileIndex );
		//this->world->printTime();
		


		if ( GetKey( olc::Key::PERIOD ).bPressed ) world->DEBUG_PRINT_TILE_SPRITES();
		

		return true;
	}


	void renderSun( float second )
	{
		float difference = second - 10800;
		float absoluteDifference = 75600;
		float p = difference / ( absoluteDifference - 1 );


		//float y = 0.000000795f * second * second - 0.0791f * second + 2039.2f;
		//float y = 0.00000069539704f * second * second - 0.0600f * second + 1367.77f;
		//float y = 0.0000007906569121 * second * second - 0.0683127572016466 * second + 1545.5555555555629326;
		float y = 0.0000007430269776 * second * second - 0.0641975308641982 * second + 1456.6666666666742458;

		DrawDecal(
			olc::vf2d{
				( ( 1.0f - p ) * 0.0f + p * 1980.0f + 0.5f ) ,
				y
			},
			this->sunDecal, olc::vf2d{1.0, 1.0}
		);

		return;
	}


	olc::Pixel transitionColor( float second )
	{
		// Ambience

		if ( second >= 21600 && second < 39600 ) // Morning
		{
			float difference = second - 21600;
			float absoluteDifference = 18000;

			std::cout << difference / absoluteDifference << std::endl;

			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 255 + 0.5 ),
				255
			};

			//return olc::Pixel{ 250, 250, 210 };
		}
		else if ( second >= 39600 && second < 61200 ) // Afternoon
		{
			//return olc::Pixel{ 255, 255, 255, 255 };
			float difference = second - 39600;
			float absoluteDifference = 21600;

			std::cout << difference / absoluteDifference << std::endl;


			float p = difference / ( absoluteDifference - 1 );


			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 146 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				255
			};
		}
		else if ( second >= 61200 && second < 75600 ) // Evening
		{
			//return olc::Pixel{ 146, 50, 50, 255 };

			float difference = second - 61200;
			float absoluteDifference = 14400;

			std::cout << difference / absoluteDifference << std::endl;


			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 146 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 9 + 0.5 ),
				255
			};

		}
		else if ( ( second >= 75600 && second < 86400 ) || ( second >= 0 && second < 21600 ) ) // Night
		{
			float unwrappedSecond = ( second >= 0 && second < 21600 ) ? second + 86400 : second;
			float difference = unwrappedSecond - 75600;
			float absoluteDifference = 32400;


			std::cout << difference / absoluteDifference << std::endl;

			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 1 + 0.5 ),
				255 
			};
		}
		else
		{
			return olc::Pixel{ 0, 0, 0, 0 };
		}
	}







	void createWorld()
	{
		std::int64_t seed = 35346236;

		TerraneanHeightMap terraneanHeightMap(
			seed, 501125321,
			12, 1024 * 3
		);


		SubterraneanHeightMap subterraneanHeightMap(
			seed, 7765867141,
			7, 1024 * 3 // 9, 1024*2
		);

		TemperatureMap temperatureMap(
			seed, 246235489, 1136930381,
			//3, 1024 * 3, 1536
			8, 1024 * 3, 1536
		);


		PrecipitationMap precipitationMap(
			seed, 7765867141, 4334744837,
			//6, 1024 * 3, 1536x
			11, 1024 * 3, 1536
		);


		BiomeSubstanceMap biomeSubstanceMap(
			seed, 111647323, 945784661,
			6, 64, 64
		);

		CaveMap upperCaveMap(
			seed, 713537467, 829439293,
			9, 512, 1536
		);


		CaveMap lowerCaveMap(
			seed, 4524575723, 457245726019,
			9, 512, 1536
		);


		
		// Biome construction
		FoliageMap borealForestFoliageMap(
			seed, 8573475611,
			12, 1024 * 3
		);
		std::pair<long double, long double> borealForestTemperatureRange{ 0.0701, 0.4194 }; // range based on biome graph line intersections
		std::pair<long double, long double> borealForestPrecipitationRange{ 0.0701, 1.0 };
		BorealForest borealForest( borealForestFoliageMap, borealForestTemperatureRange, borealForestPrecipitationRange );

		FoliageMap subtropicalDesertFoliageMap(
			seed, 327249029,
			12, 1024 * 3
		);
		std::pair<long double, long double> subtropicalDesertTemperatureRange{ 0.5216, 1.0 };
		std::pair<long double, long double> subtropicalDesertPrecipitationRange{ 0.0, 0.188 };
		SubtropicalDesert subtropicalDesert( subtropicalDesertFoliageMap, subtropicalDesertTemperatureRange, subtropicalDesertPrecipitationRange );

		FoliageMap temperateGrasslandFoliageMap(
			seed, 677136037,
			12, 1024 * 3
		);
		std::pair<long double, long double> temperateGrasslandTemperatureRange{ 0.044, 0.574 };
		std::pair<long double, long double> temperateGrasslandPrecipitationRange{ 0.0, 0.1612 };
		TemperateGrassland temperateGrassland( temperateGrasslandFoliageMap, subtropicalDesertTemperatureRange, subtropicalDesertPrecipitationRange );

		FoliageMap temperateRainforestFoliageMap(
			seed, 1645273573,
			12, 1024 * 3
		);
		std::pair<long double, long double> temperateRainforestTemperatureRange{ 0.3634, 0.8495 };
		std::pair<long double, long double> temperateRainforestPrecipitationRange{ 0.7135, 1.0 };
		TemperateRainforest temperateRainforest( temperateRainforestFoliageMap, temperateRainforestTemperatureRange, temperateRainforestPrecipitationRange );

		FoliageMap temperateSeasonalForestFoliageMap(
			seed, 4583686091,
			5, 1024 * 3
		);
		std::pair<long double, long double> temperateSeasonalForestTemperatureRange{ 0.2199, 0.7556 };
		std::pair<long double, long double> temperateSeasonalForestPrecipitationRange{ 0.2917, 0.801 };
		TemperateSeasonalForest temperateSeasonalForest( temperateSeasonalForestFoliageMap, temperateSeasonalForestTemperatureRange, temperateSeasonalForestPrecipitationRange );

		FoliageMap tropicalRainforestFoliageMap(
			seed, 64583266801,
			12, 1024 * 3
		);
		std::pair<long double, long double> tropicalRainforestTemperatureRange{ 0.7556, 1.0 };
		std::pair<long double, long double> tropicalRainforestPrecipitationRange{ 0.659, 1.0 };
		TropicalRainforest tropicalRainforest( tropicalRainforestFoliageMap, tropicalRainforestTemperatureRange, tropicalRainforestPrecipitationRange );

		FoliageMap tropicalSeasonalForestFoliageMap(
			seed, 28457237801,
			12, 1024 * 3
		);
		std::pair<long double, long double> tropicalSeasonalForestTemperatureRange{ 0.5745, 1.0 };
		std::pair<long double, long double> tropicalSeasonalForestPrecipitationRange{ 0.1612, 0.7135 };
		TropicalSeasonalForest tropicalSeasonalForest( tropicalSeasonalForestFoliageMap, tropicalSeasonalForestTemperatureRange, tropicalSeasonalForestPrecipitationRange );

		FoliageMap tundraFoliageMap(
			seed, 9693061229,
			12, 1024 * 3
		);
		std::pair<long double, long double>  tundraTemperatureRange{ 0.0, 0.138 };
		std::pair<long double, long double>  tundraPrecipitationRange{ 0.0, 1.0 };
		Tundra tundra( tundraFoliageMap, tundraTemperatureRange, tundraPrecipitationRange );

		FoliageMap woodlandFoliageMap(
			seed, 7452826273,
			12, 1024 * 3
		);
		std::pair<long double, long double> woodlandTemperatureRange{ 0.0561, 0.6299 };
		std::pair<long double, long double> woodlandPrecipitationRange{ 0.1285, 0.2776 };
		Woodland woodland( woodlandFoliageMap, woodlandTemperatureRange, woodlandPrecipitationRange );


		this->world = new World(
			seed,
			terraneanHeightMap,
			subterraneanHeightMap,
			temperatureMap,
			precipitationMap,
			biomeSubstanceMap,
			upperCaveMap,
			lowerCaveMap,
			borealForest,
			subtropicalDesert,
			temperateGrassland,
			temperateRainforest,
			temperateSeasonalForest,
			tropicalRainforest,
			tropicalSeasonalForest,
			tundra,
			woodland,
			21600.0f, /*75600.0f,*/ 0, 0,
			this->daySprite, this->dayDecal, this->nightSprite, this->nightDecal, this->sunSprite, this->sunDecal, this->landscapeSprite, this->landscapeDecal
		);

		/*
		this->camera = new Camera(
			BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::FOCAL_POINT_CELL_WIDTH, Settings::Camera::FOCAL_POINT_CELL_HEIGHT ),
			BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::VIEW_CELL_WIDTH, Settings::Camera::VIEW_CELL_HEIGHT ),
			1.0f, 1.0f,
			this->world);
			*/
		this->camera = new Camera(
			BoundingBox<long double>( Settings::Player::Character::DEFAULT_CENTER_X, Settings::Player::Character::DEFAULT_CENTER_Y, Settings::Camera::FOCAL_POINT_CELL_WIDTH, Settings::Camera::FOCAL_POINT_CELL_HEIGHT ),
			BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::VIEW_CELL_WIDTH, Settings::Camera::VIEW_CELL_HEIGHT ),
			1.0f, 1.0f,
			this->world );

		// Initialize world [!]
		this->world->initializeCamera( this->camera );
		this->world->initializeDatabase();
		this->world->initializeSprites();
		this->world->initializeDelimits( this->camera->getFocalPoint() );
		this->world->initializeWorldChunks();
		this->world->startWorldMemorySystem();

		return;
	}

	void createPlayer()
	{
		this->player = new Player(
			olc::v2d_generic<long double>{ Settings::Player::Character::DEFAULT_CENTER_X, Settings::Player::Character::DEFAULT_CENTER_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_HALF_SIZE_X, Settings::Player::Character::DEFAULT_HALF_SIZE_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_SCALE_X, Settings::Player::Character::DEFAULT_SCALE_Y },
			CharacterState::Stand,
			Settings::Player::Character::DEFAULT_RUN_SPEED,
			Settings::Player::Character::DEFAULT_JUMP_SPEED,
			this->world, 0.0,
			this->alphaSprite, this->alphaDecal
		);

		this->world->setPlayer( this->player );
		return;
	}



	void createEnemy()
	{
		this->enemy1 = new Zombie(
			olc::v2d_generic<long double>{ Settings::Player::Character::DEFAULT_CENTER_X, Settings::Player::Character::DEFAULT_CENTER_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_HALF_SIZE_X*1.5, Settings::Player::Character::DEFAULT_HALF_SIZE_Y*0.5 },
			olc::vf2d{ Settings::Player::Character::DEFAULT_SCALE_X, Settings::Player::Character::DEFAULT_SCALE_Y },
			ZombieState::Stand,
			8.0f,
			15.0f,
			this->world, 0.0
		);

		this->enemy2 = new Zombie(
			olc::v2d_generic<long double>{ Settings::Player::Character::DEFAULT_CENTER_X - 1, Settings::Player::Character::DEFAULT_CENTER_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_HALF_SIZE_X*0.5, Settings::Player::Character::DEFAULT_HALF_SIZE_Y*0.75 },
			olc::vf2d{ Settings::Player::Character::DEFAULT_SCALE_X, Settings::Player::Character::DEFAULT_SCALE_Y },
			ZombieState::Stand,
			9.0f,
			17.5f,
			this->world, 0.0
		);

		this->enemy3 = new Zombie(
			olc::v2d_generic<long double>{ Settings::Player::Character::DEFAULT_CENTER_X + 5, Settings::Player::Character::DEFAULT_CENTER_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_HALF_SIZE_X, Settings::Player::Character::DEFAULT_HALF_SIZE_Y },
			olc::vf2d{ Settings::Player::Character::DEFAULT_SCALE_X, Settings::Player::Character::DEFAULT_SCALE_Y },
			ZombieState::Stand,
			7.5f,
			20.0f,
			this->world, 0.0
		);

		return;
	}

	void loadAssets()
	{
		Assets::get();
		return;
	}
	









	void renderPlayer()
	{
		const Character& character = this->player->getCharacter();
		const AABB& aabb = character.getAABB();
		const olc::vf2d& aabbOffset = character.getAABBOffset();
		const olc::vf2d& characterScale = character.getScale();
		//.getAABB().getCenter();
		
		return;
	}












	void drawTileIndexString( const olc::vi2d& tileIndex )
	{
		DrawStringDecal(
			olc::vi2d( GetMouseX(), GetMouseY() ),
			"[" + std::to_string( tileIndex.x ) + "," + std::to_string( -tileIndex.y ) + "]",
			olc::WHITE,
			olc::vf2d( 2.0f, 2.0f )
		);
	}
};


int main()
{
	//ShowCursor( NULL );
	Example demo;
	/*
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize ) )
		demo.Start();
	*/
	/*
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize, true, true, true ) )
		demo.Start();
		*/
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize, false, false, true ) )
		demo.Start();
	return 0;
}