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


void World::saveWorldAtlas()
{
	// Overwrite the entire worldMap.dat file
	MemoryManager::saveWorldAtlas( "./worldAtlas.dat", this->_worldAtlas ); // [!] singleton
	return;
}


void World::loadWorldAtlas()
{
	// Load the items ( index : offset ) from worldMap.dat into this->_worldAtlas
	MemoryManager::loadWorldAtlas( "./worldAtlas.dat", this->_worldAtlas ); // [!] singleton
	return;
}


void World::viewWorldAtlas()
{
	MemoryManager::viewWorldAtlas( this->_worldAtlas );
	return;
}


void World::updateWorldAtlas( int indexX, int indexY )
{
	// Update the worldAtlas on the drive by appending new data ( index : offset ) 
	if ( this->_worldAtlas.find( std::tuple<int, int>{ indexX, indexY } ) == this->_worldAtlas.end() )
	{
		MemoryManager::updateWorldAtlas( "./worldAtlas.dat", indexX, indexY, this->_worldAtlas.size() );
	}
	return;
}


bool World::findWorldMap( int indexX, int indexY ) const
{
	return this->_worldAtlas.find( std::tuple<int, int>{ indexX, indexY } ) != this->_worldAtlas.end();
}


void World::saveWorldChunk( WorldChunk& worldChunk )
{
	//MemoryManager::saveWorldChunkCrude( "./world.dat", worldChunk, this->_worldAtlas );
	MemoryManager::saveWorldChunkCrude( "./world.dat", worldChunk, this->_worldAtlas.at( std::tuple<int, int>{worldChunk.getChunkIndexX(), worldChunk.getChunkIndexY()} ) );
	return;
}


void World::loadWorldChunk( WorldChunk& worldChunk )
{
	MemoryManager::loadWorldChunkCrude( "./world.dat", worldChunk, this->_worldAtlas.at( std::tuple<int, int>{worldChunk.getChunkIndexX(), worldChunk.getChunkIndexY()} ) );
	return;
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

			if ( this->_worldAtlas.find( std::tuple<int, int>{newChunkIndexX, newChunkIndexY} ) == this->_worldAtlas.end() )
			{
				this->updateWorldAtlas( newChunkIndexX, newChunkIndexY );
				this->_worldAtlas.emplace( std::tuple<int, int>{ newChunkIndexX, newChunkIndexY }, this->_worldAtlas.size() );
			}
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
		this->loadWorldChunk( this->_worldChunks[i] );
		// [!] Load in world chunks here ( from history or procedural )
		// [!] if ( this->_worldAtlas.find( std::tuple<int, int>{newChunkIndexX, newChunkIndexY} ) == this->_worldAtlas.end() ) ? procedural : history
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

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			WorldChunk* worldChunk = &this->_worldChunks[y * this->_numChunkWidth + x];
			int worldChunkIndexX = worldChunk->getChunkIndexX();
			int worldChunkIndexY = worldChunk->getChunkIndexY();

			// If the worldChunk is out of range relative to the camera
			if ( std::abs( cameraIndexX - worldChunkIndexX ) > this->_chunkRadius || std::abs( cameraIndexY - worldChunkIndexY ) > this->_chunkRadius )
			{
				int newChunkIndexX = cameraIndexX + -( worldChunkIndexX - this->_prevCameraIndexX );
				int newChunkIndexY = cameraIndexY + -( worldChunkIndexY - this->_prevCameraIndexY );

				// Save world chunk
				this->saveWorldChunk( *worldChunk );

				// Delimit
				this->delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );
				
				// Update the world atlas if found new chunk that is not been discovered yet
				if ( this->_worldAtlas.find( std::tuple<int, int>{newChunkIndexX, newChunkIndexY} ) == this->_worldAtlas.end() )
				{
					this->updateWorldAtlas( newChunkIndexX, newChunkIndexY );
					this->_worldAtlas.emplace( std::tuple<int, int>{ newChunkIndexX, newChunkIndexY }, this->_worldAtlas.size() );
					///this->_worldChunks[y * this->_numChunkWidth + x].fill( 1 ); // [!] test
					// [!] Procedural generation here
				}
				// Else we load in the worldChunk data here
				else
				{
					this->loadWorldChunk( *worldChunk );
				}

			}
		}
	}

	// Update camera index
	this->_prevCameraIndexX = cameraIndexX;
	this->_prevCameraIndexY = cameraIndexY;

	return;
}


void World::replaceWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY ) // [!] may not need
{
	this->saveWorldChunk( worldChunk );
	this->loadWorldChunk( worldChunk );
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