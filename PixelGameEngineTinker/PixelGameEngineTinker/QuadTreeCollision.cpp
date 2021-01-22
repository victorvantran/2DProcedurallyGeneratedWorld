#include "QuadTreeCollision.h"
#include "World.h"





QuadTreeCollision::QuadTreeCollision() :
	_width( 0 ), _height( 0 ), _horizontalAreasCount( 0 ), _verticleAreasCount( 0 ),
	_objectsInArea(),
	_objectAreas(), _objectIndices(), _world( nullptr )
{

}

QuadTreeCollision::QuadTreeCollision( std::int64_t width, std::int64_t height, World* world ) :
	_width( width ), _height( height ),
	_horizontalAreasCount( ( std::uint32_t )std::ceil( ( long double )this->_width / ( long double )this->_gridWidth ) ), _verticleAreasCount( ( std::uint32_t )std::ceil( ( long double )this->_height / ( long double )this->_gridHeight ) ),
	_objectsInArea( std::vector<DynamicObject>{ this->_verticleAreasCount * this->_horizontalAreasCount } ),
	_objectAreas(),
	_objectIndices(),
	_world( world )
{

}


QuadTreeCollision::~QuadTreeCollision()
{

}






/*template <class T>
QuadTreeCollision<T>::QuadTreeCollision() :
	_width( 0 ), _height( 0 ), _horizontalAreasCount( 0 ), _verticleAreasCount( 0 ), _objectsInArea(), _objectAreas(), _objectIndices(), _world( nullptr )
{

}

template <class T>
QuadTreeCollision<T>::QuadTreeCollision( std::int64_t width, std::int64_t height, World* world ) :
	_width( width ), _height( height ),
	_horizontalAreasCount( ( std::int64_t )std::ceil( ( long double )this->_width / ( long double )this->_gridWidth ) ), _verticleAreasCount( ( std::int64_t )std::ceil( ( long double )this->_height / ( long double )this->_gridHeight ) ),
	_objectsInArea( std::vector<T>{ this->_verticleAreasCount* this->_horizontalAreasCount } ),
	_objectAreas(),
	_objectIndices(),
	_world( world )
{

}


template <class T>
QuadTreeCollision<T>::~QuadTreeCollision()
{

}

template <class T>
void QuadTreeCollision<T>::updateAreas( T object )
{
	return;
}
*/