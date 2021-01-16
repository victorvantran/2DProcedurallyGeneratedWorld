#include "QuadTree.h"
#include "TileRender.h"
#include "LightRender.h"

// TileRender

template <>
void QuadTree<TileRender>::divide()
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

	std::int64_t x = this->_quadTreeBounds.getX();
	std::int64_t y = this->_quadTreeBounds.getY();
	std::uint16_t subWidth = this->_quadTreeBounds.getWidth() / 2;
	std::uint16_t subHeight = this->_quadTreeBounds.getHeight() / 2;

	this->_cell[0] = TileRender( TileIdentity::Void, 0, BoundingBox<std::int64_t>( x, y, subWidth, subHeight ) );
	this->_cell[1] = TileRender( TileIdentity::Void, 0, BoundingBox<std::int64_t>( x + subWidth, y, subWidth, subHeight ) );
	this->_cell[2] = TileRender( TileIdentity::Void, 0, BoundingBox<std::int64_t>( x, y + subHeight, subWidth, subHeight ) );
	this->_cell[3] = TileRender( TileIdentity::Void, 0, BoundingBox<std::int64_t>( x + subWidth, y + subHeight, subWidth, subHeight ) );

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
	QuadTree<TileRender>* ancestor = this;
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




template<>
void QuadTree<TileRender>::consolidate( int level )
{
	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	// Height check not necessary if our bounding box is a square, but for formality

	if ( _cell[0].getId() != TileIdentity::Void &&
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
		TileIdentity id = _cell[0].getId();
		std::uint8_t bordersDecalIndex = _cell[0].getBordersDecalIndex();
		bool exist = _cell[0].exists();

		TileRender renderCell = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( posX, posY, width, height ) );

		this->_consolidated = true;
		this->_referenceNodes[this->_parentIndex].insert( renderCell );
		// Else no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
	}

	return;
}





template<>
void QuadTree<TileRender>::insert( const TileRender& aRenderCell )
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

		this->insert( TileRender( aRenderCell.getId(), aRenderCell.getBordersDecalIndex(), BoundingBox<std::int64_t>( trimX, trimY, trimWidth, trimHeight ) ) );
		return;
	}

	int quadrant = this->getQuadrant( aRenderCell );
	if ( quadrant == -1 )
	{
		int x = aBoundingBox.getX();
		int y = aBoundingBox.getY();
		TileIdentity id = aRenderCell.getId();
		std::uint8_t bordersDecalIndex = aRenderCell.getBordersDecalIndex();
		bool exist = aRenderCell.exists();

		int subWidth1 = aBoundingBox.getWidth() / 2;
		int subHeight1 = aBoundingBox.getHeight() / 2;
		int subWidth2 = aBoundingBox.getWidth() - subWidth1;
		int subHeight2 = aBoundingBox.getHeight() - subHeight1;

		const TileRender aSubRenderCell0 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const TileRender aSubRenderCell1 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TileRender aSubRenderCell2 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TileRender aSubRenderCell3 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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
		TileIdentity rId = aRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( !this->_cell[i].exists() && aBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rId*/ ) // [!] why does cell[i] have the same id???
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				this->_cell[i].setId( aRenderCell.getId() );
				this->_cell[i].setBordersDecalIndex( aRenderCell.getBordersDecalIndex() );
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
				this->_cell[quadrant].setBordersDecalIndex( aRenderCell.getBordersDecalIndex() );
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





template<>
void QuadTree<TileRender>::remove( const TileRender& rRenderCell )
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
		TileIdentity rId = rRenderCell.getId();
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].exists() && rBoundingBox.intersects( this->_cell[i].getBounds() ) && this->_cell[i].getId() == rId ) // what are the bigger cell's id when mixutre? [!]
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				//this->_cell[i].setId( TileIdentity::Void );
				this->_cell[i].clear();
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
				//this->_cell[i].setId( TileIdentity::Void );
				this->_cell[i].clear();
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
		TileIdentity id = rRenderCell.getId();
		std::uint8_t bordersDecalIndex = rRenderCell.getBordersDecalIndex();
		bool exist = rRenderCell.exists();

		int subWidth1 = rBoundingBox.getWidth() / 2;
		int subHeight1 = rBoundingBox.getHeight() / 2;

		int subWidth2 = rBoundingBox.getWidth() - subWidth1;
		int subHeight2 = rBoundingBox.getHeight() - subHeight1;

		const TileRender rSubRenderCell0 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const TileRender rSubRenderCell1 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const TileRender rSubRenderCell2 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const TileRender rSubRenderCell3 = TileRender( id, bordersDecalIndex, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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






























// LightRender

template <>
void QuadTree<LightRender>::divide()
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

	std::int64_t x = this->_quadTreeBounds.getX();
	std::int64_t y = this->_quadTreeBounds.getY();
	std::int64_t subWidth = this->_quadTreeBounds.getWidth() / 2;
	std::int64_t subHeight = this->_quadTreeBounds.getHeight() / 2;


	this->_cell[0] = LightRender( 0, 0, 0, 0, false, BoundingBox<std::int64_t>( x, y, subWidth, subHeight ) );
	this->_cell[1] = LightRender( 0, 0, 0, 0, false, BoundingBox<std::int64_t>( x + subWidth, y, subWidth, subHeight ) );
	this->_cell[2] = LightRender( 0, 0, 0, 0, false, BoundingBox<std::int64_t>( x, y + subHeight, subWidth, subHeight ) );
	this->_cell[3] = LightRender( 0, 0, 0, 0, false, BoundingBox<std::int64_t>( x + subWidth, y + subHeight, subWidth, subHeight ) );

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
	QuadTree<LightRender>* ancestor = this;
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





template<>
void QuadTree<LightRender>::insert( const LightRender& aRenderCell )
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

		//
		this->insert( LightRender( aRenderCell.corner0, aRenderCell.corner1, aRenderCell.corner2, aRenderCell.corner3, aRenderCell.exists(),
			BoundingBox<std::int64_t>( trimX, trimY, trimWidth, trimHeight ) ) );
		return;
	}

	int quadrant = this->getQuadrant( aRenderCell );
	if ( quadrant == -1 )
	{
		int x = aBoundingBox.getX();
		int y = aBoundingBox.getY();

		//int id = aRenderCell.getId();
		std::uint32_t corner0 = aRenderCell.corner0;
		std::uint32_t corner1 = aRenderCell.corner1;
		std::uint32_t corner2 = aRenderCell.corner2;
		std::uint32_t corner3 = aRenderCell.corner3;
		
		
		bool exist = aRenderCell.exists();

		int subWidth1 = aBoundingBox.getWidth() / 2;
		int subHeight1 = aBoundingBox.getHeight() / 2;
		int subWidth2 = aBoundingBox.getWidth() - subWidth1;
		int subHeight2 = aBoundingBox.getHeight() - subHeight1;

		const LightRender aSubRenderCell0 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const LightRender aSubRenderCell1 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const LightRender aSubRenderCell2 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const LightRender aSubRenderCell3 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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

	//int rId = aRenderCell.getId();
	std::uint32_t rCorner0 = aRenderCell.corner0;
	std::uint32_t rCorner1 = aRenderCell.corner1;
	std::uint32_t rCorner2 = aRenderCell.corner2;
	std::uint32_t rCorner3 = aRenderCell.corner3;

	// Level Min: Simply fill in the bounding boxes and end the recursion
	if ( this->_level == this->_minLevel )
	{
		// Find the index by using the bounds, and the position of the single-cell bounding box
		for ( int i = 0; i < 4; i++ )
		{
			if ( !this->_cell[i].exists() && aBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rId*/ ) // [!] why does cell[i] have the same id???
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				// this->_cell[i].setId( aRenderCell.getId() );
				this->_cell[i].corner0 = rCorner0;
				this->_cell[i].corner1 = rCorner1;
				this->_cell[i].corner2 = rCorner2;
				this->_cell[i].corner3 = rCorner3;
				this->_cell[i].exist = aRenderCell.exists();

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
				//this->_cell[quadrant].setId( aRenderCell.getId() );
				this->_cell[quadrant].corner0 = rCorner0;
				this->_cell[quadrant].corner1 = rCorner1;
				this->_cell[quadrant].corner2 = rCorner2;
				this->_cell[quadrant].corner3 = rCorner3;
				this->_cell[quadrant].exist = aRenderCell.exists();

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




template<>
void QuadTree<LightRender>::consolidate( int level )
{
	// Check the four boundingBoxes. If two conditions are met:
	// 1) they all have the same consolidation level
	// 2) Given the position and size of the boundingBoxes from the array, determine if they "fill" the bounds of the QuadTree
	// Height check not necessary if our bounding box is a square, but for formality
	
	if ( _cell[0].exists() &&
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


		//int id = _cell[0].getId();
		LightRender renderCell = LightRender( _cell[0].corner0, _cell[0].corner1, _cell[0].corner2, _cell[0].corner3, _cell[0].exists(), 
			BoundingBox<std::int64_t>( posX, posY, width, height ) );

		this->_consolidated = true;
		this->_referenceNodes[this->_parentIndex].insert( renderCell );

		// Else no need to consolidate at the root node beacuse there is no parent to insert the encapsulate box
	}

	return;
}





template<>
void QuadTree<LightRender>::remove( const LightRender& rRenderCell )
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
		/*int rId = rRenderCell.getId();*/
		for ( int i = 0; i < 4; i++ )
		{
			if ( this->_cell[i].exists() && rBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rId*/ ) // what are the bigger cell's id when mixutre? [!]
			{
				// Localizing the offset solves the issue of different quadrants introducing negative/wrong values when indexing.
				int localCellIndexX = this->_cell[i].getBounds().getX() - this->_referenceNodes[0].getBounds().getX();
				int localCellIndexY = this->_cell[i].getBounds().getY() - this->_referenceNodes[0].getBounds().getY();

				/*this->_cell[i].setId( TileIdentity::Void );*/
				//this->_cell[i].blacken();
				this->_cell[i].clear();
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
			if ( this->_cell[i].exists() && rBoundingBox.intersects( this->_cell[i].getBounds() ) /*&& this->_cell[i].getId() == rRenderCell.getId()*/ )
			{
				//this->_cell[i].setId( TileIdentity::Void );
				//this->_cell[i].blacken();
				this->_cell[i].clear();
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

		std::uint32_t corner0 = rRenderCell.corner0;
		std::uint32_t corner1 = rRenderCell.corner1;
		std::uint32_t corner2 = rRenderCell.corner2;
		std::uint32_t corner3 = rRenderCell.corner3;

		bool exist = rRenderCell.exists();

		int subWidth1 = rBoundingBox.getWidth() / 2;
		int subHeight1 = rBoundingBox.getHeight() / 2;

		int subWidth2 = rBoundingBox.getWidth() - subWidth1;
		int subHeight2 = rBoundingBox.getHeight() - subHeight1;

		const LightRender rSubRenderCell0 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x, y, subWidth1, subHeight1 ) );
		const LightRender rSubRenderCell1 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x + subWidth1, y, subWidth2, subHeight1 ) );
		const LightRender rSubRenderCell2 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x, y + subHeight1, subWidth1, subHeight2 ) );
		const LightRender rSubRenderCell3 = LightRender( corner0, corner1, corner2, corner3, exist, BoundingBox<std::int64_t>( x + subWidth1, y + subHeight1, subWidth2, subHeight2 ) );

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


