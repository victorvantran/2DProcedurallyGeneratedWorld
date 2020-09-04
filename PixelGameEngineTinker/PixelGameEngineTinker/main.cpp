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
	olc::vf2d camera = olc::vf2d{ 1,1 };

	//olc::ResourcePack _rpPlayer;
	olc::Sprite *_spriteLoading;
	olc::Layer<Tile> _layerLoading;
	olc::Atlas _atlasLoading;
	///


public:
	Game()
	{
		this->initializeGame();
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

		delete this->_pScreen;
		// this->_rpPlayer.ClearPack();
		return;
	}


	void initializeAtlases()
	{
		// Information needed: png, resolution of png, and resolution of the tile
		// For each tile [0,0], [1,0], [2,0], ..., [n,m], mark the location of the subarea (via bounding box) of the given png based on the resolution of the tile
		this->_spriteLoading = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\worldmap480x270_8x8.png" );
		this->_atlasLoading.create( this->_spriteLoading );

		for ( int row = 0; row < 270; row++ )
		{
			for ( int column = 0; column < 480; column++ )
			{
				this->_atlasLoading.mapping.emplace_back((int)column * 8, (int)row * 8, 8, 8);
			}

		}

		return;
	}


	void destroyAtlases()
	{
		/// Free memory of what was used in initializeAtlases()
		return;
	}


	void initializeLayers()
	{
		this->_layerLoading.create( olc::vi2d{ 480, 270 }, olc::vi2d{ 8, 8 } ); 
		return;
	}

	void destroyLayers()
	{
		/// free memory of what was used in initializeLayers()
		return;
	}


	GameState getGameState()
	{
		return this->_gameState;
	}


	void runGameStateLoading( float fElapsedTime )
	{
		Clear( olc::WHITE );

		//SetPixelMode( olc::Pixel::ALPHA );
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, camera, olc::vi2d{ 128, 72 }, 1 ); // [col, row]
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