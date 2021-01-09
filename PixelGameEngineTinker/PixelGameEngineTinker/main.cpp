#define OLC_PGE_APPLICATION



#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "Assets.h"
#include "World.h"
#include "Tile.h"
#include "TileRender.h"
#include "Camera.h"

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
	Camera* camera = nullptr;

	olc::vi2d decalGridDimension;

	Tile* tiles; // exist, id, configuration
	int* configurations;

	olc::vi2d gridDimension;

	int tileId = 1; // never insert a void block (0) unless you know how to remove it without seeing it
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
	}

public:
	bool OnUserCreate() override
	{
		loadAssets();
		createWorld();
		return true;
	}


	bool OnUserUpdate( float fElapsedTime ) override
	{


		// Camera Debug
		long double mouseX = ( long double )GetMouseX();
		long double mouseY = ( long double )GetMouseY();

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

		if ( GetKey( olc::Key::W ).bPressed )
		{
			tileId = 1;
		}
		else if ( GetKey( olc::Key::S ).bPressed )
		{
			tileId = 2;
		}
		else if ( GetKey( olc::Key::D ).bPressed )
		{
			tileId = 3;
		}

		// Insert to world Debug 
		long double tilePositionX;
		long double tilePositionY;

		this->camera->screenToWorld( GetMouseX(), GetMouseY(), tilePositionX, tilePositionY );

		olc::v2d_generic<std::int64_t> tileIndex = olc::v2d_generic<std::int64_t>{
			( std::int64_t )( std::floor( tilePositionX ) ),
			( std::int64_t )( std::floor( tilePositionY ) )
		};



		if ( GetKey( olc::Key::P ).bPressed )
		{
			world->insert( tileIndex.x, tileIndex.y, 1, 1, tileId );
		}





		if ( GetKey( olc::Key::R ).bPressed || GetKey( olc::Key::R ).bHeld )
		{
			std::uint8_t r = std::rand() % 256;
			std::uint8_t g = std::rand() % 256;
			std::uint8_t b = std::rand() % 256;

			//world->insertLightTile( tileIndex.x, tileIndex.y, r, g, b, 255, 20 );
		}


		if ( GetMouse( 0 ).bPressed || GetMouse( 0 ).bHeld )
			//if ( GetMouse( 0 ).bPressed )
		{
			world->insert( tileIndex.x, tileIndex.y, 5, 5, tileId );
		}

		if ( GetMouse( 1 ).bPressed || GetMouse( 1 ).bHeld )
			//if ( GetMouse( 1 ).bPressed )
		{
			world->remove( tileIndex.x, tileIndex.y, 5, 5, tileId );
		}

		// updateLighting
		this->world->resetLighting();
		this->world->activateCursorLightSource( tilePositionX, tilePositionY, 20 );
		this->world->updateLighting();


		// Render
		Clear( olc::BLACK );
		this->camera->renderWorld();
		// this->world->renderLighting();
		this->world->updateDecals();
	

		// DEBUG
		drawTileIndexString( tileIndex );

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
		this->world->initializeDelimits( this->camera->getFocalPoint() );
		this->world->initializeWorldChunks();
		this->world->startWorldMemorySystem();
	}


	void loadAssets()
	{
		Assets::get();
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
	Example demo;
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize ) )
		demo.Start();
	return 0;
}