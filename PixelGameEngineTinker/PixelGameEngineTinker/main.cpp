#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "Assets.h"
#include "World.h"
#include "Tile.h"
#include "TileRender.h"
#include "Camera.h"
#include "Player.h"

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
	Camera* camera = nullptr;

	olc::vi2d decalGridDimension;

	Tile* tiles; // exist, id, configuration
	int* configurations;

	olc::vi2d gridDimension;


	TileIdentity tileId = TileIdentity::MossDirt;
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
	}

public:
	bool OnUserCreate() override
	{
		loadAssets();
		createWorld();
		createPlayer();
		return true;
	}


	bool OnUserUpdate( float fElapsedTime ) override
	{
		// 
		//player->resetInputs();

		// Update player inputs



		// Camera Debug
		long double mouseX = ( long double )GetMouseX();
		long double mouseY = ( long double )GetMouseY();



		//player->updateInputs( *this );



		//long double panSpeed = 20.0f;
		long double panSpeed = 32.0f;


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
			tileId = TileIdentity::Alfisol;
		}
		else if ( GetKey( olc::Key::K2 ).bPressed )
		{
			tileId = TileIdentity::YellowClay;
		}
		else if ( GetKey( olc::Key::K3 ).bPressed )
		{
			tileId = TileIdentity::RedClay;
		}
		else if ( GetKey( olc::Key::K4 ).bPressed )
		{
			tileId = TileIdentity::Cambisol;
		}
		else if ( GetKey( olc::Key::K5 ).bPressed )
		{
			tileId = TileIdentity::Silt;
		}
		else if ( GetKey( olc::Key::K6 ).bPressed )
		{
			tileId = TileIdentity::IronOre;
		}
		else if ( GetKey( olc::Key::K7 ).bPressed )
		{
			tileId = TileIdentity::AluminiumOre;
		}
		else if ( GetKey( olc::Key::K8 ).bPressed )
		{
			tileId = TileIdentity::Entisol;
		}
		else if ( GetKey( olc::Key::K9 ).bPressed )
		{
			tileId = TileIdentity::Saltstone;
		}
		else if ( GetKey( olc::Key::K0 ).bPressed )
		{
			tileId = TileIdentity::Quartz;
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


		if ( GetKey( olc::Key::O ).bPressed )
		{
			world->remove( static_cast< TileIdentity >( tileId ), tileIndex.x, tileIndex.y, 5, 5 );
		}

		if ( GetKey( olc::Key::L ).bPressed )
		{

			//const Tile* tile = this->world->getTile( tileIndex.x, tileIndex.y );
			//std::cout << (int)tile->getType() << std::endl;
			const Tile* tile = this->world->getTile( tilePositionX, tilePositionY );
			std::cout << (int)tile->getType() << std::endl;

			//std::cout << tileIndex.x << ", " << tileIndex.y << std::endl;
			//world->insertTorch( tileIndex.x, tileIndex.y, tileId );
			// insert -> insertTile
			//		  -> insertTorch
			//		  -> insert...

			// emit static light... iterate through all worldChunks for all lightSources. See if lightsource radial distance are within camera view. emit
		}


		if ( GetKey( olc::Key::R ).bPressed || GetKey( olc::Key::R ).bHeld )
		{
			std::uint8_t r = std::rand() % 256;
			std::uint8_t g = std::rand() % 256;
			std::uint8_t b = std::rand() % 256;

		}


		if ( GetKey( olc::Key::B ).bPressed || GetKey( olc::Key::B ).bHeld )
		{
			std::cout << (int)world->getTile( tileIndex.x, tileIndex.y )->getBorder( TileBorder::North ) << std::endl;
			//world->insertLightTile( tileIndex.x, tileIndex.y, r, g, b, 255, 20 );
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

		// updateLighting
		//this->world->updateLighting( tilePositionX, tilePositionY );




		// Update
		this->player->update( fElapsedTime, *this );

		if ( GetKey( olc::Key::Q ).bPressed || GetKey( olc::Key::Q ).bHeld )
		{
			camera->setPosition( this->player->getCharacter().getAABB().getCenter().x, this->player->getCharacter().getAABB().getCenter().y );
		}
		// Render
		this->camera->renderPlayer( *player );
		// this->camera->renderWorld( *world );
		this->world->render();

		/*
		Clear( olc::BLACK );
		this->camera->renderWorld();
		this->world->updateDecals();
		*/

		// DEBUG
		drawTileIndexString( tileIndex );

		/*
		DrawStringDecal(
			olc::vi2d( GetMouseX(), GetMouseY() ),
			"O",
			olc::WHITE,
			olc::vf2d( 2.0f, 2.0f )
		);
		*/

		if ( GetKey( olc::Key::M ).bPressed ) world->DEBUG_PRINT_TILE_SPRITES();
		
		return true;
	}


	void createWorld()
	{
		
		this->world = new World();
		this->camera = new Camera(
			BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::FOCAL_POINT_CELL_WIDTH, Settings::Camera::FOCAL_POINT_CELL_HEIGHT ),
			BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::VIEW_CELL_WIDTH, Settings::Camera::VIEW_CELL_HEIGHT ),
			1.0f, 1.0f,
			this->world);

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
			this->world
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
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize ) )
		demo.Start();
	return 0;
}