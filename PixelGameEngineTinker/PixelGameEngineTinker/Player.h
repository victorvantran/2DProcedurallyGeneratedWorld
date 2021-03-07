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
	
	/*
	void World::saveWorldGeography()
	{
		// Saves all the worldChunks that have been built up in the queue into the SQLite database
		std::lock_guard<std::mutex> lockSave( this->_mutexSaveWorldChunks );

		if ( this->_saveWorldChunks.empty() )
		{
			return;
		}

		std::unique_lock<std::mutex> lockDatabase( this->_worldDatabaseMutex );

		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;
		command =
			"INSERT INTO world_geography( chunk_index_x, chunk_index_y, tiles, palette )\n"
			"VALUES ( ?1, ?2, ?3, ?4 )\n"
			"ON CONFLICT( chunk_index_x, chunk_index_y ) DO UPDATE SET tiles = ?3, palette = ?4;";

		sqlite3_open( "./world.db", &database );

		// Save all blobs/palettes in one go
		for ( int i = 0; i < this->_saveWorldChunks.size(); i++ )
		{
			std::cout << "Saving Chunk: [" << this->_saveWorldChunks[i]->getChunkIndexX() << "," << this->_saveWorldChunks[i]->getChunkIndexY() << "]" << std::endl; // [~!]
			// save

			WorldChunkMemory* worldChunkMemory = this->_saveWorldChunks[i];

			std::int64_t chunkIndexX = worldChunkMemory->getChunkIndexX();
			std::int64_t chunkIndexY = worldChunkMemory->getChunkIndexY();

			unsigned char* tilesBlob = worldChunkMemory->getTilesBlob();
			std::uint64_t* paletteBlob = worldChunkMemory->getPaletteBlob();

			std::uint16_t tilesBlobNumBytes = worldChunkMemory->getTilesBlobNumBytes();
			std::uint16_t paletteBlobNumBytes = worldChunkMemory->getPaletteBlobNumBytes();

			rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
			rc = sqlite3_bind_int( statement, 1, chunkIndexX );
			rc = sqlite3_bind_int( statement, 2, chunkIndexY );
			rc = sqlite3_bind_blob( statement, 3, tilesBlob, tilesBlobNumBytes, SQLITE_STATIC );
			rc = sqlite3_bind_blob( statement, 4, paletteBlob, paletteBlobNumBytes, SQLITE_STATIC );
			rc = sqlite3_step( statement );
			rc = sqlite3_finalize( statement );

		}

		sqlite3_close( database );
		lockDatabase.unlock();

		for ( int i = 0; i < _saveWorldChunks.size(); i++ )
		{
			delete _saveWorldChunks[i];
		}
		this->_saveWorldChunks.clear();

		return;
	}
	*/

	static int callback( void* NotUsed, int argc, char** argv, char** azColName ) {

		// int argc: holds the number of results
		// (array) azColName: holds each column returned
		// (array) argv: holds each value

		// Return successful
		return 0;
	}

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

		// Save all blobs/palettes in one go

		// save

		/*
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		char* value = _strdup( "somevalue" );
		rc = sqlite3_bind_text( statement, 1, value, -1, free );
		rc = sqlite3_bind_int( statement, 2, 1 );
		rc = sqlite3_step( statement );
		rc = sqlite3_finalize( statement );
		*/

		//const char* newName = "Playa";

		
		std::string newName = "Player";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_text( statement, 1, newName.c_str(), newName.length(), SQLITE_TRANSIENT );
		rc = sqlite3_bind_int( statement, 2, 8675309 );
		rc = sqlite3_step( statement );
		rc = sqlite3_finalize( statement );
		

		/*
		std::string sql = "INSERT INTO PLAYER ('ID', 'NAME') VALUES ('3', 'KEY');";

		rc = sqlite3_exec( database, sql.c_str(), callback, 0, &errorMessage );

		sqlite3_close( database );

		std::cout << "SAVED NEW NAME" << std::endl;
		*/

		/*
		std::string sql = "INSERT INTO player('LEVEL', 'NAME') VALUES ('3', 'KEY');";

		rc = sqlite3_exec( database, sql.c_str(), callback, 0, &errorMessage );
		*/


		sqlite3_close( database );
		
		std::cout << "Saved Player" << std::endl;

		sqlite3_close( database );

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
		
		/*
		command =
			"CREATE TABLE IF NOT EXISTS player\n"\
			"(\n"\
			"	name TEXT,\n"\
			"	level INT,\n"\
			");";
		*/
		
		
		command =
			"CREATE TABLE IF NOT EXISTS player"\
			"("\
			"	name TEXT NOT NULL,"\
			"	level INT NOT NULL"\
			");";
			

		/*
		command =
			"CREATE TABLE IF NOT EXISTS PLAYER"\
			"("\
			"	ID INT PRIMARY KEY NOT NULL,"\
			"	NAME TEXT NOT NULL"\
			");";
		*/
		/*
		command = "CREATE TABLE IF NOT EXISTS PLAYER ("  \
			"ID INT PRIMARY KEY     NOT NULL," \
			"NAME           TEXT    NOT NULL);";
		*/

		/*
		command = "CREATE TABLE PLAYER ("  \
			"ID INT PRIMARY KEY     NOT NULL," \
			"NAME           TEXT    NOT NULL);";
		*/
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
	

	/*
	void loadDatabase()
	{
		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;

		//std::vector<std::int64_t> proceduralGenerateCache;

		sqlite3_open( "./player.db", &database );


		// Check if world chunk ( chunkIndexX, chunkIndexY ) exists
		command =
			"SELECT EXISTS( SELECT 1 FROM world_geography WHERE chunk_index_x = ?1 AND chunk_index_y = ?2 );";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );


		// If it does not exists, return or throw error
		bool exists = sqlite3_column_int( statement, 0 );
		if ( !exists )
		{
			rc = sqlite3_step( statement );
			rc = sqlite3_finalize( statement );

			std::cout << "Chunk does has not existed beforehand. Procedural generation" << std::endl; // [~!]
			// std::cout << this->_worldChunks[index].getRelativeChunkIndex() << std::endl;
			//proceduralGenerateCache.push_back( index );
			this->procedurallyGenerateChunk( this->_worldChunks[index] );
			continue;
		}
		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		// Select tile_data and palette based on primary key ( chunkIndexX, chunkIndexY )
		command =
			"SELECT tiles, palette\n"
			"FROM world_geography\n"
			"WHERE chunk_index_x = ? AND chunk_index_y = ?;";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );

		// Create tilesData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesTiles = sqlite3_column_bytes( statement, 0 );
		unsigned char* tilesData = new unsigned char[numBytesTiles];
		std::memcpy( tilesData, sqlite3_column_blob( statement, 0 ), numBytesTiles );

		// Create paletteData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesPalette = sqlite3_column_bytes( statement, 1 );
		std::uint16_t numUniqueKeys = numBytesPalette / sizeof( std::uint64_t );
		std::uint64_t* paletteData = new std::uint64_t[numUniqueKeys];
		std::memcpy( paletteData, sqlite3_column_blob( statement, 1 ), numBytesPalette );

		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );



		sqlite3_close( database );
	}
	*/


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
		//if ( pge.GetKey( olc::Key::K0 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::ZERO] = true;
		//if ( pge.GetKey( olc::Key::K1 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::ONE] = true;
		//if ( pge.GetKey( olc::Key::K2 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::TWO] = true;
		//if ( pge.GetKey( olc::Key::K3 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::THREE] = true;
		//if ( pge.GetKey( olc::Key::K4 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::FOUR] = true;
		//if ( pge.GetKey( olc::Key::K5 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::FIVE] = true;
		//if ( pge.GetKey( olc::Key::K6 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SIX] = true;
		//if ( pge.GetKey( olc::Key::K7 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SEVEN] = true;
		//if ( pge.GetKey( olc::Key::K8 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::EIGHT] = true;
		//if ( pge.GetKey( olc::Key::K9 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::NINE] = true;
		

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