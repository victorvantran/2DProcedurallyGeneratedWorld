#pragma once

#include <cstdint>
#include <vector>
#include <set>

#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "DynamicObject.h"
// #include "World.h"

#include "BoundingBox.h"


class World;

class SpatialPartition
{
private:
	World* _world;
	std::vector<DynamicObject*> _mObjectsInArea[Settings::SpatialPartition::NUM_SPACE_ROWS * Settings::SpatialPartition::NUM_SPACE_COLS];
	//std::vector<olc::v2d_generic<long double>> _mOverlappingAreas;
	std::set<std::int64_t> _mOverlappingAreas;
	std::vector<std::int64_t> _index;

public:
	SpatialPartition()
		: _world( nullptr ),
		_mObjectsInArea{},
		_mOverlappingAreas(),
		_index()
	{

	}


	SpatialPartition( World* world )
		: _world( world ),
		_mObjectsInArea{},
		_mOverlappingAreas(),
		_index()
	{

	}


	~SpatialPartition()
	{
	}



	void updateSpaces( DynamicObject* object );


	void addObjectToSpace( std::int64_t spaceIndex, DynamicObject* object )
	{
		// Adds object to a particular paritioned space
		std::vector<DynamicObject*> space = this->_mObjectsInArea[spaceIndex];
		object->addToSpaces( spaceIndex, space.size() );
		space.push_back( object );
	}



	void removeObjectFromSpace( std::int64_t spaceIndex, std::size_t objectIndexInSpace, DynamicObject* object )
	{
		// Removes object to a particular paritioned space ( swapping with last object in vector )
		std::vector<DynamicObject*> space = this->_mObjectsInArea[spaceIndex];		
		// Base Case 1
		if ( space.size() == 0 )
		{
			return;
		}
		// Base Case 2
		else if ( space.size() == 1 )
		{
			std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = object->getSpaces();
			std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( tempSpaces.begin(), tempSpaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
			if ( spacesIt != tempSpaces.end() )
			{
				tempSpaces.erase( spacesIt );
				tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
				space.pop_back();
				return;
			}

			return;
		}
		

		// Swap
		DynamicObject* lastObject = space[space.size() - 1];
		space[space.size() - 1] = object;
		space[objectIndexInSpace] = lastObject;

		// Update the swapped object's areas
		/*
		std::set<std::int64_t>& tempSpaces = object->getSpaces();
		std::set<std::size_t>& tempIdsInSpaces = object->getIdsInSpaces();
		for ( std::size_t i = 0; i < tempSpaces.size(); i++ )
		{
			if ( tempSpaces[i] == spaceIndex )
			{
				tempIdsInSpaces[i] = objectIndexInSpace;
				break;
			}
		}
		*/

		std::set<std::pair<std::int64_t, std::size_t>>& tempSpaces = object->getSpaces();
		std::set<std::pair<std::int64_t, std::size_t>>::iterator spacesIt = std::find_if( tempSpaces.begin(), tempSpaces.end(), [&] ( const std::pair<std::int64_t, std::size_t>& p ) { return p.first == spaceIndex; } );
		if ( spacesIt != tempSpaces.end() )
		{
			tempSpaces.erase( spacesIt );
			tempSpaces.insert( std::pair<std::int64_t, std::size_t>{ spaceIndex, objectIndexInSpace } );
		}


		// Remove the swapped last object
		space.pop_back();
	}
};
