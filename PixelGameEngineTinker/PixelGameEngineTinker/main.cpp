#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "settings.h"
#include "GameState.h"
#include "Screen.h"



// Override base class with your custom functionality
class Game : public olc::PixelGameEngine
{
private:
	GameState _gameState;
	olc::Screen* _pScreen;


	///

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
		// initialize the screen
		// run through the first state of the game
		sAppName = "Example";
		this->_gameState = GameState::LOADING;
		this->_pScreen = new olc::Screen();
		return;
	}

	void destroyGame()
	{
		// free memory of what was used in Game::initalizeGame()
		delete this->_pScreen;
		return;
	}


	GameState getGameState()
	{
		return this->_gameState;
	}


	void runGameStateLoading( float fElapsedTime )
	{
		//
		//this->_pScreen->drawLayer();
		return;
	}


	void runGameStateTitle( float fElapsedTime )
	{
		//
		return;
	}


	void runGameStateTinker( float fElapsedTime )
	{
		//
		return;
	}



public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
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