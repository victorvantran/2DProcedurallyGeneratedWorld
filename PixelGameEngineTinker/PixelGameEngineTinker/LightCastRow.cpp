#include "LightCastRow.h"

LightCastRow::LightCastRow( long double depth_, long double startSlope_, long double endSlope_ ) : depth( depth_ ), startSlope( startSlope_ ), endSlope( endSlope_ ) {}


LightCastRow::~LightCastRow() {}


std::vector<olc::v2d_generic<long double>> LightCastRow::getTiles()
{
	std::vector< olc::v2d_generic<long double>> rowTiles;

	long double minCol = std::floor( ( this->depth * this->startSlope ) + 0.5 );
	long double maxCol = std::ceil( ( this->depth * this->endSlope ) - 0.5 );

	for ( long double col = minCol; col <= maxCol; col++ )
	{
		rowTiles.push_back( olc::v2d_generic<long double>{ col, this->depth } );
	}

	return rowTiles;
}


LightCastRow LightCastRow::getNext()
{
	return LightCastRow( this->depth + 1.0, this->startSlope, this->endSlope );
}
