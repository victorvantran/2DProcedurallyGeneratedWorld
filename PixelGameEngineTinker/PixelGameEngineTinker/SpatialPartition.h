#pragma once

#include <cstdint>
#include <vector>
#include <set>

#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "DynamicObject.h"
#include "CollisionData.h"
// #include "World.h"

#include "BoundingBox.h"


class World;
class DynamicObject;

class SpatialPartition
{
private:
	World* _world;
	std::vector<DynamicObject*> _objectsInSpace[Settings::SpatialPartition::NUM_SPACE_ROWS * Settings::SpatialPartition::NUM_SPACE_COLS];
	std::set<std::int64_t> _overlappingSpaces;
	std::vector<std::int64_t> _index;

public:
	SpatialPartition()
		: _world( nullptr ),
		_objectsInSpace{},
		_overlappingSpaces(),
		_index()
	{

	}


	SpatialPartition( World* world )
		: _world( world ),
		_objectsInSpace{},
		_overlappingSpaces(),
		_index()
	{

	}


	~SpatialPartition()
	{
	}



	std::vector<DynamicObject*>* getObjectsInSpace()
	{
		return this->_objectsInSpace;
	}

	void updateSpaces( DynamicObject* object );


	void addObjectToSpace( std::int64_t spaceIndex, DynamicObject* object )
	{
		// Adds object to a particular paritioned space
		std::vector<DynamicObject*>& space = this->_objectsInSpace[spaceIndex];
		object->addToSpaces( spaceIndex, space.size() );
		space.push_back( object );
	}



	/*
	void removeObjectFromSpace( std::int64_t spaceIndex, std::size_t objectIndexInSpace, DynamicObject* object, std::set<std::pair<std::int64_t, std::size_t>>::iterator& spacesIter )
	{
		// Removes object to a particular paritioned space ( swapping with last object in vector )
		std::vector<DynamicObject*>& objectsInSpace = this->_objectsInSpace[spaceIndex];
		// Base Case 1
		if ( objectsInSpace.size() == 0 )
		{
			return;
		}

		// Base Case 2
		else if ( objectsInSpace.size() == 1 )
		{
			std::cout << "ONE GONE" << std::endl;
			std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = object->getSpaces();

			std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( tempSpaces.begin(), tempSpaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );

			if ( spacesIt == spacesIter && spacesIt != tempSpaces.end() )
			{
				spacesIter = tempSpaces.erase( spacesIter );

				// tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
				objectsInSpace.pop_back();
				std::cout << "Removed1" << std::endl;
				return;
			}

			else if ( spacesIt != spacesIter && spacesIt != tempSpaces.end() )
			{
				tempSpaces.erase( spacesIt );

				// tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
				objectsInSpace.pop_back();

				std::cout << "Removed2" << std::endl;
				return;
			}


			return;
		}

		else
		{
			std::cout << "TWO GONE" << std::endl;

			// Swap with last object to remove
			DynamicObject* lastObject = objectsInSpace[objectsInSpace.size() - 1];


			// if ( lastObject == object ) return;
			objectsInSpace[objectsInSpace.size() - 1] = object;
			objectsInSpace[objectIndexInSpace] = lastObject;

			// Update the swapped object's areas
			//std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = object->getSpaces();
			std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = lastObject->getSpaces();
			std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( tempSpaces.begin(), tempSpaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
			if ( spacesIt != tempSpaces.end() )
			{
				tempSpaces.erase( spacesIt );
				tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
			}

			// Remove the swapped last object
			objectsInSpace.pop_back();

			return;
		}

	}
	*/


	/*
	void removeObjectFromSpace( std::int64_t spaceIndex, std::size_t objectIndexInSpace, DynamicObject* object, std::set<std::pair<std::int64_t, std::size_t>>::iterator& spacesIter )
	{
		// Removes object to a particular paritioned space ( swapping with last object in vector )
		std::vector<DynamicObject*>& objectsInSpace = this->_objectsInSpace[spaceIndex];
		// Base Case 1
		if ( objectsInSpace.size() == 0 )
		{
			return;
		}

		std::cout << "TWO GONE" << std::endl;

		// Swap with last object to remove
		DynamicObject* lastObject = objectsInSpace[objectsInSpace.size() - 1];


		// if ( lastObject == object ) return;
		objectsInSpace[objectsInSpace.size() - 1] = object;
		objectsInSpace[objectIndexInSpace] = lastObject;

		// Update the swapped object's areas
		//std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = object->getSpaces();
		std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = lastObject->getSpaces();
		std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( tempSpaces.begin(), tempSpaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
		if ( spacesIt != tempSpaces.end() )
		{
			tempSpaces.erase( spacesIt );
			tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
		}

		// Remove the swapped last object
		objectsInSpace.pop_back();

		return;
	

	}
	*/


	void removeObjectFromSpace( std::int64_t spaceIndex, std::size_t objectIndexInSpace, DynamicObject* object )
	{
		// Removes object to a particular paritioned space ( swapping with last object in vector )
		std::vector<DynamicObject*>& objectsInSpace = this->_objectsInSpace[spaceIndex];
		// Base Case 1
		if ( objectsInSpace.size() == 0 ) return;
		

		// Swap with last object to remove
		DynamicObject* lastObject = objectsInSpace[objectsInSpace.size() - 1];
		objectsInSpace[objectsInSpace.size() - 1] = object;
		objectsInSpace[objectIndexInSpace] = lastObject;

		// Update the swapped object's areas
		std::vector<std::pair<std::int64_t, std::size_t>>& tempSpaces = lastObject->getSpaces();

		for ( std::size_t i = 0; i < tempSpaces.size(); i++ )
		{
			if ( tempSpaces[i].first == spaceIndex )
			{
				tempSpaces[i].second = objectIndexInSpace;
				break;
			}
		}

		// Remove the swapped last object
		objectsInSpace.pop_back();

		return;


	}

	void checkCollisions();
};
