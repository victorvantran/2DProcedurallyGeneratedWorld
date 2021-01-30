#include "Assets.h"

// Objects
Assets* Assets::_instance = nullptr;

std::mutex Assets::_readSpriteDatabase;


Assets::Assets()
{
	this->loadSpritesDatabase();
}


Assets::~Assets()
{
	// Deallocate
}


Assets& Assets::get()
{
	if ( !_instance )
	{
		_instance = new Assets();
	}
	return *_instance;
}


void Assets::loadSpritesDatabase()
{
	// Create a sprites database file if one is not created yet
	// Tables: tiles, ...

	std::lock_guard<std::mutex> accessDatabaseLock( Assets::_readSpriteDatabase );

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	sqlite3_open( Settings::Assets::SPRITES_FILE_PATH.c_str(), &database );

	// Create Table [ chunk_x, chunk_y ] -> [ tiles, palette ]
	command =
		"CREATE TABLE IF NOT EXISTS tiles\n"\
		"(\n"\
		"	tile_id INT,\n"\
		"	tileset_png BLOB,\n"\
		"	PRIMARY KEY ( tile_id )\n"\
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


olc::Sprite* Assets::loadSpriteTile( std::uint64_t tileId )
{
	// Create and return sprite based on a loaded png blob from the SQLite database (sprites.db)
	std::unique_lock<std::mutex> accessDatabaseLock( Assets::_readSpriteDatabase );

	std::uint64_t tileset_png_NumBytes = 0;
	char* tileset_png_Blob = nullptr;

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	sqlite3_open( Settings::Assets::SPRITES_FILE_PATH.c_str(), &database );

	// Check if world chunk ( chunkIndexX, chunkIndexY ) exists
	command =
		"SELECT EXISTS( SELECT 1 FROM tiles WHERE tile_id = ?1 );";
	rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
	rc = sqlite3_bind_int( statement, 1, tileId );
	rc = sqlite3_step( statement );


	// If it does not exists, return or throw error
	bool exists = sqlite3_column_int( statement, 0 );
	if ( !exists )
	{
		rc = sqlite3_step( statement );
		rc = sqlite3_finalize( statement );
		sqlite3_close( database );

		std::cout << "[tiles] Row has not existed beforehand." << std::endl;
		return nullptr;
	}

	rc = sqlite3_step( statement ); // Extra step for null
	rc = sqlite3_finalize( statement );

	// Select tile_data and palette based on primary key ( chunkIndexX, chunkIndexY )
	command =
		"SELECT tileset_png\n"
		"FROM tiles\n"
		"WHERE tile_id = ?;";
	rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
	rc = sqlite3_bind_int( statement, 1, tileId );
	rc = sqlite3_step( statement );

	// Create tilesData used as reference when reconstructing the worldChunk's tiles
	tileset_png_NumBytes = sqlite3_column_bytes( statement, 0 );
	tileset_png_Blob = new char[tileset_png_NumBytes];
	std::memcpy( tileset_png_Blob, sqlite3_column_blob( statement, 0 ), tileset_png_NumBytes );

	rc = sqlite3_step( statement ); // Extra step for null
	rc = sqlite3_finalize( statement );
	sqlite3_close( database );

	accessDatabaseLock.unlock();

	olc::Sprite* sprite = new olc::Sprite( tileset_png_Blob, tileset_png_NumBytes ); // to be deleted by World class

	delete[] tileset_png_Blob;

	return sprite;
}





std::map<std::uint64_t, olc::Sprite*> Assets::loadSpriteTiles( std::vector<std::uint64_t> tileIds )
{
	// Create and return sprites based on a loaded png blob from the SQLite database (sprites.db)
	std::unique_lock<std::mutex> accessDatabaseLock( Assets::_readSpriteDatabase );

	std::map<std::uint64_t, olc::Sprite*> spriteTilesMap;


	std::uint64_t tileset_png_NumBytes = 0;
	char* tileset_png_Blob = nullptr;

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	sqlite3_open( Settings::Assets::SPRITES_FILE_PATH.c_str(), &database );

	for ( std::uint64_t tileId : tileIds )
	{
		// Check if world chunk ( chunkIndexX, chunkIndexY ) exists
		command =
			"SELECT EXISTS( SELECT 1 FROM tiles WHERE tile_id = ?1 );";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, tileId );
		rc = sqlite3_step( statement );


		// If it does not exists, return or throw error
		bool exists = sqlite3_column_int( statement, 0 );
		if ( !exists )
		{
			rc = sqlite3_step( statement );
			rc = sqlite3_finalize( statement );
			sqlite3_close( database );

			std::cout << "[tiles] Row has not existed beforehand." << std::endl;
			return spriteTilesMap; // [!] throw error
		}

		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		// Select tile_data and palette based on primary key ( chunkIndexX, chunkIndexY )
		command =
			"SELECT tileset_png\n"
			"FROM tiles\n"
			"WHERE tile_id = ?;";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, tileId );
		rc = sqlite3_step( statement );

		// Create tilesData used as reference when reconstructing the worldChunk's tiles
		tileset_png_NumBytes = sqlite3_column_bytes( statement, 0 );
		tileset_png_Blob = new char[tileset_png_NumBytes];
		std::memcpy( tileset_png_Blob, sqlite3_column_blob( statement, 0 ), tileset_png_NumBytes );

		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		olc::Sprite* sprite = new olc::Sprite( tileset_png_Blob, tileset_png_NumBytes ); // to be deleted by World class
		spriteTilesMap.emplace( tileId, sprite );

		delete[] tileset_png_Blob;
		tileset_png_Blob = nullptr;

	}
	
	sqlite3_close( database );
	accessDatabaseLock.unlock();

	return spriteTilesMap;
}

