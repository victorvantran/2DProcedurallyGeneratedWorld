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
	olc::ResourcePack _rpPlayer;
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
		/// free memory of what was used in Game::initalizeGame()
		this->destroyAtlases();
		this->destroyLayers();

		delete this->_pScreen;
		// this->_rpPlayer.ClearPack();
		return;
	}


	void initializeAtlases()
	{
		this->_spriteLoading = new olc::Sprite();
		//this->_atlasLoading.create(new olc::Sprite("C:\\Users\\Victor\\Desktop\\Tinker\\test64x64.png", &this->_rpPlayer) );
		this->_atlasLoading.create( new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\test64x64.png") );

		for ( int i = 0; i < 64; i++ )
		{
			for ( int j = 0; j < 64; j++ )
			{
				this->_atlasLoading.mapping.emplace_back(j * 64, i * 64, 64, 64);
			}

		}


		return;
	}


	void destroyAtlases()
	{
		/// free memory of what was used in initializeAtlases()
		return;
	}


	void initializeLayers()
	{
		//this->_layerLoading.create( SETTINGS::RESOLUTION::SCREEN_X, SETTINGS::RESOLUTION::SCREEN_Y, SETTINGS::RESOLUTION::PIXEL_SCALE_X, SETTINGS::RESOLUTION::PIXEL_SCALE_Y );
		//this->_layerLoading.create( SETTINGS::RESOLUTION::SCREEN_X, SETTINGS::RESOLUTION::SCREEN_Y, 64, 64);
		this->_layerLoading.create( 64, 64, 64, 64 ); // NOT PIXEl RESOLUTION BUT MATRIX SIZE (64 cells by 64 cells, 64 pixel by 64 pixel for the tile resolution)
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
		///

		Clear( olc::WHITE );


		//SetPixelMode( olc::Pixel::ALPHA );
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, olc::vf2d{55, 55}, olc::vi2d{ 5, 5 }, 1 ); // 5 visible tilex, 5 visible tiles y (need to calcualte #visible tiles based on zoom too!)
		//SetPixelMode( olc::Pixel::NORMAL );

		//std::cout << "state: loading" << std::endl;
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
		
		/*
		// called once per frame, draws random coloured pixels
		for ( int x = 0; x < ScreenWidth(); x++ )
			for ( int y = 0; y < ScreenHeight(); y++ )
				Draw( x, y, olc::Pixel( rand() % 256, rand() % 256, rand() % 256 ) );
		*/

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