#pragma once

#include <cstdint>

class WorldMemoryManager; // Forward Declaration
class WorldChunk; // Forward Declaration

class WorldChunkMemory
{
private:
	int _chunkIndexX;
	int _chunkIndexY;

	unsigned char* _tilesBlob;
	std::uint64_t* _paletteBlob;

	std::uint16_t _tilesBlobNumBytes;
	std::uint16_t _paletteBlobNumBytes;
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