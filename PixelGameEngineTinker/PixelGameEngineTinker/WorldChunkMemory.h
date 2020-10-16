#pragma once

#include <cstdint>

class WorldMemoryManager; // Forward Declaration
class WorldChunk; // Forward Declaration

class WorldChunkMemory
{
private:
	int _chunkIndexX;
	int _chunkIndexY;

	unsigned char* _tilesBlob; // = createTilesBlob( tiles, palette );
	std::uint64_t* _paletteBlob; // = createPaletteBlob( palette );

	std::uint16_t _tilesBlobNumBytes; // = ( int )ceil( ( float )tilesBlobNumBits / 8.0f );
	std::uint16_t _paletteBlobNumBytes; // = palette.size() * 8;
public:
	WorldChunkMemory();
	WorldChunkMemory( WorldChunk* worldChunk );
	~WorldChunkMemory();

	int getChunkIndexX();
	int getChunkIndexY();

	unsigned char* getTilesBlob();
	std::uint64_t* getPaletteBlob();

	std::uint16_t getTilesBlobNumBytes();
	std::uint16_t getPaletteBlobNumBytes();

};