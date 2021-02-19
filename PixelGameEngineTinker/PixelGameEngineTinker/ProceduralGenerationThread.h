#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "olcPixelGameEngine.h"
#include "WorldChunk.h"
#include "Tile.h"


class ProceduralGenerationThread
{
private:
	bool _alive = true;
	std::mutex _mutexGenerate;
	std::condition_variable _condStart;
	std::thread _thread;

	WorldChunk* _worldChunk;

public:
	ProceduralGenerationThread() {}
	~ProceduralGenerationThread() {}


	void execute()
	{
		//thread_pool a;
		std::unique_lock<std::mutex> lock( this->_mutexGenerate );
		this->_condStart.notify_one();
	}


	void generate()
	{
		while ( this->_alive )
		{
			std::unique_lock<std::mutex> lock( this->_mutexGenerate );

			if ( this->_worldChunk != nullptr )
			{
				this->_condStart.wait( lock );

				static const std::int64_t OVERWORLD_HEIGHT = 1536; // [!] put in settings
				TileIdentity* proceduralChunk = this->getProceduralChunk( this->_worldChunk->getChunkIndexX(), this->_worldChunk->getChunkIndexY() );
				for ( int row = 0; row < 32; row++ )
				{
					for ( int col = 0; col < 32; col++ )
					{
						this->_worldChunk->insert( proceduralChunk[row * 32 + col], 0, this->_worldChunk->getChunkIndexX() * 32 + col, this->_worldChunk->getChunkIndexY() * 32 + row, 1, 1 ); // [~!]
					}
				}

				delete[] proceduralChunk;

				return;
			}
		}
	}



	static TileIdentity* getProceduralChunk( std::int64_t chunkIndexX, std::int64_t chunkIndexY )
	{
		// Get topleft coordinates of a chunk and procedurally generate
		static const std::int64_t OVERWORLD_HEIGHT = 1536;

		std::int64_t originX = chunkIndexX * 32;
		std::int64_t originY = chunkIndexY * 32;

		TileIdentity* chunk = new TileIdentity[32 * 32];

		for ( int row = 0; row < 32; row++ )
		{
			for ( int col = 0; col < 32; col++ )
			{
				if ( row == 0 )
				{
					chunk[row * 32 + col] = TileIdentity::Stone;

				}
				else if ( col != 5 )
				{
					chunk[row * 32 + col] = TileIdentity::Water;

				}
				else
				{
					chunk[row * 32 + col] = TileIdentity::Void;

				}
			}
		}

		return chunk;

	}
};


