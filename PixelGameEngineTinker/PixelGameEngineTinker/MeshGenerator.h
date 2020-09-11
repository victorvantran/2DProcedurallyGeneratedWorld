#pragma once

#include "olcPixelGameEngine.h"
#include "Tile.h"
class MeshGenerator : public olc::PGEX
{
public:
	class Node
	{
	private:
	public:
		olc::vf2d position;
		int vertexIndex = -1;

		Node()
		{

		}

		Node( const olc::vf2d& position )
		{
			this->position = position;
		}

		~Node()
		{

		}
	};


	class ControlNode : public Node
	{
	private:
	public:
		Node* nodeUp;
		Node* nodeRight;
		bool exists;

		ControlNode()
		{
			this->nodeUp = nullptr;
			this->nodeRight = nullptr;
			this->exists = false;
		}

		~ControlNode()
		{
		}

		ControlNode( olc::vf2d position, bool exists ) : Node( position )
		{
			this->exists = exists;
			this->nodeUp = new Node( position + olc::vf2d{ 0.0f, -0.5f } );
			this->nodeRight = new Node( position + olc::vf2d{ 0.5f, 0.0f } );
		}
	};


	class Square
	{
	private:
	public:
		ControlNode* topLeft;
		ControlNode* topRight;
		ControlNode* bottomRight;
		ControlNode* bottomLeft;

		Node* centerTop;
		Node* centerRight;
		Node* centerBottom;
		Node* centerLeft;

		int configuration; // 16 possible square configurations

		Square()
		{
			this->topLeft = nullptr;
			this->topRight = nullptr;
			this->bottomRight = nullptr;
			this->bottomLeft = nullptr;

			this->centerTop = nullptr;
			this->centerRight = nullptr;
			this->centerBottom = nullptr;
			this->centerLeft = nullptr;

			this->configuration = 0;
		}

		~Square()
		{
		}

		Square( ControlNode* topLeft, ControlNode* topRight, ControlNode* bottomRight, ControlNode* bottomLeft )
		{
			this->topLeft = topLeft;
			this->topRight = topRight;
			this->bottomRight = bottomRight;
			this->bottomLeft = bottomLeft;

			this->centerTop = topLeft->nodeRight;
			this->centerRight = bottomRight->nodeUp;
			this->centerBottom = bottomLeft->nodeRight;
			this->centerLeft = bottomLeft->nodeUp;

			this->configuration = 0;
			if ( this->topLeft->exists )
			{
				this->configuration += 1;
			}
			if ( this->topRight->exists )
			{
				this->configuration += 2;
			}
			if ( this->bottomRight->exists )
			{
				this->configuration += 4;
			}
			if ( this->bottomLeft->exists )
			{
				this->configuration += 8;
			}

		}
	};



	class SquareGrid
	{
	private:
	public:
		Square* squares;
		ControlNode* controlNodes;

		int gridWidth;
		int gridHeight;


		SquareGrid()
		{
			this->squares = nullptr;
			this->controlNodes = nullptr;
			this->gridWidth = 0;
			this->gridHeight = 0;
		}

		~SquareGrid()
		{
			for ( int x = 0; x < this->gridWidth; x++ )
			{
				for ( int y = 0; y < this->gridHeight; y++ )
				{
					delete this->controlNodes[y * gridWidth + x].nodeUp;
					delete this->controlNodes[y * gridWidth + x].nodeRight;

					this->controlNodes[y * gridWidth + x].nodeUp = nullptr;
					this->controlNodes[y * gridWidth + x].nodeRight = nullptr;
				}
			}


			delete[] this->squares;
			this->squares = nullptr;

			delete[] this->controlNodes;
			this->controlNodes = nullptr;
		}


		SquareGrid( Tile map[], int gridWidth, int gridHeight )
		{
			this->gridWidth = gridWidth;
			this->gridHeight = gridHeight;

			this->controlNodes = new ControlNode[gridWidth * gridHeight];
			for ( int x = 0; x < gridWidth; x++ )
			{
				for ( int y = 0; y < gridHeight; y++ )
				{
					olc::vf2d position{ ( float )x, ( float )y }; // add 0.5f to center it
					bool exist = map[y * gridWidth + x].exists(); //true; // [!] based on map[y * layerWidth + x]'s normalized perlin value. ARbritray 0.5f for now
					this->controlNodes[y * gridWidth + x] = ControlNode( position, exist );
				}
			}

			this->squares = new Square[( gridWidth - 1 ) * ( gridHeight - 1 )]; // one less square than there are nodes per final row and column due to neighbors not existing
			for ( int x = 0; x < gridWidth - 1; x++ )
			{
				for ( int y = 0; y < gridHeight - 1; y++ )
				{
					this->squares[y * ( gridWidth - 1 ) + x] = Square( &controlNodes[( y )*gridWidth + x], &controlNodes[( y )*gridWidth + ( x + 1 )], &controlNodes[( y + 1 ) * gridWidth + ( x + 1 )], &controlNodes[( y + 1 ) * gridWidth + x] );

				}
			}

		}
	};


private:

public:
	SquareGrid* squareGrid;
	int* meshMapping;
	/*
	std::vector<olc::vf2d> verticies;
	std::vector<int> triangles;
	*/
	int gridWidth;
	int gridHeight;


	MeshGenerator()
	{
		this->squareGrid = nullptr;
		this->meshMapping = nullptr;
		/*
		this->verticies = {};
		this->triangles = {};
		*/
		this->gridWidth = 0;
		this->gridHeight = 0;
	}

	~MeshGenerator()
	{
		delete this->squareGrid;
		this->squareGrid = nullptr;
	}


	void generate( Tile map[], int layerWidth, int layerHeight) // 2d array of type instead of float
	{
		this->gridWidth = layerWidth;
		this->gridHeight = layerHeight;
		this->squareGrid = new SquareGrid( map, layerWidth, layerHeight ); // can now just do SquareGrid( map )

		this->meshMapping = new int[layerWidth * layerHeight];
		/*
		this->verticies = {};
		this->triangles = {};
		*/


		for ( int x = 0; x < gridWidth; x++ )
		{
			for ( int y = 0; y < gridHeight; y++ )
			{
				if ( x == 0 || y == 0 )
				{
					this->meshMapping[y * this->gridWidth + x] = 15;
				}
				else
				{
					this->meshMapping[y * this->gridWidth + x] = this->squareGrid->squares[(y - 1) * ( this->gridWidth - 1 ) + (x - 1)].configuration;

				}
			}
		}
	}

	void meshify( Layer<Tile>& layer )
	{
		Tile* layerMap = layer.getMapping();

		int layerWidth = layer.getDimension().x;
		int layerHeight = layer.getDimension().y;

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
					layerMap[y * layerWidth + x].configuration = 1;
				}
			}
		}
		return;
	}


	int* getMapping()
	{
		return this->meshMapping;
	}

};