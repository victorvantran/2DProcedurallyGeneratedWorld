#pragma once

#include <vector>
#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "DynamicObject.h" // [!] Forward declare?

class World;

class QuadTreeCollision
{
private:
	std::int64_t _width;
	std::int64_t _height;

	// [!] position

	static const std::int32_t _gridWidth = Settings::QuadTreeCollision::GRID_WIDTH;
	static const std::int32_t _gridHeight = Settings::QuadTreeCollision::GRID_HEIGHT;
	
	std::uint32_t _horizontalAreasCount; // col // [!] ceil positives, floor negatives
	std::uint32_t _verticleAreasCount; // row



	std::vector<DynamicObject> _objectsInArea;
	std::vector<std::int64_t> _objectAreas;
	std::vector<std::int64_t> _objectIndices;



	World* _world;


public:
	QuadTreeCollision();
	QuadTreeCollision( std::int64_t width, std::int64_t height, World* world );
	~QuadTreeCollision();



	void updateAreas( const DynamicObject& object )
	{
		olc::v2d_generic<long double> center = object.getAABB().getCenter();
		olc::v2d_generic<long double> topLeft = center;// +object.getAABB().getHalfSize();
		long double topRight;
		long double bottomLeft;
		long double bottomRight;
		return;
	}
	/*
	QuadTreeCollision() :
		_width( 0 ), _height( 0 ), _horizontalAreasCount( 0 ), _verticleAreasCount( 0 ), 
		//_objectsInArea(),
		_objectAreas(), _objectIndices(), _world( nullptr )
	{

	}

	QuadTreeCollision( std::int64_t width, std::int64_t height, World* world ) :
		_width( width ), _height( height ),
		_horizontalAreasCount( ( std::uint32_t )std::ceil( ( long double )this->_width / ( long double )this->_gridWidth ) ), _verticleAreasCount( ( std::uint32_t )std::ceil( ( long double )this->_height / ( long double )this->_gridHeight ) ),
		//_objectsInArea( std::vector<DynamicObject>{ this->_verticleAreasCount * this->_horizontalAreasCount } ),
		_objectAreas(),
		_objectIndices(),
		_world( world )
	{

	}


	~QuadTreeCollision()
	{

	}
	*/
	/*
	void updateAreas( const DynamicObject& object )
	{
		return;
	}
	*/

};