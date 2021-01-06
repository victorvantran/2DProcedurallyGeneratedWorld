#include "WorldChunkMemory.h"
#include "World.h"
#include "WorldChunk.h"

WorldChunkMemory::WorldChunkMemory()
	: _chunkIndexX( 0 ), _chunkIndexY( 0 ),
	_tilesBlob( nullptr ), _paletteBlob( nullptr ),
	_tilesBlobNumBytes( 0 ), _paletteBlobNumBytes( 0 )
{

}


WorldChunkMemory::WorldChunkMemory( WorldChunk* worldChunk )
{
	// Construct a worldChunkMemory comprising of tilesBlob and its respective paletteBlob to be saved in the SQLite database

	// Get tiles and palette from the worldChunk
	Tile* tiles = worldChunk->getTiles();
	std::vector<std::uint64_t> palette = worldChunk->getPalette();

	// Get the worldChunk index used as the primary composite key to our table
	this->_chunkIndexX = worldChunk->getChunkIndexX();
	this->_chunkIndexY = worldChunk->getChunkIndexY();

	// Get numBytes for worldChunkBlob and paletteBlob
	std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
	std::uint16_t numBitsPerKey = ( ceil( log2( numUniqueKeys ) ) ) > 0 ? ( ceil( log2( numUniqueKeys ) ) ) : 1; // max is 10 bits // [!] 8 to 16
	std::uint32_t tilesBlobNumBits = ( numBitsPerKey * Settings::World::NUM_CELLS_PER_CHUNK ); // max is 32*32*64

	this->_tilesBlobNumBytes = ( int )ceil( ( float )tilesBlobNumBits / 8.0f );
	this->_paletteBlobNumBytes = palette.size() * 8;

	this->_tilesBlob = World::createTilesBlob( tiles, palette );
	this->_paletteBlob = World::createPaletteBlob( palette );
}


WorldChunkMemory::~WorldChunkMemory()
{
	delete[] this->_tilesBlob;
	this->_tilesBlob = nullptr;

	delete[] this->_paletteBlob;
	this->_paletteBlob = nullptr;
}


std::int64_t WorldChunkMemory::getChunkIndexX()
{
	return this->_chunkIndexX;
}


std::int64_t WorldChunkMemory::getChunkIndexY()
{
	return this->_chunkIndexY;
}


unsigned char* WorldChunkMemory::getTilesBlob()
{
	return this->_tilesBlob;
}


std::uint64_t* WorldChunkMemory::getPaletteBlob()
{
	return this->_paletteBlob;
}


std::uint16_t  WorldChunkMemory::getTilesBlobNumBytes()
{
	return this->_tilesBlobNumBytes;
}


std::uint16_t WorldChunkMemory::getPaletteBlobNumBytes()
{
	return this->_paletteBlobNumBytes;
}
