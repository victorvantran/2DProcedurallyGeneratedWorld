#pragma once

#include <mutex>
#include "sqlite/sqlite3.h"
#include "olcPixelGameEngine.h"
#include "Character.h"

enum class PlayerKeyInput
{
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	SPACE,
	count
};


 
// Player -> key Inputs
// Player ->update Commands
// Character ->read Commands
// Fixed Update



class World;

class Player
{
private:
	Character _character;
	World* _world;

	bool _prevInputs[( std::size_t )PlayerKeyInput::count];
	bool _currInputs[( std::size_t )PlayerKeyInput::count];
public:
	// Constructors/Destructors
	Player() : _character(), _world( nullptr ), _prevInputs{ false }, _currInputs{ false } 
	{
		this->initializeDatabase();
	}

	Player( olc::v2d_generic<long double> center, olc::vf2d halfSize, olc::vf2d scale, CharacterState characterState, float runSpeed, float jumpSpeed, World* world, float time,
		olc::Sprite* characterSprite, olc::Decal* characterDecal ) :
		_character( center, halfSize, scale, characterState, runSpeed, jumpSpeed, world, time,
			characterSprite, characterDecal ),
		_world( world ),
		_prevInputs{ false }, _currInputs{ false }
	{
		this->initializeDatabase();
	}


	~Player() 
	{
		this->saveDatabase();
	}


	// Getters
	Character& getCharacter()
	{
		return this->_character;
	}


	// Setters

	void saveDatabase()
	{
		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;
		command =
			"INSERT INTO player( name, level )\n"
			"VALUES ( ?1, ?2 )\n";

		sqlite3_open( "./player.db", &database );


		std::string newName = "Player";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_text( statement, 1, newName.c_str(), newName.length(), SQLITE_TRANSIENT );
		rc = sqlite3_bind_int( statement, 2, 8675309 );
		rc = sqlite3_step( statement );
		rc = sqlite3_finalize( statement );
		

		sqlite3_close( database );
		
		std::cout << "Saved Player" << std::endl;
		return;
	}

	
	void initializeDatabase()
	{
		// [!] set player first to the world? Or ideally have world set player only after initialize is done
		// Create a player file if one does not exist already.

		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;

		sqlite3_open( "./player.db", &database );

		// Create Table [ chunk_x, chunk_y ] -> [ tiles, palette ]
		
		command =
			"CREATE TABLE IF NOT EXISTS player"\
			"("\
			"	name TEXT NOT NULL,"\
			"	level INT NOT NULL"\
			");";
			
		rc = sqlite3_exec(
			database,
			command,
			NULL,
			NULL,
			&errorMessage
		);


		if ( rc != SQLITE_OK )
		{
			sqlite3_close( database );
		}

		sqlite3_close( database );
		return;
	}
	


	// Methods
	void resetInputs()
	{
		for ( std::size_t i = 0; i < ( std::size_t )PlayerKeyInput::count; i++ )
		{
			this->_currInputs[i] = false;
		}
		return;
	}


	
	void updateInputs( olc::PixelGameEngine& pge )
	{
		// Given key inputs, cache the inputs from the current frame 
		if ( pge.GetKey( olc::Key::W ).bPressed || pge.GetKey( olc::Key::W ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::W] = true;
		if ( pge.GetKey( olc::Key::A ).bPressed || pge.GetKey( olc::Key::A ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::A] = true;
		if ( pge.GetKey( olc::Key::S ).bPressed || pge.GetKey( olc::Key::S ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::S] = true;
		if ( pge.GetKey( olc::Key::D ).bPressed || pge.GetKey( olc::Key::D ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::D] = true;
		if ( pge.GetKey( olc::Key::SPACE ).bPressed || pge.GetKey( olc::Key::SPACE ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SPACE] = true;

		return;
	}
	

	void updatePrevInputs()
	{
		// Cache inputs from previous frame
		std::size_t count = ( std::size_t )PlayerKeyInput::count;

		for ( std::size_t i = 0; i < ( std::size_t )PlayerKeyInput::count; i++ )
		{
			this->_prevInputs[i] = this->_currInputs[i];
		}

		return;
	}

	bool* createCharacterCommands()
	{
		// Create current commands for character based on current player inputs
		bool characterCommands[( std::size_t )Command::count];

		characterCommands[( std::size_t )Command::GoRight] = this->_currInputs[( std::size_t )PlayerKeyInput::D];
		characterCommands[( std::size_t )Command::GoLeft] = this->_currInputs[( std::size_t )PlayerKeyInput::A];
		characterCommands[( std::size_t )Command::Jump] = this->_currInputs[( std::size_t )PlayerKeyInput::W];
		characterCommands[( std::size_t )Command::Drop] = this->_currInputs[( std::size_t )PlayerKeyInput::S];
		//characterCommands[( std::size_t )Command::Drop] = false; // [!] no drop implemented yet

		return characterCommands;
	}


	void update( olc::PixelGameEngine& pge );
	void render();
};