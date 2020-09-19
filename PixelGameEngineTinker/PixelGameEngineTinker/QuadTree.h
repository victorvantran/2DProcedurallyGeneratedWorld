#pragma once

#include "BoundingBox.h"
#include "BoundingBoxConsolidated.h"
#include <vector>
#include <iterator>


template<typename T>
class QuadTree
{
public: // private
	const static int _MAX_OBJECTS = 4;
	const static int _MIN_LEVELS = 0; // (level => gridsize => pixelSize) 5 => 1024 =>64, 4 => 32 => 512, 3 => 16 => 256, 2 => 8 => 128, 1 => 4 => 64, 0 => 2 => 32
	const static int _MAX_LEVELS = 5; // [!] need to fix max levels

	int _myIndex;
	int _parentIndex;
	int _level;
	int _quadrant;

	BoundingBox<int> _quadTreeBounds;


	bool _divided;
	bool _consolidated;
	bool _shaded;

	BoundingBoxConsolidated<int>* _boundingBoxes;
	int _boundingBoxCount;


	int* _childrenNodeIndicies;

	QuadTree<T>* _referenceNodes; // list of nodes to reference

	// Bounding boxes can be used to locate the tile contents with a reference to layer mapping
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

	bool isCellOccupied( const BoundingBoxConsolidated<int>& BoundingBox );

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

	// Bounding Box not completely within a quadrant; used to indicate splitting when removing an area
	return -1;
}





template<typename T>
void QuadTree<T>::consolidate( int level )
{
	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	// Height check not necessary if our bounding box is a square, but for formality
	if (
		(
			( _boundingBoxes[0].id == _boundingBoxes[1].id ) &&
			( _boundingBoxes[2].id == _boundingBoxes[3].id ) &&
			( _boundingBoxes[0].id == _boundingBoxes[3].id )
			)
		&&
		(
			( level == this->_MIN_LEVELS ) ||
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
	{
		// Since level is lowest level, there is no children to check if all of them are consolidated
		if ( this->_level == this->_MIN_LEVELS )
		{
			int posX = std::min<int>( std::min<int>( _boundingBoxes[0].x, _boundingBoxes[1].x ), std::min<int>( _boundingBoxes[2].x, _boundingBoxes[3].x ) ); // technically only need to check for 3 of them
			int posY = std::min<int>( std::min<int>( _boundingBoxes[0].y, _boundingBoxes[1].y ), std::min<int>( _boundingBoxes[2].y, _boundingBoxes[3].y ) );
			int width = _boundingBoxes[0].width * 2;
			int height = _boundingBoxes[0].height * 2;
			int id = _boundingBoxes[0].id;


			BoundingBoxConsolidated<int> encapsulateBox = BoundingBoxConsolidated<int>( posX, posY, width, height, id );
			this->_consolidated = true;
			this->_referenceNodes[this->_parentIndex].insert( encapsulateBox );
		}
		// Since level is not lowest level, there is children to necessarilly check if all of them are consolidated
		else if ( this->_level < this->_MAX_LEVELS )
		{
			for ( int i = 0; i < 4; i++ )
			{
				if ( !this->_referenceNodes[this->_childrenNodeIndicies[i]].isConsolidated() )
				{
					return;
				}
			}

			int posX = std::min<int>( std::min<int>( _boundingBoxes[0].x, _boundingBoxes[1].x ), std::min<int>( _boundingBoxes[2].x, _boundingBoxes[3].x ) ); // technically only need to check for 3 of them
			int posY = std::min<int>( std::min<int>( _boundingBoxes[0].y, _boundingBoxes[1].y ), std::min<int>( _boundingBoxes[2].y, _boundingBoxes[3].y ) );
			int width = _boundingBoxes[0].width * 2;
			int height = _boundingBoxes[0].height * 2;
			int id = _boundingBoxes[0].id;


			BoundingBoxConsolidated<int> encapsulateBox = BoundingBoxConsolidated<int>( posX, posY, width, height, id );
			this->_consolidated = true;
			this->_referenceNodes[this->_parentIndex].insert( encapsulateBox );
		}
		// Else no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
	}

	return;
}




template<typename T>
void QuadTree<T>::insert( BoundingBoxConsolidated<int> boundingBox )
{
	// Recursively adds bounding boxes and consolidates



	// Cannot add what is out of boundss
	if ( !boundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}



	// If it is already consolidated, the bounding boxes is already included
	if ( this->_consolidated || this->_boundingBoxCount == 4 ) // [!]
	{
		return;
	}


	// Check if a bounding box is already occupied
	for ( int i = 0; i < 4; i++ )
	{
		if ( ( this->_boundingBoxes[i] == boundingBox ) )
		{
			return;
		}
	}


	// If any dimension size ( x or y ) < 1, return
	if ( boundingBox.width < 1 || boundingBox.height < 1 )
	{
		return;
	}


	// If the boundingBox has any area that is outside the boundaries of quadTreeBounds, trim
	if ( !( this->_quadTreeBounds >= boundingBox ) )
	{
		int trimX;
		int trimY;
		int trimWidth;
		int trimHeight;

		trimX = std::max<int>( this->_quadTreeBounds.x, boundingBox.x );
		trimY = std::max<int>( this->_quadTreeBounds.y, boundingBox.y );

		trimWidth = std::min<int>( this->_quadTreeBounds.x + this->_quadTreeBounds.width, boundingBox.x + boundingBox.width ) - trimX;
		trimHeight = std::min<int>( this->_quadTreeBounds.y + this->_quadTreeBounds.height, boundingBox.y + boundingBox.height ) - trimY;


		if ( trimWidth == 0 || trimHeight == 0 )
		{
			return;
		}

		const BoundingBoxConsolidated<int> trimmedBox = BoundingBoxConsolidated<int>( trimX, trimY, trimWidth, trimHeight, boundingBox.id );
		this->insert( trimmedBox );
		return;
	}



	int quadrant = this->getQuadrant( boundingBox );
	if ( quadrant == -1 )
	{
		int x = boundingBox.x;
		int y = boundingBox.y;
		int id = boundingBox.id;

		int subWidth1 = boundingBox.width / 2;
		int subHeight1 = boundingBox.height / 2;

		int subWidth2 = boundingBox.width - subWidth1;
		int subHeight2 = boundingBox.height - subHeight1;

		const BoundingBoxConsolidated<int> rSubBoundingBox0 = BoundingBoxConsolidated<int>( x, y, subWidth1, subHeight1, id );
		const BoundingBoxConsolidated<int> rSubBoundingBox1 = BoundingBoxConsolidated<int>( x + subWidth1, y, subWidth2, subHeight1, id );
		const BoundingBoxConsolidated<int> rSubBoundingBox2 = BoundingBoxConsolidated<int>( x, y + subHeight1, subWidth1, subHeight2, id );
		const BoundingBoxConsolidated<int> rSubBoundingBox3 = BoundingBoxConsolidated<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2, id );

		this->insert( rSubBoundingBox0 );
		this->insert( rSubBoundingBox1 );
		this->insert( rSubBoundingBox2 );
		this->insert( rSubBoundingBox3 );

		return;
	}

	// Else
	// Our aBoundingBox is gauranteed to be encapsulated by the quadTreeBounds at this point; And it is gauranteed to not occupy a bounding box yet
	// More than encapsulated, since we trimmed and split, it must be or not be the same as only one children bound ( we can break early )
	// Create the bounding box if any children quadrant bounds is equal to the aBoundingBox; this means we can index our bounding box by the quadrant!
	// [!] can update to make _boundingBoxes *{ data strcuture with id, configuration, exist }


	// Level 0: Simply fill in the bounding boxes and end the recursion
	if ( this->_level == 0 )
	{
		// Existing check already done
		// Find the index by using the bounds, and the position of the single-cell bounding box

		if ( this->_boundingBoxes[0].id == -1 && ( boundingBox.x == this->_quadTreeBounds.x ) && ( boundingBox.y == this->_quadTreeBounds.y ) )
		{
			this->_boundingBoxes[0] = boundingBox;
			this->_boundingBoxCount += 1;
		}
		else if ( this->_boundingBoxes[1].id == -1 && ( boundingBox.x == ( this->_quadTreeBounds.x + 1 ) ) && ( boundingBox.y == this->_quadTreeBounds.y ) )
		{
			this->_boundingBoxes[1] = boundingBox;
			this->_boundingBoxCount += 1;
		}
		else if ( this->_boundingBoxes[2].id == -1 && ( boundingBox.x == this->_quadTreeBounds.x ) && ( boundingBox.y == ( this->_quadTreeBounds.y + 1 ) ) )
		{
			this->_boundingBoxes[2] = boundingBox;
			this->_boundingBoxCount += 1;
		}
		else if ( this->_boundingBoxes[3].id == -1 )
		{
			this->_boundingBoxes[3] = boundingBox;
			this->_boundingBoxCount += 1;
		}
	}
	// Level > 0: Fill the bounding boxes and recur to children nodes
	else
	{
		if ( this->_childrenNodeIndicies == nullptr )
		{
			this->divide();
		}


		int quadrant = this->getQuadrant( boundingBox ); // Gauranteed not to equal negative one based on the previous conditions we checked for
		if ( this->_referenceNodes[this->_childrenNodeIndicies[quadrant]].getBounds() == boundingBox )
		{
			this->_boundingBoxes[quadrant] = boundingBox;
			this->_boundingBoxCount += 1;
		}

		// Add to child regardless
		this->_referenceNodes[this->_childrenNodeIndicies[quadrant]].insert( boundingBox );
	}


	// Consolidate if met the requirements of max objects and level
	if ( this->_boundingBoxCount >= this->_MAX_OBJECTS && this->_level >= this->_MIN_LEVELS )
	{
		this->consolidate( this->_level );
	}

	return;
}





template<typename T>
void QuadTree<T>::remove( const BoundingBoxConsolidated<int>& boundingBox )
{

	// If COMPLETELY(not partially out of obunds) out of bounds; return
	if ( !boundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}


	// If the rBoundingBlock is indeed within the bounds, consolidation must equal false because we are removing within (whether it was consolidated to begin with or not)
	this->_consolidated = false;

	// Base Case: Check to see if a high level tree has the single cell
	if ( boundingBox.width == 1 && boundingBox.height == 1 )
	{
		for ( int i = 0; i < 4; i++ )
		{
			// Check to see if its within a bounding box
			if ( boundingBox.intersects( this->_boundingBoxes[i] ) && this->_boundingBoxes[i].id == boundingBox.id )
			{
				this->_boundingBoxes[i].clear();
				this->_boundingBoxCount -= 1;

				// Can break because 1x1 cell cannot reside in multiple bounding boxes
				// Deltes a nxn >= 1x1 bounding box but not neccessarily the 1x1 bounding box, so can't return yet
				break;
			}
		}


		// If not check the children
		if ( this->_childrenNodeIndicies != nullptr )
		{
			// Since the cell is one big, there is no chance that it will reside within multiple boundingboxes; So no need tosplit, nor check
			int quadrant = this->getQuadrant( boundingBox );
			if ( quadrant != -1 )
			{
				// If a quadrant is found, then the cell is definitely within the bounds, so consolidation will be false as there would be a "hole"
				this->_referenceNodes[this->_childrenNodeIndicies[quadrant]].remove( boundingBox );
			}
		}
		// Else the children Nodes don't exist, so the cell one is trying to remove does not exist; return.
	}
	// Recursive Case: The cell is bigger than 1x1; therefore, we need to remove any bounding boxes found in children nodes that happen to encapsulate any part of that cell (due to consolidation)
	else
	{
		// Try to find any bounding box to delete that touches the rBoundingBox (there may be multiple, so don't break the for loop preemptively)
		for ( int i = 0; i < 4; i++ )
		{
			if ( boundingBox.intersects( this->_boundingBoxes[i] ) ) // [!] Doesn't matter the id of the cell, just remove regardless
			{
				this->_boundingBoxes[i].clear();
				this->_boundingBoxCount -= 1;
				// Cannot necessarilly return here because there may be smaller bounding boxes to remove and/or need to fragmentize the rBoundingBox
			}
		}

		// Remember that it may not find a big/exact size bounding box to delete; still need to search deeper and fragmentize for potential small ones to remove
		// Check to see if there are any child nodes to look deeper in
		if ( this->_childrenNodeIndicies != nullptr )
		{
			int quadrant = this->getQuadrant( boundingBox );
			// If the box does not fully reside in one quadrant, we need to split the box; there is probably calculations one can do to optimize the split, but for now, just split it in half both ways
			if ( quadrant == -1 )
			{
				// Divide bounding boxes
				int x = boundingBox.x;
				int y = boundingBox.y;
				int id = boundingBox.id;


				// Deal with splitting too much
				// If width equals 1, split between the top left and bottom left (as the right does not exist)
				if ( boundingBox.width == 1 )
				{

					int subWidth1 = 1;
					int subHeight1 = boundingBox.height / 2;
					int subHeight2 = boundingBox.height - subHeight1;

					const BoundingBoxConsolidated<int> rSubBoundingBox0 = BoundingBoxConsolidated<int>( x, y, subWidth1, subHeight1, id );
					const BoundingBoxConsolidated<int> rSubBoundingBox2 = BoundingBoxConsolidated<int>( x, y + subHeight1, subWidth1, subHeight2, id );

					this->remove( rSubBoundingBox0 );
					this->remove( rSubBoundingBox2 );
				}
				// If height equals 1, split between the top left and top right (as the bottom does not exist)
				else if ( boundingBox.height == 1 )
				{

					int subHeight1 = 1;
					int subWidth1 = boundingBox.width / 2;
					int subWidth2 = boundingBox.width - subWidth1;

					const BoundingBoxConsolidated<int> rSubBoundingBox0 = BoundingBoxConsolidated<int>( x, y, subWidth1, subHeight1, id );
					const BoundingBoxConsolidated<int> rSubBoundingBox1 = BoundingBoxConsolidated<int>( x + subWidth1, y, subWidth2, subHeight1, id );

					this->remove( rSubBoundingBox0 );
					this->remove( rSubBoundingBox1 );
				}
				// Split all 4 ways
				else // [!] deals with 1,1 case
				{
					int subWidth1 = boundingBox.width / 2;
					int subHeight1 = boundingBox.height / 2;

					int subWidth2 = boundingBox.width - subWidth1;
					int subHeight2 = boundingBox.height - subHeight1;

					const BoundingBoxConsolidated<int> rSubBoundingBox0 = BoundingBoxConsolidated<int>( x, y, subWidth1, subHeight1, id );
					const BoundingBoxConsolidated<int> rSubBoundingBox1 = BoundingBoxConsolidated<int>( x + subWidth1, y, subWidth2, subHeight1, id );
					const BoundingBoxConsolidated<int> rSubBoundingBox2 = BoundingBoxConsolidated<int>( x, y + subHeight1, subWidth1, subHeight2, id );
					const BoundingBoxConsolidated<int> rSubBoundingBox3 = BoundingBoxConsolidated<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2, id );

					this->remove( rSubBoundingBox0 );
					this->remove( rSubBoundingBox1 );
					this->remove( rSubBoundingBox2 );
					this->remove( rSubBoundingBox3 );
				}
				// Don't need to check if both width and height == 1 because that was already accounted for in our base case
			}
			// The rBoundingBox fully resides in only one of the quadrants. Remove from that quadrant/CHILD
			else
			{
				this->_referenceNodes[this->_childrenNodeIndicies[quadrant]].remove( boundingBox );
			}
		}
		// Else, there is not more child nodes to search through; return.
	}

	return;
}



template<typename T>
bool QuadTree<T>::isCellOccupied( const BoundingBoxConsolidated<int>& boundingBox )
{
	// If out of bounds, techincally not occupied
	if ( !boundingBox.intersects( this->_quadTreeBounds ) )
	{
		// throw error out of bounds
		return false;
	}

	// If it is already consolidated, the bounding boxes is already occupied
	if ( this->_consolidated )
	{
		return true;
	}


	// Check to see if the cell is occupied
	for ( int i = 0; i < 4; i++ )
	{
		if ( ( this->_boundingBoxes[i] == boundingBox ) )
		{
			return true;
		}
	}

	// If this level is not occupied, check the lower levels
	if ( this->_childrenNodeIndicies != nullptr )
	{
		int quadrant = this->getQuadrant( boundingBox );
		// If addition can be fully encapsulated to one quadrant, check only tha quadrant
		if ( quadrant != -1 )
		{
			return this->_referenceNodes[this->_childrenNodeIndicies[quadrant]].isCellOccupied( boundingBox );
		}
		// If not, we need to divide and check occupy for each subdivision on THIS (but for now just say false)
		else
		{
			// Divide
			return false;
		}
	}

	// If no children, we reached level zero (implied by childrenNodeIndicies == nullptr),
	// checked all its bounding boxes, still sees no occupation, then return false 
	return false;
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

