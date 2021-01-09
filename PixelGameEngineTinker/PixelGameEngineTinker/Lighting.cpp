#include "Lighting.h"
/*
template<class T>
void Lighting<T>::calculateLights( const BoundingBox<long double>& cameraView, World* world )
{
	// Render every tileRender but only drawing one properlly scaled tile for each consolidated render

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;

	this->reset();

	for ( std::int16_t i = 0; i < Settings::World::NUM_CELLS_PER_CHUNK; i++ )
	{
		std::uint16_t x = i % chunkSize;
		std::uint16_t y = i / chunkSize;

		std::int64_t worldPosX = x + this->_chunkIndexX * chunkSize;
		std::int64_t worldPosY = y + this->_chunkIndexY * chunkSize;

		std::int64_t topLeftX = ( std::int64_t )std::floor( cameraView.getX() );
		std::int64_t topLeftY = ( std::int64_t )std::floor( cameraView.getY() );

		std::int64_t bottomRightX = ( std::int64_t )std::ceil( cameraView.getX() + cameraView.getWidth() );
		std::int64_t bottomRightY = ( std::int64_t )std::ceil( cameraView.getY() + cameraView.getHeight() );

		// Only render lights within camera frame
		if ( worldPosX >= topLeftX && worldPosX <= bottomRightX && worldPosY >= topLeftY && worldPosY <= bottomRightY )
		{
			int ne = ( y - 1 ) * chunkSize + ( x + 1 ); // Northerneastern neighbor
			int n = ( y - 1 ) * chunkSize + x; // Northern neighbor
			int nw = ( y - 1 ) * chunkSize + ( x - 1 ); // // Northwestern

			int e = y * chunkSize + ( x + 1 ); // Eastern neighbor
			int c = y * chunkSize + x; // Current tile
			int w = y * chunkSize + ( x - 1 ); // Western neighbor

			int se = ( y + 1 ) * chunkSize + ( x + 1 ); // Southeastern neighbor
			int s = ( y + 1 ) * chunkSize + x; // Southern neighbor
			int sw = ( y + 1 ) * chunkSize + ( x - 1 ); // Southwestern neighbor


			// Quadrant edge merge
			Light* neLight = ( ne < 0 || ne >= chunkSize ) ? world->getLight( worldPosX + 1, worldPosY - 1 ) : &this->_lights[ne];
			Light* nLight = ( n < 0 || n >= chunkSize ) ? world->getLight( worldPosX, worldPosY - 1 ) : &this->_lights[n];
			Light* nwLight = ( nw < 0 || nw >= chunkSize ) ? world->getLight( worldPosX - 1, worldPosY - 1 ) : &this->_lights[nw];
			Light* eLight = ( e < 0 || e >= chunkSize ) ? world->getLight( worldPosX + 1, worldPosY ) : &this->_lights[e];
			Light* cLight = &this->_lights[i];
			Light* wLight = ( w < 0 || w >= chunkSize ) ? world->getLight( worldPosX - 1, worldPosY ) : &this->_lights[w];
			Light* seLight = ( se < 0 || se >= chunkSize ) ? world->getLight( worldPosX + 1, worldPosY + 1 ) : &this->_lights[se];
			Light* sLight = ( s < 0 || s >= chunkSize ) ? world->getLight( worldPosX, worldPosY + 1 ) : &this->_lights[s];
			Light* swLight = ( sw < 0 || sw >= chunkSize ) ? world->getLight( worldPosX - 1, worldPosY + 1 ) : &this->_lights[sw];

			// Out of bounds check
			if ( neLight == nullptr || nLight == nullptr || nwLight == nullptr ||
				eLight == nullptr || wLight == nullptr ||
				seLight == nullptr || sLight == nullptr || swLight == nullptr
				)
			{
				continue;
			}

			std::uint8_t corner0R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getRed() + nwLight->getRed() + nLight->getRed() + wLight->getRed() ) / 4 ), 255 ) );
			std::uint8_t corner0G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getGreen() + nwLight->getGreen() + nLight->getGreen() + wLight->getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner0B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getBlue() + nwLight->getBlue() + nLight->getBlue() + wLight->getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner0A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getAlpha() + nwLight->getAlpha() + nLight->getAlpha() + wLight->getAlpha() ) / 4 ), 255 ) );

			std::uint8_t corner1R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getRed() + swLight->getRed() + sLight->getRed() + wLight->getRed() ) / 4 ), 255 ) );
			std::uint8_t corner1G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getGreen() + swLight->getGreen() + sLight->getGreen() + wLight->getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner1B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getBlue() + swLight->getBlue() + sLight->getBlue() + wLight->getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner1A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getAlpha() + swLight->getAlpha() + sLight->getAlpha() + wLight->getAlpha() ) / 4 ), 255 ) );

			std::uint8_t corner2R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getRed() + seLight->getRed() + sLight->getRed() + eLight->getRed() ) / 4 ), 255 ) );
			std::uint8_t corner2G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getGreen() + seLight->getGreen() + sLight->getGreen() + eLight->getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner2B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getBlue() + seLight->getBlue() + sLight->getBlue() + eLight->getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner2A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getAlpha() + seLight->getAlpha() + sLight->getAlpha() + eLight->getAlpha() ) / 4 ), 255 ) );

			std::uint8_t corner3R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getRed() + neLight->getRed() + nLight->getRed() + eLight->getRed() ) / 4 ), 255 ) );
			std::uint8_t corner3G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getGreen() + neLight->getGreen() + nLight->getGreen() + eLight->getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner3B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getBlue() + neLight->getBlue() + nLight->getBlue() + eLight->getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner3A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( cLight->getAlpha() + neLight->getAlpha() + nLight->getAlpha() + eLight->getAlpha() ) / 4 ), 255 ) );

			std::uint32_t corner0 = ( corner0R << 24 ) + ( corner0G << 16 ) + ( corner0B << 8 ) + ( corner0A );
			std::uint32_t corner1 = ( corner1R << 24 ) + ( corner1G << 16 ) + ( corner1B << 8 ) + ( corner1A );
			std::uint32_t corner2 = ( corner2R << 24 ) + ( corner2G << 16 ) + ( corner2B << 8 ) + ( corner2A );
			std::uint32_t corner3 = ( corner3R << 24 ) + ( corner3G << 16 ) + ( corner3B << 8 ) + ( corner3A );


			std::uint32_t corner0 = 0xff0000ff;
			std::uint32_t corner1 = 0xff0000ff;
			std::uint32_t corner2 = 0xff0000ff;
			std::uint32_t corner3 = 0xff0000ff;

			this->insertLightRenders( corner0, corner1, corner2, corner3, true, worldPosX, worldPosY, 1, 1 );
		}
	}
	return;
}
*/