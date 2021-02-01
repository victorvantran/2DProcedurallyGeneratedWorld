#include "SpatialPartition.h"
#include "World.h"


void SpatialPartition::updateSpaces( DynamicObject* object )
{
	// [!] needs to be on thread that updates worldchunk, focal chunk, etc.
	// Assume object cannot be bigger than partition

	WorldChunk* topLeftWorldChunk = this->_world->getRelativeWorldChunks( 0 );

	olc::v2d_generic<long double> offset = olc::v2d_generic<long double>{ ( long double )topLeftWorldChunk->getChunkIndexX() * Settings::WorldChunk::SIZE, ( long double )topLeftWorldChunk->getChunkIndexY() * Settings::WorldChunk::SIZE };


	olc::v2d_generic<long double> objectCenter = olc::v2d_generic<long double>{ ( long double )object->getAABB().getCenter().x, ( long double )object->getAABB().getCenter().y } -offset;
	olc::v2d_generic<long double> objectHalfSize = olc::v2d_generic<long double>{ ( long double )object->getAABB().getHalfSize().x, ( long double )object->getAABB().getHalfSize().y };

	// Check for objects out of world bounds and ignore
	BoundingBox<long double> worldBounds( 0, 0, Settings::SpatialPartition::NUM_SPACE_COLS * Settings::SpatialPartition::WIDTH, Settings::SpatialPartition::NUM_SPACE_ROWS * Settings::SpatialPartition::HEIGHT );
	if ( !worldBounds.encapsulates( BoundingBox<long double>{ objectCenter.x - objectHalfSize.x, objectCenter.y - objectHalfSize.y, objectHalfSize.x * 2, objectHalfSize.y * 2 } ) )
	{
		return;
	}


	olc::v2d_generic<long double> topLeft = objectCenter + olc::v2d_generic<long double>{ -objectHalfSize.x, -objectHalfSize.y };
	olc::v2d_generic<long double> topRight = objectCenter + olc::v2d_generic<long double>{ objectHalfSize.x, -objectHalfSize.y };
	olc::v2d_generic<long double> bottomLeft = objectCenter + olc::v2d_generic<long double>{ -objectHalfSize.x, objectHalfSize.y };
	olc::v2d_generic<long double> bottomRight = objectCenter + olc::v2d_generic<long double>{ -objectHalfSize.x, objectHalfSize.y };

	topLeft.x /= Settings::SpatialPartition::WIDTH;
	topLeft.y /= Settings::SpatialPartition::HEIGHT;
	topRight.x /= Settings::SpatialPartition::WIDTH;
	topRight.y /= Settings::SpatialPartition::HEIGHT;
	bottomLeft.x /= Settings::SpatialPartition::WIDTH;
	bottomLeft.y /= Settings::SpatialPartition::HEIGHT;
	bottomRight.x = topRight.x;
	bottomRight.y = bottomLeft.y;


	// std::cout << topLeft.x << std::endl;
	if ( ( std::int64_t )topLeft.x == ( std::int64_t )topRight.x && ( std::int64_t )topLeft.y == ( std::int64_t )bottomLeft.y )
	{
		this->_mOverlappingAreas.insert( ( std::int64_t )topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topLeft.x );
	}
	else if ( ( std::int64_t )topLeft.x == ( std::int64_t )topRight.x )
	{
		this->_mOverlappingAreas.insert( ( std::int64_t )topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topLeft.x );
		this->_mOverlappingAreas.insert( ( std::int64_t )bottomLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )bottomLeft.x );
	}
	else if ( ( std::int64_t )topLeft.y == ( std::int64_t )bottomLeft.y )
	{
		this->_mOverlappingAreas.insert( ( std::int64_t )topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topLeft.x );
		this->_mOverlappingAreas.insert( ( std::int64_t )topRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topRight.x );
	}
	else
	{
		this->_mOverlappingAreas.insert( ( std::int64_t )topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topLeft.x );
		this->_mOverlappingAreas.insert( ( std::int64_t )bottomLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )bottomLeft.x );
		this->_mOverlappingAreas.insert( ( std::int64_t )topRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )topRight.x );
		this->_mOverlappingAreas.insert( ( std::int64_t )bottomRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + ( std::int64_t )bottomRight.x );
	}



	std::set<std::pair<std::int64_t, std::size_t>>& spaces = object->getSpaces();
	std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIter = spaces.begin();
	while ( spacesIter != spaces.end() )
	{
		if ( this->_mOverlappingAreas.find( ( *spacesIter ).first ) == this->_mOverlappingAreas.end() )
		{
			this->removeObjectFromSpace( ( *spacesIter ).first, ( *spacesIter ).second, object );
			spacesIter = spaces.erase( spacesIter );
		}
		else
		{
			spacesIter++;
		}
	}


	// Loop through new areas to add any potentional overlapping spaces
	for ( std::int64_t spaceIndex : this->_mOverlappingAreas )
	{

		std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( spaces.begin(), spaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
		if ( spacesIt == spaces.end() )
		{
			std::cout << spaceIndex << std::endl;
			this->addObjectToSpace( spaceIndex, object );
		}
	}

	this->_mOverlappingAreas.clear();

	return;
}