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
	};






	template<typename T> // Example: Tile
	class MarchingSquares
	{
	private:
		Grid<T>* _grid;
		int* _mesh; // An array of integers that represent the configuration/id of a tile. Only purpose is to render. Nothing else about it is interactable. That is reserved for the Tile
		
	public:

		MarchingSquares();
		~MarchingSquares();

		
		void generateMeshMapping( T* map, const olc::vi2d& mapDimension );
		// void generate( layer );
		void destroy();

		int* getMeshMapping();
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
	if ( this->topLeft->exist )
	{
		this->configuration += 2;
	}
	if ( this->topLeft->exist )
	{
		this->configuration += 4;
	}
	if ( this->topLeft->exist )
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
			bool exist = map[y * gridWidth + x].exists(); // ensure that T has exists() method
			this->_controlIndicies[y * gridWidth + x] = ControlIndex( position, exist );
		}
	}

	// The reason for subtracting 1 is that the cells in the first rows and/or column do not have the neighbors
	// to assign all of topRight, topLeft, bottomRight, bottomLeft
	// Cells[0,0] is actually starting on Grid[1,1]
	this->_cells = new Cells[( gridWidth - 1 ) * ( gridHeight - 1 )];
	for ( int x = 0; x < ( gridWidth - 1 ); x++ )
	{
		for ( int y = 0; y < ( gridHeight - 1 ); y++ )
		{
			this->_cells[y * ( gridWidth - 1 ) + x] = Cell( 
				&controlIndicies[( y ) * ( gridWidth - 1 ) + ( x )], // topLeft
				&controlIndicies[( y ) * ( gridWidth - 1 ) + ( x + 1 )], // topRight
				&controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x + 1)], // bottomRight
				&controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x )]); // bottomLeft
		}
	}
}

template<typename T>
mesh::Grid<T>::Grid( T* map, int gridWidth, int gridHeight )
{
	// Mark whether a control index exists (for example, has a tile located on it)
	this->_controlIndicies = new ControlIndex[gridWidth * gridHeight];
	for ( int x = 0; x < gridWidth; x++ )
	{
		for ( int y = 0; y < gridHeight; y++ )
		{
			olc::vf2d position{ ( float )x, ( float )y }; // top left
			bool exist = map[y * gridWidth + x].exists(); // ensure that T has exists() method
			this->_controlIndicies[y * gridWidth + x] = ControlIndex( position, exist );
		}
	}

	// The reason for subtracting 1 is that the cells in the first rows and/or column do not have the neighbors
	// to assign all of topRight, topLeft, bottomRight, bottomLeft
	// Cells[0,0] is actually starting on Grid[1,1]
	this->_cells = new Cells[( gridWidth - 1 ) * ( gridHeight - 1 )];
	for ( int x = 0; x < ( gridWidth - 1 ); x++ )
	{
		for ( int y = 0; y < ( gridHeight - 1 ); y++ )
		{
			this->_cells[y * ( gridWidth - 1 ) + x] = Cell(
				&controlIndicies[( y ) * ( gridWidth - 1 ) + ( x )], // topLeft
				&controlIndicies[( y ) * ( gridWidth - 1 ) + ( x + 1 )], // topRight
				&controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x + 1 )], // bottomRight
				&controlIndicies[( y + 1 ) * ( gridWidth - 1 ) + ( x )] ); // bottomLeft
		}
	}
}



// MarchingSquares
template<typename T>
mesh::MarchingSquares<T>::MarchingSquares()
{
	this->_grid = nullptr;
	this->_mesh = nullptr;
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
	this->_meshMapping = new int[ ( gridWidth - 1 ) * ( gridHeight - 1 ) ]; // to be deleted by the Mesh, so must be assigned to a mesh

	int gridWidth = mapDimension.x;
	int gridHeight = mapDimension.y;

	for ( int x = 0; x < gridWidth - 1; x++ )
	{
		for ( int y = 0; y < gridHeight - 1; y++ )
		{
			this->_meshMapping[y * ( gridWidth - 1 ) + x] = this->_grid->_cells[y * ( gridWidth - 1 ) + x].configuration;
		}
	}

	return;
}

// generate ( World Chunk )

template<typename T>
void mesh::MarchingSquares<T>::destroy()
{
	delete this->_grid;
	this->_grid = nullptr;
	return;
}

// generate ( World Chunk )

template<typename T>
int* mesh::MarchingSquares<T>::getMeshMapping()
{
	return this->_meshMapping;
}