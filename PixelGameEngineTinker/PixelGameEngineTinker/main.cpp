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


	///
	olc::vf2d camera = olc::vf2d{ 10,10 };

	//olc::ResourcePack _rpPlayer;



	/// Datatype for sprites
	olc::Sprite* _spriteLoading;

	/// Datatype for decals
	olc::Decal* _decalLoading;



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

		this->_gameState = GameState::LOADING;
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

		delete this->_pScreen;
		// this->_rpPlayer.ClearPack();
		return;
	}

	void initializeSprites()
	{
	/// Initialize the datatype that holds all the sprites
		this->_spriteLoading = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\worldmap480x270_8x8.png" );
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


	GameState getGameState()
	{
		return this->_gameState;
	}




	void runGameStateLoading( float fElapsedTime )
	{
		Clear( olc::DARK_BLUE );

		//SetPixelMode( olc::Pixel::ALPHA );
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, camera, olc::vi2d{ 128, 72 }, 1.0f ); // [col, row]
		camera = camera * 1.001;
		//SetPixelMode( olc::Pixel::NORMAL );

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