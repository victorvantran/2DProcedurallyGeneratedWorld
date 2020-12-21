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
	// [!] need to chagne absolute position and zoom of the camera in accordance to pixelSize
	const static int pixelSize = Settings::Screen::PIXEL_SIZE;
	const static int screenWidth = Settings::Screen::SCREEN_PIXEL_WIDTH;
	const static int screenHeight = Settings::Screen::SCREEN_PIXEL_HEIGHT;
	const static int cellSize = Settings::Screen::CELL_PIXEL_SIZE;


public:
	World* world = nullptr;
	Camera* camera = nullptr;

	olc::vi2d decalGridDimension;

	Tile* tiles; // exist, id, configuration
	int* configurations;

	olc::vi2d gridDimension;

	int tileId = 1; // [!] never insert a void block (0) unless you know how to remove it without seeing it
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
		float mouseX = ( float )GetMouseX();
		float mouseY = ( float )GetMouseY();

		float panSpeed = 20.0f;
		if ( GetKey( olc::Key::UP ).bPressed || GetKey( olc::Key::UP ).bHeld )
		{
			camera->panY( -panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::DOWN ).bPressed || GetKey( olc::Key::DOWN ).bHeld )
		{
			camera->panY( panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::LEFT ).bPressed || GetKey( olc::Key::LEFT ).bHeld )
		{
			camera->panX( -panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::RIGHT ).bPressed || GetKey( olc::Key::RIGHT ).bHeld )
		{
			camera->panX( panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::T ).bPressed || GetKey( olc::Key::T ).bHeld )
		{
			camera->pan( panSpeed * fElapsedTime, panSpeed * fElapsedTime );
		}


		if ( GetKey( olc::Key::F1 ).bPressed )
		{
			camera->panY( -panSpeed * 100 );
		}
		if ( GetKey( olc::Key::F2 ).bPressed )
		{
			camera->panY( panSpeed * 100 );
		}
		if ( GetKey( olc::Key::F3 ).bPressed )
		{
			camera->panX( -panSpeed * 100 );
		}
		if ( GetKey( olc::Key::F4 ).bPressed )
		{
			camera->panX( panSpeed * 100 );
		}

		// Zoom in
		if ( GetKey( olc::Key::Z ).bPressed || GetKey( olc::Key::Z ).bHeld )
		{
			float zoomScale = 1.0f + ( 1.0f * fElapsedTime );
			this->camera->zoom( zoomScale );
		}

		// Zoom out
		if ( GetKey( olc::Key::X ).bPressed || GetKey( olc::Key::X ).bHeld )
		{
			float zoomScale = 1.0f - ( 1.0f * fElapsedTime );
			this->camera->zoom( zoomScale );
		}

		// Reset camera
		if ( GetKey( olc::Key::C ).bPressed || GetKey( olc::Key::C ).bHeld )
		{
			this->camera->setZoom( 1.0f );
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
		float tilePositionX;
		float tilePositionY;

		this->camera->screenToWorld( GetMouseX(), GetMouseY(), tilePositionX, tilePositionY );

		olc::vi2d tileIndex = olc::vi2d{
			( int )( std::floorf( tilePositionX ) ),
			( int )( std::floorf( tilePositionY ) )
		};

		if ( GetKey( olc::Key::P ).bPressed )
		{
			world->insert( tileIndex.x, tileIndex.y, 1, 1, tileId );
		}

		if ( GetKey( olc::Key::K ).bPressed || GetKey( olc::Key::K ).bHeld )
		{
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



		// Render
		Clear( olc::BLACK );
		this->camera->renderWorld();
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
			BoundingBox<float>( 0.0f, 0.0f, Settings::Camera::FOCAL_POINT_CELL_WIDTH, Settings::Camera::FOCAL_POINT_CELL_HEIGHT ),
			BoundingBox<float>( 0.0f, 0.0f, Settings::Camera::VIEW_CELL_WIDTH, Settings::Camera::VIEW_CELL_HEIGHT ),
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