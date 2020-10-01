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


void World::insert( const TileConsolidated& tileConsolidated )
{
	// We can leverage the robustness of the quad tree and just try to add onto all the worldChunks
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		this->_worldChunks[i].insert( tileConsolidated );
	}


	return;
}


void World::remove( const TileConsolidated& tileConsolidated )
{
	// We can leverage the robustness of the quad tree and just try to remove onto all the worldChunks
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		this->_worldChunks[i].remove( tileConsolidated );
	}

	return;
}


void World::saveWorldMap()
{
	MemoryManager::saveMap( "./worldMap.dat", this->_worldMap ); // [!] singleton
	return;
}


void World::loadWorldMap()
{
	MemoryManager::loadMap( "./worldMap.dat", this->_worldMap ); // [!] singleton
	return;
}


void World::viewWorldMap()
{
	MemoryManager::viewMap( this->_worldMap );
	return;
}


void World::updateWorldMap( int indexX, int indexY )
{
	this->_worldMap.emplace( std::tuple<int, int>{ indexX, indexY }, this->_worldMap.size() );
	return;
}


bool World::findWorldMap( int indexX, int indexY ) const
{
	return this->_worldMap.find( std::tuple<int, int>{ indexX, indexY } ) != this->_worldMap.end();
}


void World::saveWorldChunk( const WorldChunk& worldChunk )
{

	return;
}


void World::loadWorldChunk( WorldChunk& worldChunk )
{
	worldChunk.clear();
	return;
}


void World::delimitWorldChunk( WorldChunk& worldChunk, int chunkIndexX, int chunkIndexY )
{
	worldChunk.clear();
	worldChunk.delimit( chunkIndexX, chunkIndexY );
	return;
}


void World::initializeDelimits( const BoundingBox<float>& cameraView )
{
	// Based on the initial camera's position, delimit the surrounding worldChunks

	int cameraIndexX = cameraView.getCenterX() >= 0 ? ( int )( cameraView.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraView.getCenterY() >= 0 ? ( int )( cameraView.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );


	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			int deltaIndexX = x - this->_chunkRadius;
			int deltaIndexY = y - this->_chunkRadius;
			delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], cameraIndexX + deltaIndexX, cameraIndexY + deltaIndexY );
			updateWorldMap( this->_worldChunks[y * this->_numChunkWidth + x].getChunkIndexX(), this->_worldChunks[y * this->_numChunkWidth + x].getChunkIndexY() );
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

			if ( std::abs( cameraIndexX - worldChunkIndexX ) > this->_chunkRadius || std::abs( cameraIndexY - worldChunkIndexY ) > this->_chunkRadius )
			{
				int newChunkIndexX = cameraIndexX + -( worldChunkIndexX - this->_prevCameraIndexX );
				int newChunkIndexY = cameraIndexY + -( worldChunkIndexY - this->_prevCameraIndexY );

				delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );



				this->_worldChunks[y * this->_numChunkWidth + x].construct();




				updateWorldMap( worldChunkIndexX, worldChunkIndexY );
			}
		}
	}

	this->_prevCameraIndexX = cameraIndexX;
	this->_prevCameraIndexY = cameraIndexY;

	return;
}



void World::replaceWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY )
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