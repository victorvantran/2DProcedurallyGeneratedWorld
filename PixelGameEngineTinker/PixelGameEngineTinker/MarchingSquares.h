#pragma once
#include "olcPixelGameEngine.h"

// MarchingSquares algorithm for terrain smoothing
// Squares <-> Cells

namespace mesh
{
	struct Point
	{
		olc::vf2d position;

		Point();
		~Point();

		Point( const olc::vf2d& position );
	};


	struct ControlIndex : public Point
	{
		// An index of a cell with two additional pointers to points that are "halfway" between two cells
		// Up and Right
		Point* pointUp;
		Point* pointRight;
		bool exist;

		ControlIndex();
		~ControlIndex();

		ControlIndex( const olc::vf2d& position, bool exist );
	};


	struct Cell
	{
		// A cell in a grid of cells that represent a map
		ControlIndex* topLeft;
		ControlIndex* topRight;
		ControlIndex* bottomRight;
		ControlIndex* bottomLeft;

		Point* centerTop;
		Point* centerRight;
		Point* centerBottom;
		Point* centerLeft;

		int configuration;

		Cell();
		~Cell();

		Cell( ControlIndex* topLeft, ControlIndex* topRight, ControlIndex* bottomRight, ControlIndex* bottomLeft );
	};



	template<typename T>
	class Grid
	{
	private:
		Cell* _cells;
		ControlIndex* _controlIndicies;
		olc::vi2d _gridDimension;

	public:
		Grid();
		~Grid();

		Grid( T* map, const olc::vi2d& gridDimension );
		Grid( T* map, int gridWidth, int gridHeight );

		Cell* getCells();
		ControlIndex* getControlIndicies();

		olc::vi2d getDimension();
	};






	template<typename T> // Example: Tile
	class MarchingSquares : public olc::PGEX
	{
	private:
		Grid<T>* _grid;
		int* _meshMapping; // An array of integers that represent the configuration/id of a tile. Only purpose is to render. Nothing else about it is interactable. That is reserved for the Tile
		
	public:
		MarchingSquares();
		~MarchingSquares();

		
		void generateMeshMapping( T* map, const olc::vi2d& mapDimension );
		// void generate( layer );
		void destroy();

		void updateCell( const olc::vi2d& index ); // [!] Future: reconfigure a cell and the surround cells ( used if a tile is modified (i.e. destroyed, built, changed )

		Grid<T>* getGrid();
		int* getMapping();


		void meshify( Layer<T>& layer );

		void update( Layer<T>& layer );

		void triangulateCell( const mesh::Cell& square, int x, int y );

	};


}




// Point
mesh::Point::Point()
{

}

mesh::Point::~Point()
{

}

mesh::Point::Point( const olc::vf2d& position )
{
	this->position = position;
}


// ControlIndex
mesh::ControlIndex::ControlIndex()
{

}

mesh::ControlIndex::~ControlIndex()
{

}

mesh::ControlIndex::ControlIndex( const olc::vf2d& position, bool exist ) : Point( position )
{
	this->exist = exist;
	this->pointUp = new Point( position + olc::vf2d{ 0.0f, -0.5f } ); // To be deleted by the grid
	this->pointRight = new Point( position + olc::vf2d{ 0.5f, 0.0f } ); // To be deleted by the grid
}


// Cell
mesh::Cell::Cell()
{
	this->topLeft = nullptr;
	this->topRight = nullptr;
	this->bottomLeft = nullptr;
	this->bottomRight = nullptr;

	this->centerTop = nullptr;
	this->centerRight = nullptr;
	this->centerBottom = nullptr;
	this->centerLeft = nullptr;

	this->configuration = 0;
}

mesh::Cell::~Cell()
{

}

mesh::Cell::Cell( ControlIndex* topLeft, ControlIndex* topRight, ControlIndex* bottomRight, ControlIndex* bottomLeft )
{
	this->topLeft = topLeft;
	this->topRight = topRight;
	this->bottomRight = bottomRight;
	this->bottomLeft = bottomLeft;

	this->centerTop = topLeft->pointRight;
	this->centerRight = bottomRight->pointUp;
	this->centerBottom = bottomLeft->pointRight;
	this->centerLeft = bottomLeft->pointUp;

	this->configuration = 0;

	// the 16 possible squares can be mapped to binary 0000 -> 1111
	if ( this->topLeft->exist )
	{
		this->configuration += 1;
	}
	if ( this->topRight->exist )
	{
		this->configuration += 2;
	}
	if ( this->bottomRight->exist )
	{
		this->configuration += 4;
	}
	if ( this->bottomLeft->exist )
	{
		this->configuration += 8;
	}
}


// Grid<T>
template<typename T>
mesh::Grid<T>::Grid()
{
	this->_cells = nullptr;
	this->_controlIndicies = nullptr;
	this->_gridDimension = olc::vi2d{ 0, 0 };
}

template<typename T>
mesh::Grid<T>::~Grid()
{
	int gridWidth = this->_gridDimension.x;
	int gridHeight = this->_gridDimension.y;

	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			delete this->_controlIndicies[y * gridWidth + x].pointUp;
			this->_controlIndicies[y * gridWidth + x].pointUp = nullptr;

			delete this->_controlIndicies[y * gridWidth + x].pointRight;
			this->_controlIndicies[y * gridWidth + x].pointRight = nullptr;
		}
	}

	delete[] this->_cells;
	this->_cells = nullptr;

	delete[] this->_controlIndicies;
	this->_controlIndicies = nullptr;
}

template<typename T>
mesh::Grid<T>::Grid( T* map, const olc::vi2d& gridDimension )
{
	this->_gridDimension = gridDimension;

	int gridWidth = this->_gridDimension.x;
	int gridHeight = this->_gridDimension.y;

	// Mark whether a control index exists (for example, has a tile located on it)
	this->_controlIndicies = new ControlIndex[gridWidth * gridHeight];
	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			olc::vf2d position{ ( float )x, ( float )y }; // top left
			bool exist = map[y * gridWidth + x].exist; // ensure that T has exists() method
			this->_controlIndicies[y * gridWidth + x] = ControlIndex( position, exist );
		}
	}

	// The reason for subtracting 1 is that the cells in the first rows and/or column do not have the neighbors
	// to assign all of topRight, topLeft, bottomRight, bottomLeft
	// Cells[0,0] is actually starting on Grid[1,1]
	this->_cells = new Cell[( gridWidth - 1 ) * ( gridHeight - 1 )];
	for ( int x = 0; x < ( gridWidth - 1 ); x++ )
	{
		for ( int y = 0; y < ( gridHeight - 1 ); y++ )
		{
			this->_cells[y * ( gridWidth - 1 ) + x] = Cell(
				&this->_controlIndicies[( y ) * gridWidth + ( x )], // topLeft
				&this->_controlIndicies[( y ) * gridWidth + ( x + 1 )], // topRight
				&this->_controlIndicies[( y + 1 ) * gridWidth + ( x + 1 )], // bottomRight
				&this->_controlIndicies[( y + 1 ) * gridWidth + ( x )] ); // bottomLeft
			/*
			this->_cells[y * ( gridWidth - 1 ) + x] = Cell( 
				&this->_controlIndicies[( y ) * ( gridWidth - 1 ) + ( x )], // topLeft
				&this->_controlIndicies[( y ) * ( gridWidth - 1 ) + ( x + 1 )], // topRight
				&this->_controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x + 1)], // bottomRight
				&this->_controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x )]); // bottomLeft
			*/
		}
	}
}

template<typename T>
mesh::Grid<T>::Grid( T* map, int gridWidth, int gridHeight )
{
	this->_gridDimension = olc::vi2d{ gridWidth, gridHeight };

	// Mark whether a control index exists (for example, has a tile located on it)
	this->_controlIndicies = new ControlIndex[gridWidth * gridHeight];
	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			olc::vf2d position{ ( float )x, ( float )y }; // top left
			bool exist = map[y * gridWidth + x].exist; // ensure that T has exists() method
			this->_controlIndicies[y * gridWidth + x] = ControlIndex( position, exist );
		}
	}

	// The reason for subtracting 1 is that the cells in the first rows and/or column do not have the neighbors
	// to assign all of topRight, topLeft, bottomRight, bottomLeft
	// Cells[0,0] is actually starting on Grid[1,1]
	this->_cells = new Cell[( gridWidth - 1 ) * ( gridHeight - 1 )];
	for ( int x = 0; x < ( gridWidth - 1 ); x++ )
	{
		for ( int y = 0; y < ( gridHeight - 1 ); y++ )
		{
			this->_cells[y * ( gridWidth - 1 ) + x] = Cell(
				&this->_controlIndicies[( y ) * gridWidth + ( x )], // topLeft
				&this->_controlIndicies[( y ) * gridWidth + ( x + 1 )], // topRight
				&this->_controlIndicies[( y + 1 ) * gridWidth + ( x + 1 )], // bottomRight
				&this->_controlIndicies[( y + 1 ) * gridWidth + ( x )] ); // bottomLeft
		}
	}
}


template<typename T>
mesh::Cell* mesh::Grid<T>::getCells()
{
	return this->_cells;
}


template<typename T>
olc::vi2d mesh::Grid<T>::getDimension()
{
	return this->_gridDimension;
}


template<typename T>
mesh::ControlIndex* mesh::Grid<T>::getControlIndicies()
{
	return this->_controlIndicies;
}


// MarchingSquares
template<typename T>
mesh::MarchingSquares<T>::MarchingSquares()
{
	this->_grid = nullptr;
	this->_meshMapping = nullptr;
}

template<typename T>
mesh::MarchingSquares<T>::~MarchingSquares()
{
	this->destroy();
}

template<typename T>
void mesh::MarchingSquares<T>::generateMeshMapping( T* map, const olc::vi2d& mapDimension )
{
	this->destroy();

	int gridWidth = mapDimension.x;
	int gridHeight = mapDimension.y;

	this->_grid = new Grid( map, gridWidth, gridHeight );
	this->_meshMapping = new int[gridWidth * gridHeight];

	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			if ( x == 0 || y == 0 )
			{
				this->_meshMapping[y * gridWidth + x] = 15; // default solid block
			}
			else
			{
				this->_meshMapping[y * gridWidth + x] = this->_grid->getCells()[(y-1) * (gridWidth - 1) + (x-1)].configuration;
			}
		}
	}

	return;
}

// generateMeshMapping ( World Chunk )

template<typename T>
void mesh::MarchingSquares<T>::destroy()
{
	// Delete everything used to set up the creation of a mesh map, except the meshMap itself
	// After destroying, can reuse generateMeshMapping given a different map parameter

	delete this->_grid;
	this->_grid = nullptr;

	delete this->_meshMapping;
	this->_meshMapping = nullptr;
	return;
}


template<typename T>
mesh::Grid<T>* mesh::MarchingSquares<T>::getGrid()
{
	return this->_grid;
}

template<typename T>
int* mesh::MarchingSquares<T>::getMapping()
{
	return this->_meshMapping;
}

template<typename T>
void mesh::MarchingSquares<T>::meshify( Layer<T>& layer )
{
	T* layerMap = layer.getMapping();

	int layerWidth = layer.getDimension().x;
	int layerHeight = layer.getDimension().y;



	Cell* cells = this->_grid->getCells();



	
	// Top
	for ( int x = 0; x < layerWidth; x++ )
	{
		for ( int y = 0; y < layerHeight; y++ )
		{
			if ( x == 0 || y == 0 )
			{
				// Do something special (or find borders if worldchunks are connected)
				//continue;
				layerMap[y * layerWidth + x].configuration = 15; // 15 is the all-solid configuration
			}
			else
			{
				layerMap[y * layerWidth + x].configuration = cells[( y - 1 ) * ( layerWidth - 1 ) + ( x - 1 )].configuration;//this->_meshMapping[y * layerWidth + x];
			}
		}
	}
	
	// Bottom
	/*
	for ( int x = 0; x < layerWidth - 1; x++ )
	{
		for ( int y = 0; y < layerHeight - 1; y++ )
		{
			if ( x == 0 || y == 0 )
			{
				// Do something special (or find borders if worldchunks are connected)
				//continue;
				layerMap[y * layerWidth + x].configuration = 15; // 15 is the all-solid configuration
			}
			else
			{
				layerMap[y * layerWidth + x].configuration = cells[y* ( layerWidth - 1 ) + x ].configuration;//this->_meshMapping[y * layerWidth + x];
			}
		}
	}
	*/




	return;
}




template<typename T>
void mesh::MarchingSquares<T>::update( Layer<T>& layer )
{
	
	olc::vi2d gridDimension = this->_grid->getDimension();
	int gridWidth = gridDimension.x;
	int gridHeight = gridDimension.y;

	//std::cout << gridWidth << gridHeight << std::endl;

	
	T* map = layer.getMapping();

	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			olc::vf2d position{ ( float )x, ( float )y }; // top left
			bool exist = map[y * gridWidth + x].exists(); // ensure that T has exists() method
			this->_grid->getControlIndicies()[y * gridWidth + x].exist = exist;// = ControlIndex( position, exist );
		}
	}

	// The reason for subtracting 1 is that the cells in the first rows and/or column do not have the neighbors
	// to assign all of topRight, topLeft, bottomRight, bottomLeft
	// Cells[0,0] is actually starting on Grid[1,1]

	Cell* cells = this->_grid->getCells();
	for ( int x = 0; x < ( gridWidth - 1 ); x++ )
	{
		for ( int y = 0; y < ( gridHeight - 1 ); y++ )
		{

			cells[y * ( gridWidth - 1 ) + x] = Cell(
					&this->_grid->getControlIndicies()[( y )*gridWidth + ( x )], // topLeft
					&this->_grid->getControlIndicies()[( y )*gridWidth + ( x + 1 )], // topRight
					&this->_grid->getControlIndicies()[( y + 1 ) * gridWidth + ( x + 1 )], // bottomRight
					&this->_grid->getControlIndicies()[( y + 1 ) * gridWidth + ( x )] ); // bottomLeft

		}
	}


	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			if ( x == 0 || y == 0 )
			{
				this->_meshMapping[y * gridWidth + x] = 15; // default solid block
			}
			else
			{
				this->_meshMapping[y * gridWidth + x] = cells[( y - 1 ) * ( gridWidth - 1 ) + ( x - 1 )].configuration;
			}
		}
	}


	/*
	for ( int x = 0; x < gridWidth - 1; x++ )
	{
		for ( int y = 0; y < gridHeight - 1; y++ )
		{
			this->triangulateCell( cells[y * ( gridWidth - 1 ) + x], x, y );
		}
	}
	*/
	
	
	this->meshify( layer );
}


/*

template<typename T>
void mesh::MarchingSquares<T>::triangulateCell( const mesh::Cell& square, int x, int y )
{
	int tileSize = 8;
	olc::vf2d controlNodeSize = -olc::vf2d{ ( float )tileSize, ( float )tileSize } *1.0f;

	switch ( square.configuration )
	{
		// 0 point
	case 0: // no mesh (empty)
		break;

		// 1 control node
	case 1:
		//pge->FillRect( olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f }, controlNodeSize, square.topLeft->exists ? olc::BLACK : olc::DARK_GREY ); // scale down the square to see center
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 1.0f, 0.0f } * tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 2:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 2.0f, 0.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 4:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 0.0f, 1.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 8:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 0.0f, 2.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;

		// 2 control node
	case 3:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 3.0f, 0.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 6:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 2.0f, 1.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 9:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 1.0f, 2.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 12:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 0.0f, 3.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 5:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 1.0f, 1.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 10:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 2.0f, 2.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;

		// 3 control node
	case 7:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 3.0f, 1.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 11:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 3.0f, 2.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 13:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 1.0f, 3.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	case 14:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 2.0f, 3.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;

		// 4 control node
	case 15:
		pge->DrawPartialDecal(
			olc::vf2d{ square.topLeft->position * tileSize - controlNodeSize / 2.0f },
			this->decalTile,
			olc::vf2d{ 3.0f, 3.0f } *tileSize,
			olc::vf2d{ ( float )tileSize, ( float )tileSize } );
		break;
	default:
		break;
	}
	return;
}
*/