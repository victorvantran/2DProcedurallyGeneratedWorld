#pragma once

#include "olcPixelGameEngine.h"
#include "sqlite//sqlite3.h"
#include <mutex>

#include "Settings.h"

/*
Singleton:
	Single location that loads sprites, decals, and variables
	These loaded resources remains persistent
*/

class Assets
{
private:
	// Make the constructor and destructors private, so the user cannot create an instance(s) of the singleton	
	static Assets* _instance;

	Assets();
	~Assets();


	void loadSpritesDatabase();
	static std::mutex _readSpriteDatabase;

public:
	static olc::Sprite* assetSprite;
	static olc::Decal* assetDecal;

	// Delete the default copy constructor created by the compiler.
	// We do not want to create shallow copies of this and subsequently reload the assets
	Assets( const Assets& ) = delete;

	// Delete the default assignment operator created by the compiler.
	// We do not want to create shallow re-assignments of member variables (subseqently reloading)
	Assets& operator=( const Assets& ) = delete;

	static Assets& get();
	static olc::Sprite* loadSpriteTile( std::uint64_t tileId );

};
