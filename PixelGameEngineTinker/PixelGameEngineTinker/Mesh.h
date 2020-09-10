#pragma once
#include "olcPixelGameEngine.h"

namespace mesh
{
	struct Point
	{
		olc::vf2d position;

		Point();
		~Point();

		Point( const olc::vf2d& position );
	};


	struct ControlCell : public Point
	{
		// A cell with two additional pointers to points that are "halfway" between two cells
		// Up and Right
		Point* pointUp;
		Point* pointRight;
		bool exist;

		ControlCell();
		~ControlCell();

		ControlCell( const olc::vf2d& position, bool exist );
	};


	class Mesh
	{
	private:

	public:

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


// ControlCell
mesh::ControlCell::ControlCell()
{

}

mesh::ControlCell::~ControlCell()
{

}

mesh::ControlCell::ControlCell( const olc::vf2d& position, bool exist ) : Point( position )
{
	this->exist = exist;
	this->pointUp = new Point( position + olc::vf2d{ 0.0f, -0.5f } ); // To be deleted by the grid
	this->pointRight = new Point( position + olc::vf2d{ 0.5f, 0.0f } ); // To be deleted by the grid
}

