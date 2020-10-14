#include "World.h"


World::World()
	: _numChunkWidth( 1 + this->_chunkRadius * 2 ), _numChunkHeight( 1 + this->_chunkRadius * 2 ), _numWorldChunks( this->_numChunkWidth* this->_numChunkHeight ),
	_prevCameraIndexX( 0 ), _prevCameraIndexY( 0 ),
	_worldChunks( new WorldChunk[( 1 + 2 * ( _chunkRadius ) ) * ( 1 + 2 * ( _chunkRadius ) )] )
{
}


World::~World()
{
	delete[] this->_worldChunks;
}


void World::insert( int x, int y, int width, int height, uint64_t id )
{
	// We can leverage the robustness of the quad tree and just try to add onto all the worldChunks
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<int>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].insert( x, y, width, height, id );
		}
	}
	return;
}


void World::remove( int x, int y, int width, int height, uint64_t id )
{
	// We can leverage the robustness of the quad tree and just try to remove onto all the worldChunks
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<int>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].remove( x, y, width, height, id );
		}
	}
	return;
}



void World::saveWorldChunk( WorldChunk* worldChunk )
{
	MemoryManager::saveWorldChunk( worldChunk ); // [!] save and load have memory leaks!
	return;
}


bool World::loadWorldChunk( WorldChunk* worldChunk )
{
	return MemoryManager::loadWorldChunk( worldChunk ); // [!] save and load have memory leaks!
}


void World::delimitWorldChunk( WorldChunk& worldChunk, int chunkIndexX, int chunkIndexY )
{
	// Clears the worldChunk, updates its proper index, and wipes the render in preparation to load in new data	and new render
	worldChunk.clear();
	worldChunk.delimit( chunkIndexX, chunkIndexY );
	worldChunk.wipeRender();
	return;
}


void World::initializeDelimits( const BoundingBox<float>& cameraView )
{
	// Based on the initial camera's position, delimit the surrounding worldChunks
	// Load in camera
	int cameraIndexX = cameraView.getCenterX() >= 0 ? ( int )( cameraView.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraView.getCenterY() >= 0 ? ( int )( cameraView.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			int deltaIndexX = x - this->_chunkRadius;
			int deltaIndexY = y - this->_chunkRadius;

			int newChunkIndexX = cameraIndexX + deltaIndexX;
			int newChunkIndexY = cameraIndexY + deltaIndexY;

			this->delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );
		}
	}

	this->_prevCameraIndexX = cameraIndexX;
	this->_prevCameraIndexY = cameraIndexY;

	return;
}


void World::initializeWorldChunks()
{
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		this->_worldChunks[i].construct();
		if ( !this->loadWorldChunk( &this->_worldChunks[i] ) )
		{
			// [!] Procedural generation here
		}
	}

	return;
}



void World::delimitWorldChunks( const BoundingBox<float>& cameraView )
{
	// Based on the updated camera's position, delimit the surrounding worldChunks
	int cameraIndexX = cameraView.getCenterX() >= 0 ? ( int )( cameraView.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraView.getCenterY() >= 0 ? ( int )( cameraView.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	// No need to delimit if the camera index has not changed
	if ( this->_prevCameraIndexX == cameraIndexX && this->_prevCameraIndexY == cameraIndexY )
	{
		return;
	}

	bool delimit = false;
	auto t1 = std::chrono::high_resolution_clock::now();

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			WorldChunk *worldChunk = &this->_worldChunks[y * this->_numChunkWidth + x];
			int worldChunkIndexX = worldChunk->getChunkIndexX();
			int worldChunkIndexY = worldChunk->getChunkIndexY();

			// If the worldChunk is out of range relative to the camera
			if ( std::abs( cameraIndexX - worldChunkIndexX ) > this->_chunkRadius || std::abs( cameraIndexY - worldChunkIndexY ) > this->_chunkRadius )
			{
				delimit = true;

				int newChunkIndexX = cameraIndexX + -( worldChunkIndexX - this->_prevCameraIndexX );
				int newChunkIndexY = cameraIndexY + -( worldChunkIndexY - this->_prevCameraIndexY );

				// Save world chunk
				this->saveWorldChunk( worldChunk );

				// Delimit
				this->delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );

				// [!] instead of having a worldatlas to grow, we can just check if it is among the surrounding
				if ( !this->loadWorldChunk( worldChunk ) )
				{
					// [!] Procedural generation here
				}


			}
		}
	}
	auto t2 = std::chrono::high_resolution_clock::now();
	if ( delimit )
	{
		std::cout << "Delta t2-t1: "
			<< std::chrono::duration_cast< std::chrono::nanoseconds >( t2 - t1 ).count()
			<< " nanoseconds" << std::endl;
	}


	// Update camera index
	this->_prevCameraIndexX = cameraIndexX;
	this->_prevCameraIndexY = cameraIndexY;

	return;
}




WorldChunk& World::getWorldChunk( int x, int y )
{
	return this->_worldChunks[y * this->_numChunkWidth + x];
}


WorldChunk* World::getWorldChunks()
{
	return this->_worldChunks;
}


int World::getChunkRadius() const
{
	return this->_chunkRadius;
}


int World::getNumWorldChunks() const
{
	return this->_numWorldChunks;
}


int World::getNumChunkWidth() const
{
	return this->_numChunkWidth;
}


int World::getNumChunkHeight() const
{
	return this->_numChunkHeight;
}