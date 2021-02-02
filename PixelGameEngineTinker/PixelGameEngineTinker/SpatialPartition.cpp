#include "SpatialPartition.h"
#include "World.h"
// #include "DynamicObject.h"

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

	olc::v2d_generic<std::int64_t> topLeft = olc::v2d_generic<std::int64_t>{
		( std::int64_t )( ( objectCenter.x - objectHalfSize.x ) / Settings::SpatialPartition::WIDTH ),
		( std::int64_t )( ( objectCenter.y - objectHalfSize.y ) / Settings::SpatialPartition::HEIGHT )
	}; 
	
	olc::v2d_generic<std::int64_t> topRight = olc::v2d_generic<std::int64_t>{
		( std::int64_t )( ( objectCenter.x + objectHalfSize.x ) / Settings::SpatialPartition::WIDTH ),
		( std::int64_t )( ( objectCenter.y - objectHalfSize.y ) / Settings::SpatialPartition::HEIGHT )
	};
	
	olc::v2d_generic<std::int64_t> bottomLeft = olc::v2d_generic<std::int64_t>{
	( std::int64_t )( ( objectCenter.x - objectHalfSize.x ) / Settings::SpatialPartition::WIDTH ),
	( std::int64_t )( ( objectCenter.y + objectHalfSize.y ) / Settings::SpatialPartition::HEIGHT )
	};

	olc::v2d_generic<std::int64_t> bottomRight = olc::v2d_generic<std::int64_t>{
		topRight.x,
		bottomLeft.y
	};

	// std::cout << topLeft.x << std::endl;
	if ( topLeft.x == topRight.x && topLeft.y == bottomLeft.y )
	{
		this->_overlappingSpaces.insert( topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + topLeft.x );
	}
	else if ( topLeft.x == topRight.x )
	{
		this->_overlappingSpaces.insert( topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + topLeft.x );
		this->_overlappingSpaces.insert( bottomLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + bottomLeft.x );
	}
	else if ( topLeft.y == bottomLeft.y )
	{
		this->_overlappingSpaces.insert( topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + topLeft.x );
		this->_overlappingSpaces.insert( topRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + topRight.x );
	}
	else
	{
		this->_overlappingSpaces.insert( topLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + topLeft.x );
		this->_overlappingSpaces.insert( bottomLeft.y * Settings::SpatialPartition::NUM_SPACE_COLS + bottomLeft.x );
		this->_overlappingSpaces.insert( topRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + topRight.x );
		this->_overlappingSpaces.insert( bottomRight.y * Settings::SpatialPartition::NUM_SPACE_COLS + bottomRight.x );
	}

	std::set<std::pair<std::int64_t, std::size_t>>& spaces = object->getSpaces();
	std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIter = spaces.begin();
	while ( spacesIter != spaces.end() )
	{
		if ( this->_overlappingSpaces.find( ( *spacesIter ).first ) == this->_overlappingSpaces.end() )
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
	for ( std::int64_t spaceIndex : this->_overlappingSpaces )
	{

		std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( spaces.begin(), spaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
		if ( spacesIt == spaces.end() )
		{
			this->addObjectToSpace( spaceIndex, object );
		}
	}

	this->_overlappingSpaces.clear();

	return;
}



void SpatialPartition::checkCollisions()
{
	olc::v2d_generic<long double> overlap;

	for ( std::size_t row = 0; row < Settings::SpatialPartition::NUM_SPACE_ROWS; row++ )
	{
		for ( std::size_t col = 0; col < Settings::SpatialPartition::NUM_SPACE_COLS; col++ )
		{
			// std::cout << "1: " << row * Settings::SpatialPartition::NUM_SPACE_COLS + col << std::endl;
			std::vector<DynamicObject*>& objectsInSpace = this->getObjectsInSpace()[row * Settings::SpatialPartition::NUM_SPACE_COLS + col];
			// std::cout << "2: " << objectsInSpace.size() << std::endl;
			for ( std::int64_t i = 0; i < ( std::int64_t )objectsInSpace.size() - 1; i++ )
			{
				// std::cout << "3: " << i << ", " << objectsInSpace.size() - 1 << std::endl;
				DynamicObject* object1 = objectsInSpace[i];
				for ( std::size_t j = i + 1; j < objectsInSpace.size(); j++ )
				{
					DynamicObject* object2 = objectsInSpace[j];
					// Detect for overlap and if collision data for created yet
					// [!] Optimization is to use a dictionary instead of a vector for DynamicObject._allCollisions in order to search in O(1) time
					if ( object1->getAABB().overlapsSigned( object2->getAABB(), overlap ) && !object1->hasCollisionDataFor( object2 ) )
					{
						// If overlap is detected, need to add collision data to both objects
						object1->getAllCollisions().push_back( CollisionData(
							object2,
							overlap,
							object1->getCurrVelocity(), object2->getCurrVelocity(),
							object1->getPrevPosition(), object2->getPrevPosition(),
							object1->getCurrPosition(), object2->getCurrPosition() )
						);

						object2->getAllCollisions().push_back( CollisionData(
							object1,
							-overlap,
							object2->getCurrVelocity(), object1->getCurrVelocity(),
							object2->getPrevPosition(), object1->getPrevPosition(),
							object2->getCurrPosition(), object1->getCurrPosition() )
						);
					}
				}
			}
		}
	}

	return;
}