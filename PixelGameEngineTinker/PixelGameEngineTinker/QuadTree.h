#pragma once

#include "BoundingBox.h"
#include "BoundingBoxConsolidated.h"
#include <vector>
#include <iterator>


//[!] to stop the drawing, need to make childrenNodeIndicies == nullptr when they are all gone (maybe trakc how many nodes in quad tree and when remove function, if end up zero, then delete and nullptr?)
// [!] problem; infinite recursion on insert and consolodate


template<typename T>
class QuadTree
{
public: // private
	const static int _MAX_OBJECTS = 4;
	const static int _MIN_LEVELS = 0; // (level => gridsize => pixelSize) 5 => 1024 =>64, 4 => 32 => 512, 3 => 16 => 256, 2 => 8 => 128, 1 => 4 => 64, 0 => 2 => 32
	const static int _MAX_LEVELS = 5; // [!] need to fix max levels stuff

	int _myIndex;
	int _parentIndex;
	int _level; // consolodationLevel 5, 4, 3, 2, 1, 0
	int _quadrant;

	BoundingBox<int> _quadTreeBounds;


	bool _divided;
	bool _consolidated; // if true, boundaries empty and = nullptr
	bool _shaded;

	BoundingBoxConsolidated<int>* _boundingBoxes; // since structs take contiguous memory, just store 4 bounding boxses instead of an array of 16 int
	int _boundingBoxCount;


	int* _childrenNodeIndicies;

	QuadTree<T>* _referenceNodes; // list of nodes to reference


	// bounding boxes can be used to locate the tile contents with a reference to layer mapping
	T* _map;

protected:

public:

	QuadTree();
	~QuadTree();

	QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> bounds, QuadTree<T>* referenceNodes, T* map );

	void constructQuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> bounds, QuadTree<T>* referenceNodes, T* map );

	void clear();
	void divide();
	int getQuadrant( const BoundingBoxConsolidated<int>& boundingBox );

	void consolidate( int level );
	void insert( BoundingBoxConsolidated<int> boundingBox );
	void remove( const BoundingBoxConsolidated<int>& rBoundingBox );
	std::vector<BoundingBoxConsolidated<int>> query( const BoundingBoxConsolidated<int>& boundingBox, std::vector<BoundingBoxConsolidated<int>>& returnObjects );


	int* getChildrenNodeIndicies();

	BoundingBox<int> getBounds();

	BoundingBoxConsolidated<int>* getBoundingBoxes();

	int getParentIndex();
	int getLevel();
	int getMyQuadrant();
	int getBoundingBoxCount();


	bool isConsolidated();


};


template<typename T>
QuadTree<T>::QuadTree()
{
	this->_myIndex = -1;
	this->_parentIndex = -1;
	this->_level = -1;
	this->_quadrant = -1;
	this->_quadTreeBounds;
	this->_divided = false;
	this->_consolidated = false;
	this->_shaded = false;

	this->_boundingBoxes = nullptr;

	this->_boundingBoxCount = 0;

	this->_childrenNodeIndicies = nullptr;

	this->_referenceNodes = nullptr;
	this->_map = nullptr;
}


template<typename T>
QuadTree<T>::QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> quadTreeBounds, QuadTree<T>* referenceNodes, T* map )
{
	constructQuadTree( myIndex, parentIndex, level, quadrant, quadTreeBounds, referenceNodes, map );
}

template<typename T>
QuadTree<T>::~QuadTree()
{
	std::cout << "destruct" << std::endl;

	this->clear();

	delete this->_childrenNodeIndicies;
	this->_childrenNodeIndicies = nullptr;

	delete[] this->_boundingBoxes;
	this->_boundingBoxes = nullptr;
}

template<typename T>
void QuadTree<T>::constructQuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> quadTreeBounds, QuadTree<T>* referenceNodes, T* map )
{
	this->_myIndex = myIndex;
	this->_parentIndex = parentIndex;
	this->_level = level;
	this->_quadrant = quadrant;
	this->_quadTreeBounds = quadTreeBounds;
	this->_divided = false;
	this->_consolidated = false;
	this->_shaded = false;

	this->_boundingBoxes = new BoundingBoxConsolidated<int>[4]; // ( topLeftPosX, topLeftPosY, width, height ) * 4
	// No need to iterate through because default constructor is called

	this->_boundingBoxCount = 0;

	this->_childrenNodeIndicies = nullptr;

	this->_referenceNodes = referenceNodes;
	this->_map = map;

	return;
}

template<typename T>
void QuadTree<T>::clear()
{
	std::cout << "clear" << std::endl;

	if ( this->_boundingBoxes != nullptr )
	{
		for ( int i = 0; i < 4; i++ )
		{
			this->_boundingBoxes[i].clear();
		}
	}


	if ( this->_childrenNodeIndicies != nullptr )
	{
		for ( int i = 0; i < 4; i++ )
		{
			this->_childrenNodeIndicies[i] = -1;
		}
	}

	return;
}

template<typename T>
void QuadTree<T>::divide()
{
	std::cout << "divide" << std::endl;

	// Splits the node into four subnodes each initialized with new bounds
	/*
	_________
	| 0 | 1 |
	|___|___|
	| 2 | 3 |
	|___|___|

	*/


	this->_childrenNodeIndicies = new int[4];

	// Calculate initial Shift
	int initialShift = 0;
	for ( int i = this->_level; i <= this->_MAX_LEVELS; i++ )
	{
		initialShift += ( 4 << ( ( this->_MAX_LEVELS - i ) * 2 ) ) / 4; // 4^(5 - parentLevel)
	}


	// Calculate recursive Shift
	int recursiveShift = 0;
	QuadTree<T>* ancestor = this;
	int j = 1;
	for ( int i = this->_level; i <= this->_MAX_LEVELS; i++ )
	{
		recursiveShift += ancestor->getMyQuadrant() * ( ( 4 << ( j * 2 ) ) / 4 );
		ancestor = &this->_referenceNodes[ancestor->getParentIndex()]; // index is the quadrant
		j++;
	}


	for ( int localIndex = 0; localIndex < 4; localIndex++ )
	{
		int finalIndex = initialShift + recursiveShift + localIndex;
		this->_childrenNodeIndicies[localIndex] = finalIndex; // index = 0, 1, 2, 3 which respectively identifies quadrants         
	}


	int x = this->_quadTreeBounds.x;
	int y = this->_quadTreeBounds.y;
	int subWidth = this->_quadTreeBounds.width / 2;
	int subHeight = this->_quadTreeBounds.height / 2;

	this->_referenceNodes[this->_childrenNodeIndicies[0]].constructQuadTree( this->_childrenNodeIndicies[0], this->_myIndex, this->_level - 1, 0, BoundingBox<int>( x, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenNodeIndicies[1]].constructQuadTree( this->_childrenNodeIndicies[1], this->_myIndex, this->_level - 1, 1, BoundingBox<int>( x + subWidth, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenNodeIndicies[2]].constructQuadTree( this->_childrenNodeIndicies[2], this->_myIndex, this->_level - 1, 2, BoundingBox<int>( x, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenNodeIndicies[3]].constructQuadTree( this->_childrenNodeIndicies[3], this->_myIndex, this->_level - 1, 3, BoundingBox<int>( x + subWidth, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );

	this->_divided = true;

	return;
}


template<typename T>
int QuadTree<T>::getQuadrant( const BoundingBoxConsolidated<int>& boundingBox )
{
	//std::cout << "getQuadrant" << std::endl;
	// Given a boundingBox, returns the index of the nodes ( which quadrant the box belongs to )
	float verticleMidpoint = this->_quadTreeBounds.x + ( this->_quadTreeBounds.width / 2 );
	float horizontalMidpoint = this->_quadTreeBounds.y + ( this->_quadTreeBounds.height / 2 );

	bool topQuadrant = ( ( boundingBox.y < horizontalMidpoint ) && ( boundingBox.y + boundingBox.height <= horizontalMidpoint ) );
	bool bottomQuadrant = ( boundingBox.y >= horizontalMidpoint );


	// Object completely fits in left quadrant
	if ( ( boundingBox.x < verticleMidpoint ) && ( boundingBox.x + boundingBox.width <= verticleMidpoint ) )
	{
		if ( topQuadrant )
		{
			return 0;
		}
		else if ( bottomQuadrant )
		{
			return 2;
		}
	}

	// Object completely fits in right quadrants
	if ( boundingBox.x >= verticleMidpoint )
	{
		if ( topQuadrant )
		{
			return 1;
		}
		else if ( bottomQuadrant )
		{
			return 3;
		}
	}
}

template<typename T>
void QuadTree<T>::consolidate( int level )
{
	std::cout << "X" << std::endl;
	std::cout << "consolidate" << std::endl;
	std::cout << "Y" << std::endl;

	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	if ( (
		( _boundingBoxes[0].id == _boundingBoxes[1].id ) &&
		( _boundingBoxes[2].id == _boundingBoxes[3].id ) &&
		( _boundingBoxes[0].id == _boundingBoxes[3].id ) )
		&&
		(
			( level == 0 ) ||
			(
				( _boundingBoxes[0].width == _boundingBoxes[1].width ) &&
				( _boundingBoxes[2].width == _boundingBoxes[3].width ) &&
				( _boundingBoxes[0].width == _boundingBoxes[3].width ) &&
				( _boundingBoxes[0].height == _boundingBoxes[1].height ) &&
				( _boundingBoxes[2].height == _boundingBoxes[3].height ) &&
				( _boundingBoxes[0].height == _boundingBoxes[3].height )
				)
			)
		&&
		(
			_boundingBoxes[0].width == ( ( 2 << ( this->_level ) ) / 2 )
			)
		)

		// height check not necessary if our bounding box is a square, but for formality
	{
		if ( this->_level < this->_MAX_LEVELS )
		{
			int posX = std::min<int>( std::min<int>( _boundingBoxes[0].x, _boundingBoxes[1].x ), std::min<int>( _boundingBoxes[2].x, _boundingBoxes[3].x ) ); // technically only need to check for 3 of them
			int posY = std::min<int>( std::min<int>( _boundingBoxes[0].y, _boundingBoxes[1].y ), std::min<int>( _boundingBoxes[2].y, _boundingBoxes[3].y ) );
			int width = _boundingBoxes[0].width * 2;
			int height = _boundingBoxes[0].height * 2;
			int id = _boundingBoxes[0].id;

			// [!] when we add encapsulate box, store the four sub bounding box
			// [!] when we remove encapsulate, destroy this considiated bounding box, recursively find the bounding box to be destoryed, and add the other 3 instead
			BoundingBoxConsolidated<int> encapsulateBox = BoundingBoxConsolidated<int>( posX, posY, width, height, id );
			this->_consolidated = true;

			this->_referenceNodes[this->_parentIndex].insert( encapsulateBox ); // make the encapsulate box hold pointer to other encapsulate box (so when removed, can re-add)
		}
		else
		{
			return; // no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
		}
	}
	else
	{
		if ( this->_level == this->_MIN_LEVELS )
		{
			return; // no need to divide anymore
		}
		else
		{
			if ( this->_childrenNodeIndicies == nullptr )
			{
				this->divide();
			}

			//this->_boundingBoxCount = 0; // if we add stuff, it would overwrite our childrenNodeIndicies history, but that's okay because our encapsulated bounding box will hold history
			//this->_boundingBoxCount = 0;



			for ( int i = 0; i < 4; i++ )
			{
				int index = this->getQuadrant( this->_boundingBoxes[i] );

				BoundingBoxConsolidated<int> cachedBox = this->_boundingBoxes[i];
				this->_boundingBoxes[i].clear();
				this->_boundingBoxCount -= 1;
				//this->_referenceNodes[this->_childrenNodeIndicies[index]].insert( this->_boundingBoxes[i] );
				this->_referenceNodes[this->_childrenNodeIndicies[index]].insert( cachedBox );
			}

			// [!] if added (do not clear) (because could have added back and if you cleared it, then you're screwed); that's also why we don' reset the boundingbox count to 0 afterwards
			// keep a record; if the same, then remove it and decrement

			/*
			for ( int i = 0; i < 4; i++ )
			{
				this->_boundingBoxes[i].clear();
			}
			*/
			//this->_boundingBoxCount = 0;
		}

	}

	return;
}


template<typename T>
void QuadTree<T>::insert( BoundingBoxConsolidated<int> boundingBox )
{
	std::cout << "insert" << std::endl;

	// Determine if the node has any children nodes conceived yet, and if the object can be added into one of them
	// If there are not yet any concieved child nodes or the object cannot entirely fit into a child node, store it in the parent node
	// After adding the object, determine if splitting needs to occur if the current number of object exceeds the maximum amount of objects
	// Splitting will shift the objects of the parent node to the proper children node (or stay in the parent node if it cannot entirely fit in any children node)

	// If child 0 exists, implies that all children nodes exist/conceived
	// If children nodes exist, and the bounds belong in one of them given by the index, then insert in the proper node



	if ( !( this->_quadTreeBounds >= boundingBox ) )
	{
		return;
	}


	if ( this->_childrenNodeIndicies != nullptr )
	{
		int index = this->getQuadrant( boundingBox );
		if ( this->_referenceNodes[this->_childrenNodeIndicies[index]].isConsolidated() == false )
		{
			this->_referenceNodes[this->_childrenNodeIndicies[index]].insert( boundingBox );
			return;
		}

	}


	// Find the first free bounding box (it could be in any order because of remove method removing any)
	for ( int i = 0; i < 4; i++ )
	{
		if ( this->_boundingBoxes[i].x == -1 ) // -1 indicates cleared
		{
			this->_boundingBoxes[i] = boundingBox;
			this->_boundingBoxCount += 1;
			break;
		}
	}

	// this->_boundingBoxCount += 1;


	if ( this->_boundingBoxCount > 4 ) // [!] need to fix
	{
		std::cout << "_________________________________________________________" << std::endl;
		//this->_boundingBoxCount = 0; // hacky non-crash, but i think will render fail
		//this->_boundingBoxCount = 4;
	}

	if ( this->_boundingBoxCount >= this->_MAX_OBJECTS && this->_level >= this->_MIN_LEVELS )
	{
		this->consolidate( this->_level );
	}

	return;
}


template<typename T>
void QuadTree<T>::remove( const BoundingBoxConsolidated<int>& boundingBox )
{
	//std::cout << "remove" << std::endl;

	// maybe it exists  without it being restricted to trees of width/2
	for ( int i = 0; i < 4; i++ )
	{
		if ( this->_boundingBoxes[i] == boundingBox && this->_boundingBoxes[i].id == boundingBox.id )
		{
			this->_boundingBoxes[i].clear();
			this->_boundingBoxCount -= 1;
			this->_consolidated = false;
			std::cout << "erased from higher level" << std::endl;

			return;
		}
	}



	if ( boundingBox.width == ( this->_quadTreeBounds.width / 2 ) ) // [!] size instead of width/height [!] can get rid of this condition
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_boundingBoxes[i] == boundingBox && this->_boundingBoxes[i].id == boundingBox.id )
			{
				this->_boundingBoxes[i].clear();
				this->_boundingBoxCount -= 1;
				this->_consolidated = false;
				std::cout << "erased from level 0" << std::endl;
				return;
			}
		}
	}
	else
	{
		int i = this->getQuadrant( boundingBox );
		if ( this->_referenceNodes != nullptr && this->_childrenNodeIndicies != nullptr && this->_referenceNodes[this->_childrenNodeIndicies[i]].getBounds() >= boundingBox )
		{
			// clear the bounding box if found one (which implies it's consolidated?)
			for ( int j = 0; j < 4; j++ )
			{
				if ( this->_boundingBoxes[j] >= boundingBox && this->_boundingBoxes[j].id == boundingBox.id )
				{
					this->_boundingBoxes[j].clear();
					this->_boundingBoxCount -= 1;
					this->_consolidated = false;
				}
			}
			this->_consolidated = false; // consolidate = false anyway

			// regardless if found one or not, keep checking the actual tree because the bounding box may have been overwritten
			this->_referenceNodes[this->_childrenNodeIndicies[i]].remove( boundingBox );
			return;
		}
		else
		{
			// Trying to remove out of bounds
			return;
		}

	}
	// Trying to remove something that is non existent
	return;
}


template<typename T>
std::vector<BoundingBoxConsolidated<int>> QuadTree<T>::query( const BoundingBoxConsolidated<int>& boundingBox, std::vector<BoundingBoxConsolidated<int>>& returnBoxes )
{
	// Returns all objects that could possibly collide with the given object (as well as a copy of the passed object itself (the copy resides in the _objects) )
	// Work around of the copies is to use pointers
	int index = getQuadrant( boundingBox );
	if ( this->_childrenNodeIndicies != nullptr )
	{
		this->_referenceNodes[this->_childrenNodeIndicies[index]].query( boundingBox, returnBoxes );
	}

	// concatenate/build up the list

	return returnBoxes;
}


template<typename T>
int* QuadTree<T>::getChildrenNodeIndicies()
{
	return this->_childrenNodeIndicies;
}

template<typename T>
BoundingBox<int> QuadTree<T>::getBounds()
{
	return this->_quadTreeBounds;
}



template<typename T>
BoundingBoxConsolidated<int>* QuadTree<T>::getBoundingBoxes()
{
	return this->_boundingBoxes;
}



template<typename T>
int QuadTree<T>::getParentIndex()
{
	return this->_parentIndex;
}


template<typename T>
int QuadTree<T>::getLevel()
{
	return this->_level;
}

template<typename T>
int QuadTree<T>::getMyQuadrant()
{
	return this->_quadrant;
}

template<typename T>
int QuadTree<T>::getBoundingBoxCount()
{
	return this->_boundingBoxCount;
}

template<typename T>
bool QuadTree<T>::isConsolidated()
{
	return this->_consolidated;
}

