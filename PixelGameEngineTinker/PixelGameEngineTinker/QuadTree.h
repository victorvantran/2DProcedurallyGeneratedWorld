#pragma once

#include "Settings.h"
#include "TileRender.h"
#include "LightRender.h"
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

template<typename TRender>
class QuadTree
{
private:
	int _myIndex;
	int _parentIndex;
	int _level;
	std::uint8_t _quadrant;
	BoundingBox<std::int64_t> _quadTreeBounds;
	bool _divided;
	bool _consolidated;
	int _cellCount;

	TRender _cell[4];
	int _childrenIndex[4];

	QuadTree<TRender>* _referenceNodes;

	std::uint32_t _minLevel;
	std::uint32_t _maxLevel;
	float _minCellSize;
public:
	QuadTree();
	~QuadTree();

	QuadTree( int myIndex, int parentIndex, int level, std::uint8_t quadrant, BoundingBox<std::int64_t> bounds, QuadTree<TRender>* referenceNodes,
		std::uint32_t minLevel, std::uint32_t maxLevel, float minCellSize );

	void constructQuadTree( int myIndex = -1, int parentIndex = -1, int level = -1, std::uint8_t quadrant = -1, BoundingBox<std::int64_t> bounds = BoundingBox<std::int64_t>(), QuadTree<TRender>* referenceNodes = nullptr,
		std::uint32_t minLevel = 0, std::uint32_t maxLevel = 0, float minCellSize = 0.0f );

	void divide();

	int getQuadrant( const TRender& boundingBox );

	void consolidate( int level );
	void insert( const TRender& encapsulateCell );
	void remove( const TRender& rRenderCell );

	bool isCellOccupied( const TRender& renderCell );

	int* getChildrenNodeIndicies();
	QuadTree<TRender>* getReferenceNodes();
	QuadTree<TRender>& getRootNode();

	BoundingBox<std::int64_t> getBounds() const;
	TRender* getCells();
	int getIndex() const;
	int getParentIndex() const;
	int getLevel() const;
	int getMyQuadrant() const;
	int getCellCount() const;

	bool isConsolidated() const;

	std::uint32_t getMinLevel() const;
	std::uint32_t getMaxLevel() const;
	float getMinCellSize() const;
};


template<typename TRender>
QuadTree<TRender>::QuadTree()
	: _myIndex( -1 ), _parentIndex( -1 ), _level( -1 ), _quadrant( -1 ), _quadTreeBounds( BoundingBox<std::int64_t>() ), _divided( false ), _consolidated( false ), _cellCount( 0 ),
	_cell{}, _childrenIndex{ -1, -1, -1, -1 }, _referenceNodes( nullptr ),
	_minLevel( 0 ), _maxLevel( 0 ), _minCellSize( 0.0f )

{

}


template<typename TRender>
QuadTree<TRender>::QuadTree( int myIndex, int parentIndex, int level, std::uint8_t quadrant, BoundingBox<std::int64_t> quadTreeBounds, QuadTree<TRender>* referenceNodes,
	std::uint32_t minLevel, std::uint32_t maxLevel, float minCellSize )
{
	constructQuadTree( myIndex, parentIndex, level, quadrant, quadTreeBounds, referenceNodes,
		minLevel, maxLevel, minCellSize );
}


template<typename TRender>
QuadTree<TRender>::~QuadTree()
{

}


template<typename TRender>
void QuadTree<TRender>::constructQuadTree( int myIndex, int parentIndex, int level, std::uint8_t quadrant, BoundingBox<std::int64_t> quadTreeBounds, QuadTree<TRender>* referenceNodes,
	std::uint32_t minLevel, std::uint32_t maxLevel, float minCellSize )
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

	this->_minLevel = minLevel;
	this->_maxLevel = maxLevel;
	this->_minCellSize = minCellSize;

	return;
}


template<typename TRender>
void QuadTree<TRender>::divide()
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

	this->_cell[0] = TRender( 0, BoundingBox<std::int64_t>( x, y, subWidth, subHeight ) );
	this->_cell[1] = TRender( 0, BoundingBox<std::int64_t>( x + subWidth, y, subWidth, subHeight ) );
	this->_cell[2] = TRender( 0, BoundingBox<std::int64_t>( x, y + subHeight, subWidth, subHeight ) );
	this->_cell[3] = TRender( 0, BoundingBox<std::int64_t>( x + subWidth, y + subHeight, subWidth, subHeight ) );

	this->_divided = true;

	// Do not further split for the lowest level
	if ( this->_level == this->_minLevel )
	{
		return;
	}

	// Calculate initial Shift
	int initialShift = 0;
	for ( int i = this->_level; i <= this->_maxLevel; i++ )
	{
		initialShift += ( 4 << ( ( this->_maxLevel - i ) * 2 ) ) / 4; // 4^(5 - parentLevel)
	}

	// Calculate recursive Shift
	int recursiveShift = 0;
	QuadTree<TRender>* ancestor = this;
	int j = 1;
	for ( int i = this->_level; i <= this->_maxLevel; i++ )
	{
		recursiveShift += ancestor->getMyQuadrant() * ( ( 4 << ( j * 2 ) ) / 4 );
		ancestor = &this->_referenceNodes[ancestor->getParentIndex()]; // index is the quadrant
		j++;
	}

	this->_childrenIndex[0] = initialShift + recursiveShift + 0;
	this->_childrenIndex[1] = initialShift + recursiveShift + 1;
	this->_childrenIndex[2] = initialShift + recursiveShift + 2;
	this->_childrenIndex[3] = initialShift + recursiveShift + 3;

	this->_referenceNodes[this->_childrenIndex[0]].constructQuadTree( this->_childrenIndex[0], this->_myIndex, this->_level - 1, 0, BoundingBox<std::int64_t>( x, y, subWidth, subHeight ), this->_referenceNodes,
		this->_minLevel, this->_maxLevel, this->_minCellSize );
	this->_referenceNodes[this->_childrenIndex[1]].constructQuadTree( this->_childrenIndex[1], this->_myIndex, this->_level - 1, 1, BoundingBox<std::int64_t>( x + subWidth, y, subWidth, subHeight ), this->_referenceNodes,
		this->_minLevel, this->_maxLevel, this->_minCellSize );
	this->_referenceNodes[this->_childrenIndex[2]].constructQuadTree( this->_childrenIndex[2], this->_myIndex, this->_level - 1, 2, BoundingBox<std::int64_t>( x, y + subHeight, subWidth, subHeight ), this->_referenceNodes,
		this->_minLevel, this->_maxLevel, this->_minCellSize );
	this->_referenceNodes[this->_childrenIndex[3]].constructQuadTree( this->_childrenIndex[3], this->_myIndex, this->_level - 1, 3, BoundingBox<std::int64_t>( x + subWidth, y + subHeight, subWidth, subHeight ), this->_referenceNodes,
		this->_minLevel, this->_maxLevel, this->_minCellSize );

	return;
}



template<typename TRender>
int QuadTree<TRender>::getQuadrant( const TRender& renderCell )
{
	BoundingBox<std::int64_t> boundingBox = renderCell.getBounds();

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





template<typename TRender>
void QuadTree<TRender>::consolidate( int level )
{
	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	// Height check not necessary if our bounding box is a square, but for formality

	if (_cell[0].getId() != /*TileIdentity::Void*/ 0 &&
		_cell[0].exists() &&
		_cell[0].getWidth() == ( ( 2 << ( this->_level ) ) / 2 ) &&
		_cell[0].getHeight() == ( ( 2 << ( this->_level ) ) / 2 ) &&
		_cell[0] == _cell[1] &&
		_cell[1] == _cell[2] &&
		_cell[2] == _cell[3]
		)
	{

		// Add encapsulated box 
		int posX = std::min<std::int64_t>( std::min<std::int64_t>( _cell[0].getX(), _cell[1].getX() ), std::min<std::int64_t>( _cell[2].getX(), _cell[3].getX() ) ); // technically only need to check for 3 of them
		int posY = std::min<std::int64_t>( std::min<std::int64_t>( _cell[0].getY(), _cell[1].getY() ), std::min<std::int64_t>( _cell[2].getY(), _cell[3].getY() ) );
		int width = _cell[0].getWidth() * 2;
		int height = _cell[0].getHeight() * 2;
		//TileIdentity id = _cell[0].getId();
		int id = _cell[0].getId();
		bool exist = _cell[0].exists();

		TRender renderCell = TRender( id, BoundingBox<std::int64_t>( posX, posY, width, height ) );

		this->_consolidated = true;
		this->_referenceNodes[this->_parentIndex].insert( renderCell );
		// Else no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
	}

	return;
}




template<typename TRender>
void QuadTree<TRender>::insert( const TRender& aRenderCell )
{
	// Recursively adds bounding boxes and consolidates
	BoundingBox<std::int64_t> aBoundingBox = aRenderCell.getBounds();

	// If it does not exist, return
	if ( !aRenderCell.exists() )
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
		if ( this->_cell[i].exists() && this->_cell[i].getBounds() == aBoundingBox )
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

		trimX = std::max<std::int64_t>( this->_quadTreeBounds.getX(), aBoundingBox.getX() );
		trimY = std::max<std::int64_t>( this->_quadTreeBounds.getY(), aBoundingBox.getY() );

		trimWidth = std::min<std::int64_t>( this->_quadTreeBounds.getX() + this->_quadTreeBounds.getWidth(), aBoundingBox.getX() + aBoundingBox.getWidth() ) - trimX;
		trimHeight = std::min<std::int64_t>( this->_quadTreeBounds.getY() + this->_quadTreeBounds.getHeight(), aBoundingBox.getY() + aBoundingBox.getHeight() ) - trimY;

		if ( trimWidth == 0 || trimHeight == 0 )
		{
			return;
		}

		this->insert( TRender( aRenderCell.getId(), BoundingBox<std::int64_t>( trimX, trimY, trimWidth, trimHeight ) ) );
		return;
	}

	int quadrant = this->getQuadrant( aRenderCell );
	if ( quadrant == -1 )
	{
		int x = aBoundingBox.getX();
		int y = aBoundingBox.getY();
		// TileIdentity id = aRenderCell.getId();
		int id = aRenderCell.getId();
		bool exist = aRenderCell.exists();

		int subWidth1 = aBoundingBox.getWidth() / 2;
		int subHeight1 = aBoundingBox.getHeight() / 2;
		int subWidth2 = aBoundingBox.getWidth() - subWidth1;
		int subHeight2 = aBoundingBox.getHeight() - subHeight1;

		const TRender aSubRenderCell0 = TRender( id, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const TRender aSubRenderCell1 = TRender( id, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TRender aSubRenderCell2 = TRender( id, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TRender aSubRenderCell3 = TRender( id, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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
	if ( this->_level == this->_minLevel )
	{
		// Find the index by using the bounds, and the position of the single-cell bounding box
		//TileIdentity rId = aRenderCell.getId();
		int rId = aRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( !this->_cell[i].exists() && aBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rId*/ ) // [!] why does cell[i] have the same id???
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				this->_cell[i].setId( aRenderCell.getId() );
				this->_cellCount += 1;
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
			if ( !this->_cell[quadrant].exists() )
			{
				this->_cell[quadrant].setId( aRenderCell.getId() );
				this->_cellCount += 1;
			}
		}

		// Add to children regardless
		this->_referenceNodes[this->_childrenIndex[quadrant]].insert( aRenderCell ); /// 
	}

	// Consolidate if met the requirements of max objects and level
	if ( this->_cellCount >= 4 && this->_level >= this->_minLevel && this->_level < this->_maxLevel )
	{
		this->consolidate( this->_level );
	}

	return;
}


template<typename TRender>
void QuadTree<TRender>::remove( const TRender& rRenderCell )
{
	const BoundingBox<std::int64_t> rBoundingBox = rRenderCell.getBounds();

	// If it does not exist, return
	if ( !rRenderCell.exists() )
	{
		return;
	}

	// If COMPLETELY (not partially out of obunds) out of bounds; return
	if ( !rBoundingBox.intersects( this->_quadTreeBounds ) )
	{
		return;
	}

	// If any of the size < minsize, return
	if ( rBoundingBox.getWidth() < this->_minCellSize || rBoundingBox.getHeight() < this->_minCellSize )
	{
		return;
	}

	// Base case: remove the single-unit bounding box, and relay the changes to the subsequent tile
	if ( this->_level == this->_minLevel )
	{
		int rId = rRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].exists() && rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rId ) // what are the bigger cell's id when mixutre? [!]
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				this->_cell[i].setId( /*TileIdentity::Void*/ 0 );
				this->_cellCount -= 1;
				this->_consolidated = false;
			}
		}
		return;
	}
	// Recursive case: remove any top level bounding boxes that intersect with the rBoundingBox
	else
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].exists() && rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rRenderCell.getId() )
			{
				this->_cell[i].setId( /*TileIdentity::Void*/ 0 );
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
		//TileIdentity id = rRenderCell.getId();
		int  id = rRenderCell.getId();
		bool exist = rRenderCell.exists();

		int subWidth1 = rBoundingBox.getWidth() / 2;
		int subHeight1 = rBoundingBox.getHeight() / 2;

		int subWidth2 = rBoundingBox.getWidth() - subWidth1;
		int subHeight2 = rBoundingBox.getHeight() - subHeight1;

		const TRender rSubRenderCell0 = TRender( id, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const TRender rSubRenderCell1 = TRender( id, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TRender rSubRenderCell2 = TRender( id, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TRender rSubRenderCell3 = TRender( id, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

		this->remove( rSubRenderCell0 );
		this->remove( rSubRenderCell1 );
		this->remove( rSubRenderCell2 );
		this->remove( rSubRenderCell3 );
	}
	else if ( this->_level > this->_minLevel )
	{
		this->_referenceNodes[this->_childrenIndex[quadrant]].remove( rRenderCell );
	}

	return;
}







template<typename TRender>
bool QuadTree<TRender>::isCellOccupied( const TRender& renderCell )
{
	BoundingBox<std::int64_t> boundingBox = renderCell.getBounds();

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
	if ( this->_level > this->_minLevel ) ///
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



template<typename TRender>
int* QuadTree<TRender>::getChildrenNodeIndicies()
{
	return this->_childrenIndex;
}


template<typename TRender>
QuadTree<TRender>* QuadTree<TRender>::getReferenceNodes()
{
	return this->_referenceNodes;
}


template<typename TRender>
QuadTree<TRender>& QuadTree<TRender>::getRootNode()
{
	return this->_referenceNodes[0];
}


template<typename TRender>
BoundingBox<std::int64_t> QuadTree<TRender>::getBounds() const
{
	return this->_quadTreeBounds;
}


template<typename TRender>
TRender* QuadTree<TRender>::getCells()
{
	return this->_cell;
}


template<typename TRender>
int QuadTree<TRender>::getIndex() const
{
	return this->_myIndex;
}


template<typename TRender>
int QuadTree<TRender>::getParentIndex() const
{
	return this->_parentIndex;
}


template<typename TRender>
int QuadTree<TRender>::getLevel() const
{
	return this->_level;
}


template<typename TRender>
int QuadTree<TRender>::getMyQuadrant() const
{
	return this->_quadrant;
}


template<typename TRender>
int QuadTree<TRender>::getCellCount() const
{
	return this->_cellCount;
}


template<typename TRender>
bool QuadTree<TRender>::isConsolidated() const
{
	return this->_consolidated;
}


template<typename TRender>
std::uint32_t QuadTree<TRender>::getMinLevel() const
{
	return this->_minLevel;
}


template<typename TRender>
std::uint32_t QuadTree<TRender>::getMaxLevel() const
{
	return this->_maxLevel;
}


template<typename TRender>
float QuadTree<TRender>::getMinCellSize() const
{
	return this->_minCellSize;
}



// Template Specilization

// TileRender
template<>
void QuadTree<TileRender>::divide();

template<>
void QuadTree<TileRender>::consolidate( int level );

template<>
void QuadTree<TileRender>::insert( const TileRender& aRenderCell );

template<>
void QuadTree<TileRender>::remove( const TileRender& aRenderCell );

// LightRender

template<>
void QuadTree<LightRender>::divide();

template<>
void QuadTree<LightRender>::consolidate( int level );

template<>
void QuadTree<LightRender>::insert( const LightRender& aRenderCell );

template<>
void QuadTree<LightRender>::remove( const LightRender& aRenderCell );