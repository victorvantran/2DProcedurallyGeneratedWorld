#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "settings.h"
#include "GameState.h"
#include "Screen.h"
#include "Layer.h"
#include "Tile.h"
#include "World.h"
#include "Character.h"

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

	olc::Sprite* _spriteTileSetForest;


	/// Temporary Datatype for decals for testing purposes
	olc::Decal* _decalPlayerMouse;
	olc::Decal* _decalLoading;

	olc::Decal* _decalTileSetTinkerWorld;

	olc::Decal* _decalTileSetForest;


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

	bool* _playerCurrInputs;
	bool* _playerPrevInputs;

	olc::vf2d _playerMouse;
	olc::vf2d _playerCamera;

	Character _playerCharacter;



	Layer<Tile>* _layerLoading;


	/// Temporary Atlas Datatype
	Atlas* _atlasLoading;
	Atlas* _atlasTinkerWorld;
	Atlas _atlasForest;
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
		this->_spriteTileSetForest = new olc::Sprite( "C:\\Users\\Victor\\Desktop\\Tinker\\forest_2x2_8x8.png" );
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
		this->_decalTileSetForest = new olc::Decal( this->_spriteTileSetForest);
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
		this->_atlasLoading = new Atlas( this->_spriteLoading, this->_decalLoading, olc::vi2d(480, 270), settings::ATLAS::TILE_DIMENSION );
		this->_atlasTinkerWorld = new Atlas( this->_spriteTileSetTinkerWorld, this->_decalTileSetTinkerWorld, olc::vi2d( 25, 16 ), settings::ATLAS::TILE_DIMENSION );
		this->_atlasForest = Atlas( this->_spriteTileSetForest, this->_decalTileSetForest, olc::vi2d{ 2,2 }, settings::ATLAS::TILE_DIMENSION );
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
		this->_layerLoading = new Layer<Tile>( olc::vi2d{ 480, 270 } );
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
	///
		this->_playerCurrInputs = new bool[(int)KeyInput::count];
		this->_playerPrevInputs = new bool[( int )KeyInput::count];
		this->_playerMouse = olc::vf2d{ 0.0f, 0.0f };
		this->_playerCamera = olc::vf2d{ 0.0f, 0.0f };

		this->_playerCharacter = Character( this->_playerCurrInputs, this->_playerPrevInputs, olc::vf2d{ 12.0f, 8.0f } );
	}


	void destroyPlayer()
	{
	///
		delete[] this->_playerCurrInputs;
		delete[] this->_playerPrevInputs;

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
	///
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

		updatePlayer( fElapsedTime );

		Clear( olc::DARK_BLUE );

		//SetPixelMode( olc::Pixel::ALPHA );
		this->_pScreen->drawLayer( this->_layerLoading, this->_atlasLoading, this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f ); // [col, row] 128,72
		//SetPixelMode( olc::Pixel::NORMAL );

		this->drawPlayerMouse();
		return;
	}



	void runGameStateTinkerForestWorldLoading( float fElapsedTime )
	{
	/// Generate forests
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + olc::vi2d{ settings::WORLD_CHUNK::DIMENSION.x, 0 }, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + olc::vi2d{ 0, settings::WORLD_CHUNK::DIMENSION.y }, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + settings::WORLD_CHUNK::DIMENSION, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );

		this->_gameState = GameState::TINKER_WORLD;
		return;
	}


	void runGameStateTinkerForestWorld( float fElapsedTime )
	{
	/// Render forest world
		updatePlayer( fElapsedTime );


		Clear( olc::DARK_CYAN );

		//this->_pScreen->drawWorldChunk( *this->_world->getWorldChunks()[0], this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f );
		this->_pScreen->drawWorld( *this->_world, this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f );


		//olc::vf2d mouse = olc::vf2d{ ( float )this->GetMouseX(), ( float )this->GetMouseY() };
		//std::cout << "[" << ( int )( ( this->_playerCamera.x * 8.0f + mouse.x ) / 8.0f ) << "," << ( int )( ( this->_playerCamera.y * 8.0f + mouse.y ) / 8.0f ) << "]" << std::endl;
	
		
		// absolute pixel = {this->_playerCamera.x * 8.0f + mouse.x, this->_playerCamera.y * 8.0f + mouse.y ) / 8.0f}

		this->drawPlayerMouse();

		return;
	}



	void runGameStateTinkerWorldLoading( float fElapsedTime )
	{
	///
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + olc::vi2d{ settings::WORLD_CHUNK::DIMENSION.x, 0 }, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + olc::vi2d{ 0, settings::WORLD_CHUNK::DIMENSION.y }, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );
		this->_world->generateTestForest( settings::WORLD_CHUNK::POSITION + settings::WORLD_CHUNK::DIMENSION, settings::WORLD_CHUNK::DIMENSION, this->_atlasForest );

		this->_gameState = GameState::TINKER_WORLD;
		return;
	}


	void runGameStateTinkerWorld( float fElapsedTime )
	{
	///
		this->updatePlayer( fElapsedTime );
		this->_playerCharacter.updateCharacter( fElapsedTime, *this->_world );


		Clear( olc::DARK_CYAN );


		this->_playerCamera = this->_playerCharacter.getCurrPosition() - ( ( settings::RESOLUTION::SCREEN_DIMENSION / settings::ATLAS::TILE_DIMENSION ) / 2 ); // Make camera centered on character ( - (screendimension/tilesize)/2 )


		this->_pScreen->drawWorld( *this->_world, this->_playerCamera, olc::vi2d{ 96, 54 }, 1.0f );
		this->_pScreen->drawCharacter( this->_playerCharacter, this->_playerCamera );


		this->DrawCircle( olc::vi2d{ 768, 432 }/2, 1, olc::WHITE );
		return;
	}

public:
	void updatePlayer( float deltaTime )
	{
		this->updatePlayerMouse();
		this->resetPlayerInputs();
		this->updatePlayerInputs();
		this->updatePlayerCamera( deltaTime );

		return;
	}

	void updatePlayerMouse()
	{
	///
		this->_playerMouse = olc::vf2d{ (float)this->GetMouseX(), (float)this->GetMouseY() };

		return;
	}


	void updatePlayerInputs()
	{
	/// Updates the array of bool values that represent a key being pressed
		if ( this->GetKey( olc::Key::UP ).bPressed || this->GetKey( olc::Key::UP ).bHeld )
		{
			this->_playerCurrInputs[( int )KeyInput::UpKey] = true;
		}
		if ( this->GetKey( olc::Key::DOWN ).bPressed || this->GetKey( olc::Key::DOWN ).bHeld )
		{
			this->_playerCurrInputs[( int )KeyInput::DownKey] = true;
		}
		if ( this->GetKey( olc::Key::LEFT ).bPressed || this->GetKey( olc::Key::LEFT ).bHeld )
		{
			this->_playerCurrInputs[( int )KeyInput::LeftKey] = true;
		}
		if ( this->GetKey( olc::Key::RIGHT ).bPressed || this->GetKey( olc::Key::RIGHT ).bHeld )
		{
			this->_playerCurrInputs[( int )KeyInput::RightKey] = true;
		}
		if ( this->GetKey( olc::Key::SPACE ).bPressed || this->GetKey( olc::Key::SPACE ).bHeld )
		{
			this->_playerCurrInputs[( int )KeyInput::JumpKey] = true;
		}

		return;
	}

	void resetPlayerInputs()
	{
	/// Resets the array of bool values that represent a key being pressed
		for ( int i = 0; i < ( int )KeyInput::count; i++ )
		{
			this->_playerCurrInputs[i] = false;
		}

		return;
	}


	void updatePlayerCamera( float fElapsedTime )
	{
		float speed = 1.0f; // 1.0f is 1 tile size
		/*if ( this->IsFocused() )
		{
			if ( this->GetKey( olc::Key::UP ).bPressed || this->GetKey( olc::Key::UP ).bHeld )
			{
				this->_playerCamera.y += -speed*fElapsedTime;
			}

			if ( this->GetKey( olc::Key::DOWN ).bPressed || this->GetKey( olc::Key::DOWN ).bHeld )
			{
				this->_playerCamera.y += speed * fElapsedTime;
			}


			if ( this->GetKey( olc::Key::LEFT ).bPressed || this->GetKey( olc::Key::LEFT ).bHeld )
			{
				this->_playerCamera.x += -speed * fElapsedTime;
			}


			if ( this->GetKey( olc::Key::RIGHT ).bPressed || this->GetKey( olc::Key::RIGHT ).bHeld )
			{
				this->_playerCamera.x += speed * fElapsedTime;
			}
		}*/

		if ( this->IsFocused() )
		{
			if ( this->GetKey( olc::Key::W ).bPressed || this->GetKey( olc::Key::W ).bHeld )
			{
				this->_playerCamera.y += -speed * fElapsedTime;
			}

			if ( this->GetKey( olc::Key::S ).bPressed || this->GetKey( olc::Key::S ).bHeld )
			{
				this->_playerCamera.y += speed * fElapsedTime;
			}


			if ( this->GetKey( olc::Key::A ).bPressed || this->GetKey( olc::Key::A ).bHeld )
			{
				this->_playerCamera.x += -speed * fElapsedTime;
			}


			if ( this->GetKey( olc::Key::D ).bPressed || this->GetKey( olc::Key::D ).bHeld )
			{
				this->_playerCamera.x += speed * fElapsedTime;
			}
		}
	}


	void drawPlayerMouse()
	{
	///
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
		if ( demo.Construct( settings::RESOLUTION::SCREEN_DIMENSION.x, settings::RESOLUTION::SCREEN_DIMENSION.y, settings::RESOLUTION::PIXEL_SCALE.x, settings::RESOLUTION::PIXEL_SCALE.y ) )
		{
			demo.Start();
		}
		return 0;
	}
}