#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "settings.h"
#include "GameState.h"
#include "Screen.h"
#include "Layer.h"
#include "Tile.h"



/// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
private:
	GameState _gameState;
	olc::Screen* _pScreen;



	//olc::ResourcePack _rpPlayer;



	/// Temporary Datatype for sprites for testing purposes
	olc::Sprite* _spritePlayerMouse;
	olc::Sprite* _spriteLoading;


	/// Temporary Datatype for decals for testing purposes
	olc::Decal* _decalPlayerMouse;
	olc::Decal* _decalLoading;


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


	olc::Layer<Tile> _layerLoading;
	olc::Atlas* _atlasLoading;

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

		this->_gameState = GameState::TINKER;
		this->_pScreen = new olc::Screen();

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

		delete this->_pScreen;
		// this->_rpPlayer.ClearPack();
		return;
	}

	void initializeSprites()
	{
	/// Initialize the datatype that holds all the sprites
		this->_spritePlayerMouse = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\dwarven_gauntlet.png" );
		this->_spriteLoading = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\worldmapgrid480x270_8x8.png" );
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
		this->_atlasLoading = new olc::Atlas();
		this->_atlasLoading->create( this->_spriteLoading, this->_decalLoading );

		for ( int row = 0; row < 270; row++ )
		{
			for ( int column = 0; column < 480; column++ )
			{
				this->_atlasLoading->mapping.emplace_back((int)column * 8, (int)row * 8, 8, 8);
			}

		}

		return;
	}


	void destroyAtlases()
	{
	/// Free memory of what was used in initializeAtlases()
		delete this->_atlasLoading;
		return;
	}


	void initializeLayers()
	{
	/// Create the matrix of cells based on the screen and tile resolution
		this->_layerLoading.create( olc::vi2d{ 480, 270 }, olc::vi2d{ 8, 8 } ); 
		return;
	}

	void destroyLayers()
	{
	/// Free memory of what was used in initializeLayers()
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
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, this->_playerCamera, olc::vi2d{ 128, 72 }, 1.0f ); // [col, row]
		//SetPixelMode( olc::Pixel::NORMAL );

		if ( this->_playerCamera.x < 1 )
		{
			this->_playerCamera = olc::vf2d{ 50.0f, 50.0f };
		}
		//this->_playerCamera = this->_playerCamera * 1.0001;

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
		float speed = 0.1f;
		if ( this->IsFocused() )
		{
			if ( this->GetKey( olc::Key::UP ).bHeld )
			{
				this->_playerCamera.y += -speed;
			}

			if ( this->GetKey( olc::Key::DOWN ).bHeld )
			{
				this->_playerCamera.y += speed;
			}


			if ( this->GetKey( olc::Key::LEFT ).bHeld )
			{
				this->_playerCamera.x += -speed;
			}


			if ( this->GetKey( olc::Key::RIGHT ).bHeld )
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