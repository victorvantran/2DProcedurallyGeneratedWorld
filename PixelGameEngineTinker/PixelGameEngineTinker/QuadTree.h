#pragma once

#include "TileRender.h"
#include "Tile.h"
#include "BoundingBox.h"
#include "BoundingBoxConsolidated.h"
#include <vector>
#include <iterator>


template<typename T, typename TRender>
class QuadTree
{
public: // private
	const static int _MAX_OBJECTS = 4;
	const static int _MIN_LEVELS = 0; // (level => gridsize => pixelSize) 5 => 1024 =>64, 4 => 32 => 512, 3 => 16 => 256, 2 => 8 => 128, 1 => 4 => 64, 0 => 2 => 32
	const static int _MAX_LEVELS = 5; // [!] need to fix max levels
	const static int _MIN_CELL_SIZE = ( 2 << QuadTree<T, TRender>::_MIN_LEVELS ) / 2;

	const static int _MAP_WIDTH = ( 2 << QuadTree<T, TRender>::_MAX_LEVELS );
	const static int _MAP_HEIGHT = ( 2 << QuadTree<T, TRender>::_MAX_LEVELS );


	int _myIndex;
	int _parentIndex;
	int _level;
	int _quadrant;

	BoundingBox<int> _quadTreeBounds;


	bool _divided;
	bool _consolidated;
	bool _shaded;


	TRender _cell[4];
	int _cellCount;

	int _childrenIndex[4];

	QuadTree<T, TRender>* _referenceNodes; // list of nodes to reference

	// Bounding boxes can be used to locate the tile contents with a reference to layer mapping
	T* _map;

protected:

public:

	QuadTree();
	~QuadTree();

	QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> bounds, QuadTree<T, TRender>* referenceNodes, T* map );

	void constructQuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> bounds, QuadTree<T, TRender>* referenceNodes, T* map );

	void clear();
	void divide();
	int getQuadrant( const TRender& boundingBox );

	void consolidate( int level );
	void insert( const TRender& encapsulateCell );
	void remove( const TRender& rRenderCell );

	//std::vector<BoundingBoxConsolidated<int>> query( const BoundingBoxConsolidated<int>& boundingBox, std::vector<BoundingBoxConsolidated<int>>& returnObjects );

	bool isCellOccupied( const TRender& renderCell );


	int* getChildrenNodeIndicies();

	BoundingBox<int> getBounds();

	//BoundingBoxConsolidated<int>* getBoundingBoxes();
	TRender* getCells();

	int getParentIndex();
	int getLevel();
	int getMyQuadrant();
	int getCellCount();


	bool isConsolidated();


};


template<typename T, typename TRender>
QuadTree<T, TRender>::QuadTree()
{
	this->_myIndex = -1;
	this->_parentIndex = -1;
	this->_level = -1;
	this->_quadrant = -1;
	this->_quadTreeBounds;
	this->_divided = false;
	this->_consolidated = false;
	this->_shaded = false;


	this->_cellCount = 0;

	for ( int i = 0; i < 4; i++ )
	{
		this->_childrenIndex[i] = -1;
	}


	this->_referenceNodes = nullptr;
	this->_map = nullptr;
}


template<typename T, typename TRender>
QuadTree<T, TRender>::QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> quadTreeBounds, QuadTree<T, TRender>* referenceNodes, T* map )
{
	constructQuadTree( myIndex, parentIndex, level, quadrant, quadTreeBounds, referenceNodes, map );
}

template<typename T, typename TRender>
QuadTree<T, TRender>::~QuadTree()
{
	this->clear();
}

template<typename T, typename TRender>
void QuadTree<T, TRender>::constructQuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> quadTreeBounds, QuadTree<T, TRender>* referenceNodes, T* map )
{
	this->_myIndex = myIndex;
	this->_parentIndex = parentIndex;
	this->_level = level;
	this->_quadrant = quadrant;
	this->_quadTreeBounds = quadTreeBounds;
	this->_divided = false;
	this->_consolidated = false;
	this->_shaded = false;

	this->_cellCount = 0;

	for ( int i = 0; i < 4; i++ )
	{
		this->_childrenIndex[i] = -1;
	}


	this->_referenceNodes = referenceNodes;
	this->_map = map;

	return;
}

template<typename T, typename TRender>
void QuadTree<T, TRender>::clear()
{
	return;
}

template<typename T, typename TRender>
void QuadTree<T, TRender>::divide()
{
	// Splits the node into four subnodes each initialized with new bounds
	/*
	_________
	| 0 | 1 |
	|___|___|
	| 2 | 3 |
	|___|___|

	*/

	// Do not further split for the lowest level
	if ( this->_level == QuadTree<T, TRender>::_MIN_LEVELS )
	{
		return;
	}


	// Calculate initial Shift
	int initialShift = 0;
	for ( int i = this->_level; i <= this->_MAX_LEVELS; i++ )
	{
		initialShift += ( 4 << ( ( this->_MAX_LEVELS - i ) * 2 ) ) / 4; // 4^(5 - parentLevel)
	}


	// Calculate recursive Shift
	int recursiveShift = 0;
	QuadTree<T, TRender>* ancestor = this;
	int j = 1;
	for ( int i = this->_level; i <= this->_MAX_LEVELS; i++ )
	{
		recursiveShift += ancestor->getMyQuadrant() * ( ( 4 << ( j * 2 ) ) / 4 );
		ancestor = &this->_referenceNodes[ancestor->getParentIndex()]; // index is the quadrant
		j++;
	}


	this->_childrenIndex[0] = initialShift + recursiveShift + 0;
	this->_childrenIndex[1] = initialShift + recursiveShift + 1;
	this->_childrenIndex[2] = initialShift + recursiveShift + 2;
	this->_childrenIndex[3] = initialShift + recursiveShift + 3;



	int x = this->_quadTreeBounds.getX();
	int y = this->_quadTreeBounds.getY();
	int subWidth = this->_quadTreeBounds.getWidth() / 2;
	int subHeight = this->_quadTreeBounds.getHeight() / 2;

	this->_referenceNodes[this->_childrenIndex[0]].constructQuadTree( this->_childrenIndex[0], this->_myIndex, this->_level - 1, 0, BoundingBox<int>( x, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[1]].constructQuadTree( this->_childrenIndex[1], this->_myIndex, this->_level - 1, 1, BoundingBox<int>( x + subWidth, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[2]].constructQuadTree( this->_childrenIndex[2], this->_myIndex, this->_level - 1, 2, BoundingBox<int>( x, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[3]].constructQuadTree( this->_childrenIndex[3], this->_myIndex, this->_level - 1, 3, BoundingBox<int>( x + subWidth, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );

	this->_divided = true;

	return;
}


template<typename T, typename TRender>
int QuadTree<T, TRender>::getQuadrant( const TRender& renderCell )
{
	BoundingBox<int> boundingBox = renderCell.getBounds();

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





template<typename T, typename TRender>
void QuadTree<T, TRender>::consolidate( int level )
{
	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	// Height check not necessary if our bounding box is a square, but for formality
	if (
		(
			( _cell[0].getId() == _cell[1].getId() ) &&
			( _cell[2].getId() == _cell[3].getId() ) &&
			( _cell[0].getId() == _cell[3].getId() )
			)
		&&
		(
			(
				_cell[0].getExist() &&
				_cell[1].getExist() &&
				_cell[2].getExist() &&
				_cell[3].getExist()
				)
			)
		&&
		(
			_cell[0].getWidth() == ( ( 2 << ( this->_level ) ) / 2 ) &&
			_cell[0].getHeight() == ( ( 2 << ( this->_level ) ) / 2 )
			)
		&&
		(
			( level == this->_MIN_LEVELS ) ||
			(
				( _cell[0].getWidth() == _cell[1].getWidth() ) &&
				( _cell[2].getWidth() == _cell[3].getWidth() ) &&
				( _cell[0].getWidth() == _cell[3].getWidth() ) &&
				( _cell[0].getHeight() == _cell[1].getHeight() ) &&
				( _cell[2].getHeight() == _cell[3].getHeight() ) &&
				( _cell[0].getHeight() == _cell[3].getHeight() )
				)
			)
		)
	{
		// Since level is not lowest level, there is children to necessarilly check if all of them are consolidated
		if ( ( this->_level < this->_MAX_LEVELS ) && ( this->_level != this->_MIN_LEVELS ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				if ( !this->_referenceNodes[this->_childrenIndex[i]].isConsolidated() ) ///
				{
					return;
				}
			}
		}

		// Add encapsulated box 
		int posX = std::min<int>( std::min<int>( _cell[0].getX(), _cell[1].getX() ), std::min<int>( _cell[2].getX(), _cell[3].getX() ) ); // technically only need to check for 3 of them
		int posY = std::min<int>( std::min<int>( _cell[0].getY(), _cell[1].getY() ), std::min<int>( _cell[2].getY(), _cell[3].getY() ) );
		int width = _cell[0].getWidth() * 2;
		int height = _cell[0].getHeight() * 2;
		int id = _cell[0].getId();
		bool exist = _cell[0].getExist();


		TRender renderCell = TRender( id, BoundingBox<int>( posX, posY, width, height ), exist );

		this->_consolidated = true;
		this->_referenceNodes[this->_parentIndex].insert( renderCell );
		// Else no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
	}

	return;
}




template<typename T, typename TRender>
void QuadTree<T, TRender>::insert( const TRender& aRenderCell )
{
	// Recursively adds bounding boxes and consolidates
	BoundingBox<int> aBoundingBox = aRenderCell.getBounds();


	// If it does not exist, return
	if ( !aRenderCell.getExist() )
	{
		return;
	}

	// Cannot add what is out of boundss
	if ( !aBoundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}



	// If it is already consolidated, the bounding boxes is already included
	if ( this->_consolidated || this->_cellCount == 4 ) // [!]
	{
		return;
	}


	// If any dimension size ( x or y ) < 1, return
	if ( aBoundingBox.getWidth() < 1 || aBoundingBox.getHeight() < 1 )
	{
		return;
	}



	// Check if a bounding box is already occupied
	for ( int i = 0; i < 4; i++ )
	{
		if ( this->_cell[i].getExist() && this->_cell[i].getBounds() == aBoundingBox )
		{
			return;
		}
	}




	// If the boundingBox has any area that is outside the boundaries of quadTreeBounds, trim
	if ( !( this->_quadTreeBounds >= aBoundingBox ) )
	{
		int trimX;
		int trimY;
		int trimWidth;
		int trimHeight;

		trimX = std::max<int>( this->_quadTreeBounds.getX(), aBoundingBox.getX() );
		trimY = std::max<int>( this->_quadTreeBounds.getY(), aBoundingBox.getY() );

		trimWidth = std::min<int>( this->_quadTreeBounds.getX() + this->_quadTreeBounds.getWidth(), aBoundingBox.getX() + aBoundingBox.getWidth() ) - trimX;
		trimHeight = std::min<int>( this->_quadTreeBounds.getY() + this->_quadTreeBounds.getHeight(), aBoundingBox.getY() + aBoundingBox.getHeight() ) - trimY;


		if ( trimWidth == 0 || trimHeight == 0 )
		{
			return;
		}


		this->insert( TRender( aRenderCell.getId(), BoundingBox<int>( trimX, trimY, trimWidth, trimHeight ), aRenderCell.getExist() ) );
		return;
	}



	int quadrant = this->getQuadrant( aRenderCell );
	if ( quadrant == -1 )
	{
		int x = aBoundingBox.getX();
		int y = aBoundingBox.getY();
		int id = aRenderCell.getId();
		bool exist = aRenderCell.getExist();

		int subWidth1 = aBoundingBox.getWidth() / 2;
		int subHeight1 = aBoundingBox.getHeight() / 2;

		int subWidth2 = aBoundingBox.getWidth() - subWidth1;
		int subHeight2 = aBoundingBox.getHeight() - subHeight1;

		const TRender aSubRenderCell0 = TRender( id, BoundingBox<int>( x, y, subWidth1, subHeight1 ), exist );
		const TRender aSubRenderCell1 = TRender( id, BoundingBox<int>( x + subWidth1, y, subWidth2, subHeight1 ), exist );
		const TRender aSubRenderCell2 = TRender( id, BoundingBox<int>( x, y + subHeight1, subWidth1, subHeight2 ), exist );
		const TRender aSubRenderCell3 = TRender( id, BoundingBox<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ), exist );


		this->insert( aSubRenderCell0 );
		this->insert( aSubRenderCell1 );
		this->insert( aSubRenderCell2 );
		this->insert( aSubRenderCell3 );

		return;
	}

	// Else
	// Our aBoundingBox is gauranteed to be encapsulated by the quadTreeBounds at this point; And it is gauranteed to not occupy a bounding box yet
	// More than encapsulated, since we trimmed and split, it must be or not be the same as only one children bound ( we can break early )
	// Create the bounding box if any children quadrant bounds is equal to the aBoundingBox; this means we can index our bounding box by the quadrant!

	// Level Min: Simply fill in the bounding boxes and end the recursion
	if ( this->_level == this->_MIN_LEVELS )
	{
		// Existing check already done
		// Find the index by using the bounds, and the position of the single-cell bounding box

		/*
		for ( int i = 0; i < 4; i++ )
		{
			if ( !this->_cell[i].getExist() && ( aBoundingBox.getX() == this->_quadTreeBounds.getX() ) && ( aBoundingBox.getY() == this->_quadTreeBounds.getY() ) )
			{
				this->_cell[i] = aRenderCell;
				this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setId( aRenderCell.getId() );
				this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setExist( true ); // always true due to the check
				this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setBounds( aBoundingBox );
				this->_cellCount += 1;

				break;
			}
		}
		*/


		if ( !this->_cell[0].getExist() && ( aBoundingBox.getX() == this->_quadTreeBounds.getX() ) && ( aBoundingBox.getY() == this->_quadTreeBounds.getY() ) )
		{
			// map[ aBoundingBox.getY() * ...root tree +  aBoundingBox.getX()]
			this->_cell[0] = aRenderCell;
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setId( aRenderCell.getId() );
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setExist( true ); // always true due to the check
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setBounds( aBoundingBox );
			this->_cellCount += 1;
		}
		else if ( !this->_cell[1].getExist() && ( aBoundingBox.getX() == ( this->_quadTreeBounds.getX() + _MIN_CELL_SIZE ) ) && ( aBoundingBox.getY() == this->_quadTreeBounds.getY() ) )
		{
			this->_cell[1] = aRenderCell;
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setId( aRenderCell.getId() );
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setExist( true );
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setBounds( aBoundingBox );
			this->_cellCount += 1;
		}
		else if ( !this->_cell[2].getExist() && ( aBoundingBox.getX() == this->_quadTreeBounds.getX() ) && ( aBoundingBox.getY() == ( this->_quadTreeBounds.getY() + _MIN_CELL_SIZE ) ) )
		{
			this->_cell[2] = aRenderCell;
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setId( aRenderCell.getId() );
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setExist( true ); // always true due to the check
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setBounds( aBoundingBox );
			this->_cellCount += 1;
		}
		else if ( !this->_cell[3].getExist() && ( aBoundingBox.getX() == this->_quadTreeBounds.getX() + _MIN_CELL_SIZE ) && ( aBoundingBox.getY() == ( this->_quadTreeBounds.getY() + _MIN_CELL_SIZE ) ) )
		{
			this->_cell[3] = aRenderCell;
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setId( aRenderCell.getId() );
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setExist( true ); // always true due to the check
			this->_map[( aBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + aBoundingBox.getX() )].setBounds( aBoundingBox );
			this->_cellCount += 1;
		}

	}
	// Level > Min: Fill the bounding boxes and recur to children nodes
	else
	{
		if ( this->_childrenIndex == nullptr ) ///
		{
			this->divide();
		}


		int quadrant = this->getQuadrant( aRenderCell ); // Gauranteed not to equal negative one based on the previous conditions we checked for
		if ( this->_referenceNodes[this->_childrenIndex[quadrant]].getBounds() == aBoundingBox ) ///
		{
			if ( !this->_cell[quadrant].getExist() )
			{
				this->_cell[quadrant] = aRenderCell;
				this->_cellCount += 1;
			}
		}

		// Add to child regardless
		this->_referenceNodes[this->_childrenIndex[quadrant]].insert( aRenderCell ); /// 
	}


	// Consolidate if met the requirements of max objects and level
	if ( this->_cellCount >= this->_MAX_OBJECTS && this->_level >= this->_MIN_LEVELS )
	{
		this->consolidate( this->_level );
	}

	return;
}





template<typename T, typename TRender>
void QuadTree<T, TRender>::remove( const TRender& rRenderCell )
{

	BoundingBox<int> rBoundingBox = rRenderCell.getBounds();

	// If it does not exist, return
	if ( !rRenderCell.getExist() )
	{
		return;
	}


	// If COMPLETELY(not partially out of obunds) out of bounds; return
	if ( !rBoundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}

	// If any of the size < minsize, return
	if ( rBoundingBox.getWidth() < this->_MIN_CELL_SIZE || rBoundingBox.getHeight() < this->_MIN_CELL_SIZE )
	{
		return;
	}

	// If the rBoundingBlock is indeed within the bounds, consolidation must equal false because we are removing within (whether it was consolidated to begin with or not)
	this->_consolidated = false;

	// Base Case: Check to see if a high level tree has the single cell, should not be possible
	//if ( boundingBox.width == this->_MIN_CELL_SIZE && boundingBox.height == this->_MIN_CELL_SIZE )


	// Recursive Case: The cell is bigger than 1x1; therefore, we need to remove any bounding boxes found in children nodes that happen to encapsulate any part of that cell (due to consolidation)
	if ( this->_level > QuadTree<T, TRender>::_MIN_LEVELS )
	{
		// Try to find any bounding box to delete that touches the rBoundingBox (there may be multiple, so don't break the for loop preemptively)
		for ( int i = 0; i < 4; i++ )
		{
			if ( rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rRenderCell.getId() )
			{
				this->_cell[i].clear();
				this->_cellCount -= 1;
				// Cannot necessarilly return here because there may be smaller bounding boxes to remove and/or need to fragmentize the rBoundingBox
			}
		}
	}


	// Remember that it may not find a big/exact size bounding box to delete; still need to search deeper and fragmentize for potential small ones to remove
	// Check to see if there are any child nodes to look deeper in
	int quadrant = this->getQuadrant( rRenderCell );
	// If the box does not fully reside in one quadrant, we need to split the box; there is probably calculations one can do to optimize the split, but for now, just split it in half both ways
	if ( quadrant == -1 )
	{
		// Divide bounding boxes
		int x = rBoundingBox.getX();
		int y = rBoundingBox.getY();
		int id = rRenderCell.getId();
		bool exist = rRenderCell.getExist();

		int subWidth1 = rBoundingBox.getWidth() / 2;
		int subHeight1 = rBoundingBox.getHeight() / 2;

		int subWidth2 = rBoundingBox.getWidth() - subWidth1;
		int subHeight2 = rBoundingBox.getHeight() - subHeight1;

		const TRender rSubRenderCell0 = TRender( id, BoundingBox<int>( x, y, subWidth1, subHeight1 ), exist );
		const TRender rSubRenderCell1 = TRender( id, BoundingBox<int>( x + subWidth1, y, subWidth2, subHeight1 ), exist );
		const TRender rSubRenderCell2 = TRender( id, BoundingBox<int>( x, y + subHeight1, subWidth1, subHeight2 ), exist );
		const TRender rSubRenderCell3 = TRender( id, BoundingBox<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ), exist );


		this->remove( rSubRenderCell0 );
		this->remove( rSubRenderCell1 );
		this->remove( rSubRenderCell2 );
		this->remove( rSubRenderCell3 );

		// Don't need to check if both width and height == _MIN_CELL_SIZE because that was already accounted for in our base case
	}
	// The rBoundingBox fully resides in only one of the quadrants. Remove from that quadrant/CHILD
	else
	{
		this->_referenceNodes[this->_childrenIndex[quadrant]].remove( rRenderCell ); ///
	}


	// Base Case
	if ( this->_level == QuadTree<T, TRender>::_MIN_LEVELS )
	{
		for ( int i = 0; i < 4; i++ )
		{
			// Check to see if its within a bounding box
			if ( rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rRenderCell.getId() )
			{
				this->_map[( rBoundingBox.getY() * QuadTree<T, TRender>::_MAP_WIDTH + rBoundingBox.getX() )].setExist( false );
				this->_cell[i].clear();
				this->_cellCount -= 1;
				// Can break because 1x1 cell cannot reside in multiple bounding boxes
				// Deltes a nxn >= 1x1 bounding box but not neccessarily the 1x1 bounding box, so can't return yet
				break;
			}
		}
	}

	return;
}


/*
template<typename T, typename TRender>
std::vector<BoundingBoxConsolidated<int>> QuadTree<T, TRender>::query( const BoundingBoxConsolidated<int>& boundingBox, std::vector<BoundingBoxConsolidated<int>>& returnBoxes )
{
	// Returns all objects that could possibly collide with the given object (as well as a copy of the passed object itself (the copy resides in the _objects) )
	// Work around of the copies is to use pointers

	if ( !boundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}





	int index = getQuadrant( boundingBox );
	if ( this->_childrenIndex != nullptr ) ///
	{
		this->_referenceNodes[this->_childrenIndex[index]].query( boundingBox, returnBoxes ); ///
	}

	// concatenate/build up the list

	return returnBoxes;
}
*/



template<typename T, typename TRender>
bool QuadTree<T, TRender>::isCellOccupied( const TRender& renderCell )
{
	BoundingBox<int> boundingBox = renderCell.getBounds();


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
		if ( ( this->_cell[i].getBounds() == boundingBox ) )
		{
			return true;
		}
	}

	// If this level is not occupied, check the lower levels
	if ( this->_level > QuadTree<T, TRender>::_MIN_LEVELS ) ///
	{
		int quadrant = this->getQuadrant( renderCell );
		// If addition can be fully encapsulated to one quadrant, check only tha quadrant
		if ( quadrant != -1 )
		{
			return this->_referenceNodes[this->_childrenIndex[quadrant]].isCellOccupied( renderCell ); ///
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



template<typename T, typename TRender>
int* QuadTree<T, TRender>::getChildrenNodeIndicies()
{
	return this->_childrenIndex; ///
}

template<typename T, typename TRender>
BoundingBox<int> QuadTree<T, TRender>::getBounds()
{
	return this->_quadTreeBounds;
}


/*
template<typename T, typename TRender>
BoundingBoxConsolidated<int>* QuadTree<T, TRender>::getBoundingBoxes()
{
	return this->_boundingBoxes;
}
*/

template<typename T, typename TRender>
TRender* QuadTree<T, TRender>::getCells()
{
	return this->_cell;
}


template<typename T, typename TRender>
int QuadTree<T, TRender>::getParentIndex()
{
	return this->_parentIndex;
}


template<typename T, typename TRender>
int QuadTree<T, TRender>::getLevel()
{
	return this->_level;
}

template<typename T, typename TRender>
int QuadTree<T, TRender>::getMyQuadrant()
{
	return this->_quadrant;
}

template<typename T, typename TRender>
int QuadTree<T, TRender>::getCellCount()
{
	return this->_cellCount;
}

template<typename T, typename TRender>
bool QuadTree<T, TRender>::isConsolidated()
{
	return this->_consolidated;
}

