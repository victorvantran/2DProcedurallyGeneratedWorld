#pragma once

#include "Settings.h"
#include "TileRender.h"
#include "Tile.h"
#include "BoundingBox.h"
#include <vector>
#include <iterator>



/*
Linearized is advantagous because it offers:
	memory locality
	less overhead in memory allocation
	less memory
*/

template<typename T, typename TRender>
class QuadTree
{
public:
	static const int _MIN_LEVEL = Settings::QuadTree::MIN_LEVEL; // ( level => gridsize => numCell ) 5 => 1024 =>64, 4 => 32 => 512, 3 => 16 => 256, 2 => 8 => 128, 1 => 4 => 64, 0 => 2 => 32
	static const int _MAX_LEVEL = Settings::QuadTree::MAX_LEVEL;
	static const int _MIN_CELL_SIZE = ( 2 << QuadTree<T, TRender>::_MIN_LEVEL ) / 2;

	static const int _MAP_WIDTH = ( 2 << QuadTree<T, TRender>::_MAX_LEVEL );
	static const int _MAP_HEIGHT = ( 2 << QuadTree<T, TRender>::_MAX_LEVEL );

private:
	int _myIndex;
	int _parentIndex;
	int _level;
	int _quadrant;

	BoundingBox<int> _quadTreeBounds;


	bool _divided;
	bool _consolidated;
	int _cellCount;

	TRender _cell[4];
	int _childrenIndex[4];

	QuadTree<T, TRender>* _referenceNodes;
	T* _map;

protected:

public:

	QuadTree();
	~QuadTree();

	QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> bounds, QuadTree<T, TRender>* referenceNodes, T* map );

	void constructQuadTree( int myIndex = -1, int parentIndex = -1, int level = -1, int quadrant = -1, BoundingBox<int> bounds = BoundingBox<int>(), QuadTree<T, TRender>* referenceNodes = nullptr, T* map = nullptr);

	void clear();
	void divide();
	int getQuadrant( const TRender& boundingBox );

	void consolidate( int level );
	void insert( const TRender& encapsulateCell );
	void remove( const TRender& rRenderCell );


	bool isCellOccupied( const TRender& renderCell );


	int* getChildrenNodeIndicies();
	QuadTree<T, TRender>* getReferenceNodes();
	QuadTree<T, TRender>& getRootNode();


	BoundingBox<int> getBounds();

	TRender* getCells();

	int getIndex();
	int getParentIndex();
	int getLevel();
	int getMyQuadrant();
	int getCellCount();

	bool isConsolidated();

};


template<typename T, typename TRender>
QuadTree<T, TRender>::QuadTree()
	: _myIndex( -1 ), _parentIndex( -1 ), _level( -1 ), _quadrant( -1 ), _quadTreeBounds( BoundingBox<int>() ), _divided( false ), _consolidated( false ), _cellCount( 0 ),
	_cell{}, _childrenIndex{ -1, -1, -1, -1 }, _referenceNodes( nullptr ), _map( nullptr )
{

}


template<typename T, typename TRender>
QuadTree<T, TRender>::QuadTree( int myIndex, int parentIndex, int level, int quadrant, BoundingBox<int> quadTreeBounds, QuadTree<T, TRender>* referenceNodes, T* map )
{
	constructQuadTree( myIndex, parentIndex, level, quadrant, quadTreeBounds, referenceNodes, map );
}


template<typename T, typename TRender>
QuadTree<T, TRender>::~QuadTree()
{
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
	// Clear all the Tiles and Consolidated Tiles within the QuadTree
	if ( !this->_divided )
	{
		return;
	}
	
	this->_myIndex = -1;
	this->_parentIndex = -1;
	this->_level = -1;
	this->_quadrant = -1;
	this->_quadTreeBounds;
	this->_divided = false;
	this->_consolidated = false;
	this->_cellCount = 0;
	
	for ( int i = 0; i < 4; i++ )
	{
		this->_cell[i] = TRender();
	}

	for ( int i = 0; i < 4; i++ )
	{
		this->_childrenIndex[i] = -1;
	}

	this->_referenceNodes = nullptr;
	this->_map = nullptr;

	if ( this->_level > QuadTree<T, TRender>::_MIN_LEVEL )
	{
		this->_referenceNodes[this->_childrenIndex[0]].clear();
		this->_referenceNodes[this->_childrenIndex[1]].clear();
		this->_referenceNodes[this->_childrenIndex[2]].clear();
		this->_referenceNodes[this->_childrenIndex[3]].clear();
	}


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

	if ( this->_divided )
	{
		return;
	}

	int x = this->_quadTreeBounds.getX();
	int y = this->_quadTreeBounds.getY();
	int subWidth = this->_quadTreeBounds.getWidth() / 2;
	int subHeight = this->_quadTreeBounds.getHeight() / 2;

	this->_cell[0] = TRender( 0, BoundingBox<int>( x, y, subWidth, subHeight ) );
	this->_cell[1] = TRender( 0, BoundingBox<int>( x + subWidth, y, subWidth, subHeight ) );
	this->_cell[2] = TRender( 0, BoundingBox<int>( x, y + subHeight, subWidth, subHeight ) );
	this->_cell[3] = TRender( 0, BoundingBox<int>( x + subWidth, y + subHeight, subWidth, subHeight ) );

	this->_divided = true;


	// Do not further split for the lowest level
	if ( this->_level == QuadTree<T, TRender>::_MIN_LEVEL )
	{
		return;
	}

	// Calculate initial Shift
	int initialShift = 0;
	for ( int i = this->_level; i <= this->_MAX_LEVEL; i++ )
	{
		initialShift += ( 4 << ( ( this->_MAX_LEVEL - i ) * 2 ) ) / 4; // 4^(5 - parentLevel)
	}

	// Calculate recursive Shift
	int recursiveShift = 0;
	QuadTree<T, TRender>* ancestor = this;
	int j = 1;
	for ( int i = this->_level; i <= this->_MAX_LEVEL; i++ )
	{
		recursiveShift += ancestor->getMyQuadrant() * ( ( 4 << ( j * 2 ) ) / 4 );
		ancestor = &this->_referenceNodes[ancestor->getParentIndex()]; // index is the quadrant
		j++;
	}

	this->_childrenIndex[0] = initialShift + recursiveShift + 0;
	this->_childrenIndex[1] = initialShift + recursiveShift + 1;
	this->_childrenIndex[2] = initialShift + recursiveShift + 2;
	this->_childrenIndex[3] = initialShift + recursiveShift + 3;

	this->_referenceNodes[this->_childrenIndex[0]].constructQuadTree( this->_childrenIndex[0], this->_myIndex, this->_level - 1, 0, BoundingBox<int>( x, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[1]].constructQuadTree( this->_childrenIndex[1], this->_myIndex, this->_level - 1, 1, BoundingBox<int>( x + subWidth, y, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[2]].constructQuadTree( this->_childrenIndex[2], this->_myIndex, this->_level - 1, 2, BoundingBox<int>( x, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );
	this->_referenceNodes[this->_childrenIndex[3]].constructQuadTree( this->_childrenIndex[3], this->_myIndex, this->_level - 1, 3, BoundingBox<int>( x + subWidth, y + subHeight, subWidth, subHeight ), this->_referenceNodes, this->_map );

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
			_cell[0].getId() != 0
		)
		&&
		(
			( _cell[0].getId() == _cell[1].getId() ) &&
			( _cell[2].getId() == _cell[3].getId() ) &&
			( _cell[0].getId() == _cell[3].getId() )
		)
		&&
		// [!] need configuration to be equal too
		// existence impies childrenNodes are consolidated
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
			( level == this->_MIN_LEVEL ) ||
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

		// Add encapsulated box 
		int posX = std::min<int>( std::min<int>( _cell[0].getX(), _cell[1].getX() ), std::min<int>( _cell[2].getX(), _cell[3].getX() ) ); // technically only need to check for 3 of them
		int posY = std::min<int>( std::min<int>( _cell[0].getY(), _cell[1].getY() ), std::min<int>( _cell[2].getY(), _cell[3].getY() ) );
		int width = _cell[0].getWidth() * 2;
		int height = _cell[0].getHeight() * 2;
		int id = _cell[0].getId();
		bool exist = _cell[0].getExist();


		TRender renderCell = TRender( id, BoundingBox<int>( posX, posY, width, height ) );

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

	// Cannot add what is out of bounds
	if ( !aBoundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}

	// If it is already consolidated, the bounding boxes is already included
	if ( this->_consolidated || this->_cellCount == 4 ) // [!] Something wrong wehre cellCount is not decremented properly
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

		this->insert( TRender( aRenderCell.getId(), BoundingBox<int>( trimX, trimY, trimWidth, trimHeight ) ) );
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

		const TRender aSubRenderCell0 = TRender( id, BoundingBox<int>( x, y, subWidth1, subHeight1 ) );
		const TRender aSubRenderCell1 = TRender( id, BoundingBox<int>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TRender aSubRenderCell2 = TRender( id, BoundingBox<int>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TRender aSubRenderCell3 = TRender( id, BoundingBox<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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
	if ( this->_level == this->_MIN_LEVEL )
	{
		// Find the index by using the bounds, and the position of the single-cell bounding box
		int rId = aRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( !this->_cell[i].getExist() && aBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rId*/ ) // [!] why does cell[i] have the same id???
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY(); 

				this->_cell[i].setId( aRenderCell.getId() );
				this->_cellCount += 1;

				//this->_map[( localCellIndexY * QuadTree<T, TRender>::_MAP_WIDTH + localCellIndexX )].setId( rId );
				//this->_map[( localCellIndexY * QuadTree<T, TRender>::_MAP_WIDTH + localCellIndexX )].setExist( true );
				//this->_map[( localCellIndexY * QuadTree<T, TRender>::_MAP_WIDTH + localCellIndexX )].setBounds( aBoundingBox );
			}
		}
	}
	// Level > Min: Fill the bounding boxes and recur to children nodes
	else
	{
		int quadrant = this->getQuadrant( aRenderCell ); // Gauranteed not to equal negative one based on the previous conditions we checked for
		// Only add to the parent once it's children are consolidated
		if ( this->_referenceNodes[this->_childrenIndex[quadrant]].getBounds() == aBoundingBox && this->_referenceNodes[this->_childrenIndex[quadrant]].isConsolidated() )
		{
			if ( !this->_cell[quadrant].getExist() )
			{
				this->_cell[quadrant].setId( aRenderCell.getId() );
				this->_cellCount += 1;
			}
		}

		// Add to children regardless
		this->_referenceNodes[this->_childrenIndex[quadrant]].insert( aRenderCell ); /// 
	}

	// Consolidate if met the requirements of max objects and level
	if ( this->_cellCount >= 4 && this->_level >= this->_MIN_LEVEL && this->_level < QuadTree<T, TRender>::_MAX_LEVEL )
	{
		this->consolidate( this->_level );
	}

	return;
}


template<typename T, typename TRender>
void QuadTree<T, TRender>::remove( const TRender& rRenderCell )
{
	const BoundingBox<int> rBoundingBox = rRenderCell.getBounds();

	// If it does not exist, return
	if ( !rRenderCell.getExist() )
	{
		return;
	}

	// If COMPLETELY (not partially out of obunds) out of bounds; return
	if ( !rBoundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}

	// If any of the size < minsize, return
	if ( rBoundingBox.getWidth() < QuadTree<T,TRender>::_MIN_CELL_SIZE || rBoundingBox.getHeight() < QuadTree<T, TRender>::_MIN_CELL_SIZE )
	{
		return;
	}

	// Base case: remove the single-unit bounding box, and relay the changes to the subsequent tile
	if ( this->_level == QuadTree<T, TRender>::_MIN_LEVEL )
	{
		int rId = rRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].getExist() && rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rId ) // what are the bigger cell's id when mixutre? [!]
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();
				
				this->_cell[i].setId( 0 );
				this->_cellCount -= 1;
				this->_consolidated = false;
				// this->_map[( localCellIndexY * QuadTree<T, TRender>::_MAP_WIDTH + localCellIndexX )].setExist( false );

				// [!] SETID to 0
			}
		}
		return;
	}
	// Recursive case: remove any top level bounding boxes that intersect with the rBoundingBox
	else
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].getExist() && rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rRenderCell.getId() ) 
			{
				this->_cell[i].setId( 0 );
				this->_cellCount -= 1;
				this->_consolidated = false;
			}

		}
	}

	// If the box does not fully reside in one quadrant, we need to split the box
	int quadrant = this->getQuadrant( rRenderCell );
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

		const TRender rSubRenderCell0 = TRender( id, BoundingBox<int>( x, y, subWidth1, subHeight1 ) );
		const TRender rSubRenderCell1 = TRender( id, BoundingBox<int>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TRender rSubRenderCell2 = TRender( id, BoundingBox<int>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TRender rSubRenderCell3 = TRender( id, BoundingBox<int>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

		this->remove( rSubRenderCell0 );
		this->remove( rSubRenderCell1 );
		this->remove( rSubRenderCell2 );
		this->remove( rSubRenderCell3 );
	}
	else if ( this->_level > QuadTree<T, TRender>::_MIN_LEVEL )
	{
		this->_referenceNodes[this->_childrenIndex[quadrant]].remove( rRenderCell );
	}

	return;
}







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
	if ( this->_level > QuadTree<T, TRender>::_MIN_LEVEL ) ///
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
	return this->_childrenIndex;
}


template<typename T, typename TRender>
QuadTree<T, TRender>* QuadTree<T, TRender>::getReferenceNodes()
{
	return this->_referenceNodes;
}


template<typename T, typename TRender>
QuadTree<T, TRender>& QuadTree<T, TRender>::getRootNode()
{
	return this->_referenceNodes[0];
}


template<typename T, typename TRender>
BoundingBox<int> QuadTree<T, TRender>::getBounds()
{
	return this->_quadTreeBounds;
}


template<typename T, typename TRender>
TRender* QuadTree<T, TRender>::getCells()
{
	return this->_cell;
}


template<typename T, typename TRender>
int QuadTree<T, TRender>::getIndex()
{
	return this->_myIndex;
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

