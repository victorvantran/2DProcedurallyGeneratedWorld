#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "settings.h"
#include "GameState.h"
#include "Screen.h"
#include "Layer.h"
#include "Tile.h"
#include "World.h"


/// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
private:
	GameState _gameState;
	Screen* _pScreen;



	//olc::ResourcePack _rpPlayer;
	World* _world;
	Layer<Tile>* _layerRandom;


	/// Temporary Datatype for sprites for testing purposes
	olc::Sprite* _spritePlayerMouse;
	olc::Sprite* _spriteLoading;

	olc::Sprite* _spriteTileSetTinkerWorld;


	/// Temporary Datatype for decals for testing purposes
	olc::Decal* _decalPlayerMouse;
	olc::Decal* _decalLoading;

	olc::Decal* _decalTileSetTinkerWorld;


	/// Temporary Player DataType
	enum class KeyInput
	{
		UpKey = 0,
		DownKey = 1,
		LeftKey = 2,
		RightKey = 3,
		JumpKey = 4,
		count
	};

	bool* _playerInputs;
	olc::vf2d _playerMouse;
	olc::vf2d _playerCamera;


	Layer<Tile>* _layerLoading;
	Atlas* _atlasLoading;

	Atlas* _atlasTinkerWorld;

	///


public:
	Game()
	{
	}

	~Game()
	{
		this->destroyGame();
	}



public:
	void initializeGame()
	{
		/// initialize the screen
		/// run through the first state of the game
		sAppName = "Example";

		this->initializeSprites();
		this->initializeDecals();
		this->initializeAtlases();
		this->initializeLayers();
		this->initializePlayer();
		this->initializeWorld();

		this->_gameState = GameState::TINKER_WORLD_LOADING;
		this->_pScreen = new Screen();

		// this->_rpPlayer.LoadPack(...);
		return;
	}

	void destroyGame()
	{
		/// Free memory of what was used in Game::initalizeGame()
		this->destroyAtlases();
		this->destroyLayers();
		this->destroySprites();
		this->destroyDecals();
		this->destroyPlayer();
		this->destroyWorld();

		delete this->_pScreen;
		// this->_rpPlayer.ClearPack();
		return;
	}

	void initializeSprites()
	{
	/// Initialize the datatype that holds all the sprites
		this->_spritePlayerMouse = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\dwarven_gauntlet_cursor.png" );
		this->_spriteLoading = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\worldmapgrid_480x270_8x8.png" );
		this->_spriteTileSetTinkerWorld = new olc::Sprite("C:\\Users\\Victor\\Desktop\\Tinker\\platformer_25x16_8x8.png");

		return;
	}

	void destroySprites()
	{
	/// Free memory occupied by the datatype that stores the sprites
		return;
	}

	void initializeDecals()
	{
	/// Initialize the datatype that holds all the decals
		this->_decalPlayerMouse = new olc::Decal( this->_spritePlayerMouse );
		this->_decalLoading = new olc::Decal( this->_spriteLoading );
		this->_decalTileSetTinkerWorld = new olc::Decal( this->_spriteTileSetTinkerWorld );

		return;
	}

	void destroyDecals()
	{
	/// Free memory occupied by the datatype that stores the decals
		return;
	}


	void initializeAtlases()
	{
	// Information needed: png, resolution of png, and resolution of the tile
	// For each tile [0,0], [1,0], [2,0], ..., [n,m], mark the location of the subarea (via bounding box) of the given png based on the resolution of the tile
		this->_atlasLoading = new Atlas( this->_spriteLoading, this->_decalLoading, olc::vi2d(480, 270), olc::vi2d(8, 8) );
		this->_atlasTinkerWorld = new Atlas( this->_spriteTileSetTinkerWorld, this->_decalTileSetTinkerWorld, olc::vi2d( 25, 16 ), olc::vi2d( 8, 8 ) );
	}


	void destroyAtlases()
	{
	/// Free memory of what was used in initializeAtlases()
		delete this->_atlasLoading;
		delete this->_atlasTinkerWorld;
		return;
	}


	void initializeLayers()
	{
	/// Create the matrix of cells based on the screen and tile resolution
		this->_layerLoading = new Layer<Tile>();
		this->_layerLoading->create( olc::vi2d{ 480, 270 } ); 
		return;
	}

	void destroyLayers()
	{
	/// Free memory of what was used in initializeLayers()
		delete this->_layerLoading;
		return;
	}


	void initializePlayer()
	{
		this->_playerInputs = new bool[(int)KeyInput::count];
		this->_playerMouse = olc::vf2d{ 0.0f, 0.0f };
		this->_playerCamera = olc::vf2d{ 0.0f, 0.0f };
	}


	void destroyPlayer()
	{
		delete[] this->_playerInputs;
	}

	void initializeWorld()
	{
	///
		this->_world = new World();
	}


	void destroyWorld()
	{
	///
		delete this->_world;
	}



	GameState getGameState()
	{
	/// Returns the current gamestate
		return this->_gameState;
	}


	void runGameStateLoading( float fElapsedTime )
	{
		return;
	}


	void runGameStateTitle( float fElapsedTime )
	{
		///
		return;
	}


	void runGameStateTinker( float fElapsedTime )
	{
	///

		updatePlayer();

		Clear( olc::DARK_BLUE );

		//SetPixelMode( olc::Pixel::ALPHA );
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f ); // [col, row] 128,72
		//SetPixelMode( olc::Pixel::NORMAL );

		this->drawPlayerMouse();
		return;
	}



	void runGameStateTinkerWorldLoading( float fElapsedTime )
	{
	///
		this->_world->generateTestLayer( olc::vi2d{1000, 1000}, this->_atlasTinkerWorld );
		this->_gameState = GameState::TINKER_WORLD;
		return;
	}


	void runGameStateTinkerWorld( float fElapsedTime )
	{
		///

		updatePlayer();



		Clear( olc::DARK_CYAN );


		//this->_pScreen->drawLayer( this->_layerRandom, this->_atlasLoading, this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f ); // [col, row] 128,72
		this->_pScreen->drawLayer( std::get<0>(this->_world->getWorldChunks()[0]), std::get<1>(this->_world->getWorldChunks()[0]), this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f );

		this->drawPlayerMouse();
		return;
	}


public:
	void updatePlayer()
	{
		this->updatePlayerMouse();
		this->resetPlayerInputs();
		this->updatePlayerInputs();
		this->updatePlayerCamera();

		return;
	}

	void updatePlayerMouse()
	{
		this->_playerMouse = olc::vf2d{ (float)this->GetMouseX(), (float)this->GetMouseY() };

		return;
	}


	void updatePlayerInputs()
	{
	/// Updates the array of bool values that represent a key being pressed
		if ( this->GetKey( olc::Key::UP ).bPressed || this->GetKey( olc::Key::UP ).bHeld )
		{
			this->_playerInputs[( int )KeyInput::UpKey] = true;
		}
		if ( this->GetKey( olc::Key::DOWN ).bPressed || this->GetKey( olc::Key::DOWN ).bHeld )
		{
			this->_playerInputs[( int )KeyInput::DownKey] = true;
		}
		if ( this->GetKey( olc::Key::LEFT ).bPressed || this->GetKey( olc::Key::LEFT ).bHeld )
		{
			this->_playerInputs[( int )KeyInput::LeftKey] = true;
		}
		if ( this->GetKey( olc::Key::RIGHT ).bPressed || this->GetKey( olc::Key::RIGHT ).bHeld )
		{
			this->_playerInputs[( int )KeyInput::RightKey] = true;
		}

		return;
	}

	void resetPlayerInputs()
	{
	/// Resets the array of bool values that represent a key being pressed
		for ( int i = 0; i < ( int )KeyInput::count; i++ )
		{
			this->_playerInputs[i] = false;
		}

		return;
	}


	void updatePlayerCamera()
	{
		float speed = 1.0f;
		if ( this->IsFocused() )
		{
			if ( this->GetKey( olc::Key::UP ).bPressed )
			{
				this->_playerCamera.y += -speed;
			}

			if ( this->GetKey( olc::Key::DOWN ).bPressed )
			{
				this->_playerCamera.y += speed;
			}


			if ( this->GetKey( olc::Key::LEFT ).bPressed )
			{
				this->_playerCamera.x += -speed;
			}


			if ( this->GetKey( olc::Key::RIGHT ).bPressed )
			{
				this->_playerCamera.x += speed;
			}
		}
	}


	void drawPlayerMouse()
	{
		this->DrawDecal( this->_playerMouse, this->_decalPlayerMouse );
		return;
	}


public:
	bool OnUserCreate() override
	{
		/// Called once at the start, so create things here
		this->initializeGame();
		return true;
	}

	bool OnUserUpdate( float fElapsedTime ) override
	{
		switch ( this->getGameState() )
		{
		case GameState::LOADING: this->runGameStateLoading( fElapsedTime ); break;
		case GameState::TITLE: this->runGameStateTitle( fElapsedTime ); break;
		case GameState::TINKER: this->runGameStateTinker( fElapsedTime ); break;
		case GameState::TINKER_WORLD_LOADING: this->runGameStateTinkerWorldLoading( fElapsedTime ); break;
		case GameState::TINKER_WORLD: this->runGameStateTinkerWorld( fElapsedTime ); break;
		}

		return true;
	}
};

int main()
{
	Game demo;
	{
		if ( demo.Construct( SETTINGS::RESOLUTION::SCREEN_X, SETTINGS::RESOLUTION::SCREEN_Y, SETTINGS::RESOLUTION::PIXEL_SCALE_X, SETTINGS::RESOLUTION::PIXEL_SCALE_Y ) )
		{
			demo.Start();
		}
		return 0;
	}
}