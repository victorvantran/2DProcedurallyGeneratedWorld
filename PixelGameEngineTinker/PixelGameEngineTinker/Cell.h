#pragma once
#include "olcPixelGameEngine.h"

#include "BoundingBox.h"



class Cell
{
protected:
	int _id;
	BoundingBox<int> _bounds;
	bool _exist;
	
	Cell();
	~Cell();

	Cell( int id, BoundingBox<int> _bounds, bool _exist );

public:
	int getId() const;
	BoundingBox<int> getBounds() const;
	int getX() const;
	int getY() const;
	int getWidth() const;
	int getHeight() const;
	bool getExist() const;



	int setId( int id );
	BoundingBox<int> setBounds( const BoundingBox<int>& _bounds );
	int setWidth( int width );
	int setHeight( int height );
	bool setExist( bool exist );


	void clear();
};

