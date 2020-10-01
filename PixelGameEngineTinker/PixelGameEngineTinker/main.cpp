#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "World.h"
#include "Tile.h"
#include "TileConsolidated.h"
#include "QuadTree.h"


#include "Assets.h"
#include "Camera.h"

// [!] probably one edge case that throws excpetion (need to find it eventually by running many test trials for the thorwn exception and videoing the smallest amount of inserts/removed to do it)

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	// [!] need to chagne absolute position and zoom of the camera in accordance to pixelSize
	const static int pixelSize = 1;
	const static int screenWidth = 1920 / pixelSize;
	const static int screenHeight = 1200 / pixelSize;
	const static int tileSize = 16 / pixelSize;


public:
	olc::Decal* decalTileMap = nullptr;
	olc::Decal* decalDirtTileConsolidationMap = nullptr;

	World world;
	Camera camera;

	olc::vi2d decalGridDimension;

	Tile* tiles; // exist, id, configuration
	int* configurations;

	olc::vi2d gridDimension;

	int tileId = 0;

private:


public:
	Example()
	{
		// Name you application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{


		createSpritesAndDecals();
		createConfigurations();
		createMap();


		return true;
	}

	bool OnUserUpdate( float fElapsedTime ) override
	{

		// Data Management Debug
		if ( GetKey( olc::Key::T ).bPressed )
		{
			world.saveWorldMap();
		}
		if ( GetKey( olc::Key::Y ).bPressed )
		{
			world.loadWorldMap();
		}
		if ( GetKey( olc::Key::U ).bPressed )
		{
			world.viewWorldMap();
		}



		// Camera Debug
		float mouseX = ( float )GetMouseX();
		float mouseY = ( float )GetMouseY();

		float panSpeed = 50.0f;
		if ( GetKey( olc::Key::UP ).bPressed || GetKey( olc::Key::UP ).bHeld )
		{
			camera.panY( -panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::DOWN ).bPressed || GetKey( olc::Key::DOWN ).bHeld )
		{
			camera.panY( panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::LEFT ).bPressed || GetKey( olc::Key::LEFT ).bHeld )
		{
			camera.panX( -panSpeed * fElapsedTime );
		}
		if ( GetKey( olc::Key::RIGHT ).bPressed || GetKey( olc::Key::RIGHT ).bHeld )
		{
			camera.panX( panSpeed * fElapsedTime );
		}


		if ( GetKey( olc::Key::F1 ).bPressed )
		{
			camera.panY( -panSpeed*100 );
		}
		if ( GetKey( olc::Key::F2 ).bPressed )
		{
			camera.panY( panSpeed*100 );
		}
		if ( GetKey( olc::Key::F3 ).bPressed )
		{
			camera.panX( -panSpeed*100 );
		}
		if ( GetKey( olc::Key::F4 ).bPressed )
		{
			camera.panX( panSpeed*100 );
		}






		// Zoom in
		if ( GetKey( olc::Key::Z ).bPressed || GetKey( olc::Key::Z ).bHeld )
		{
			float zoomScale = 1.0f + ( 1.0f * fElapsedTime );
			this->camera.zoom( zoomScale );
		}

		// Zoom out
		if ( GetKey( olc::Key::X ).bPressed || GetKey( olc::Key::X ).bHeld )
		{
			float zoomScale = 1.0f - ( 1.0f * fElapsedTime );
			this->camera.zoom( zoomScale );
		}

		if ( GetKey( olc::Key::D ).bPressed )
		{
			tileId = 0;
		}
		else if ( GetKey( olc::Key::S ).bPressed )
		{
			tileId = 1;
		}



		// Insert to world Debug 
		float tilePositionX;
		float tilePositionY;


		// [!] NEED TO FIX TO ACCOUNT FOR NEGATIVE ( LIKE HOW I DID FOR DELIMITER CAMERA )
		this->camera.screenToWorld( GetMouseX(), GetMouseY(), tilePositionX, tilePositionY );

		olc::vi2d tileIndex = olc::vi2d{
			( int )( tilePositionX ),
			( int )( tilePositionY )
		};


		if ( GetKey( olc::Key::P ).bPressed || GetKey( olc::Key::P ).bHeld )
		{
			//quadTree->insert( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 1, 1 ), true ) );
			world.insert( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 32, 32*32 ), true ) );
			//updateTileConfiguration( tileIndex, true );
		}

		if ( GetMouse( 0 ).bPressed || GetMouse( 0 ).bHeld )
			//if ( GetMouse( 0 ).bPressed )
		{
			world.insert( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 5, 5 ), true ) );

			//quadTree->insert( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 5, 5 ), true ) );
			//updateTileConfiguration( tileIndex, true );
		}

		if ( GetMouse( 1 ).bPressed || GetMouse( 1 ).bHeld )
			//if ( GetMouse( 1 ).bPressed )
		{
			world.remove( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 5, 5 ), true ) );
			//quadTree->remove( TileConsolidated( tileId, BoundingBox<int>( tileIndex.x, tileIndex.y, 5, 5 ), true ) );
			//updateTileConfiguration( tileIndex, true );
		}

		if ( GetKey( olc::Key::SPACE ).bPressed || GetKey( olc::Key::SPACE ).bHeld )
		{
			//std::cout << countQuadTree( 0 ) << std::endl;

		}




		Clear( olc::BLACK );

		//drawAllSingleTiles( this->tiles );
		//drawQuadTree( this->quadTrees, 0, this->camera );
		this->world.delimitWorldChunks( this->camera.getView() );
		this->camera.renderWorld( this->world );
		this->camera.renderCamera();



		drawTileIndexString( tileIndex );

		return true;
	}


	void createMap()
	{
		//this->world = World(); // stop calling stuff twice
		world.loadWorldMap();

		int screenCellWidth = screenWidth / tileSize;
		int screenCellHeight = screenHeight / tileSize;
		//camera = Camera( BoundingBox<float>( 0.0f, 0.0f, screenCellWidth, screenCellHeight ), 1.0f, 1.0f );
		this->camera = Camera( BoundingBox<float>( 0.0f, 0.0f, 32, 32 ), 1.0f, 1.0f );

		this->world.initializeDelimits( this->camera.getView() );
		this->world.initializeWorldChunks();

	}



	void createConfigurations()
	{
		decalGridDimension = olc::vi2d{ 8, 8 }; // x*y = #of members in a single family

		// initially calculate all configurations
		for ( int x = 0; x < gridDimension.x; x++ )
		{
			for ( int y = 0; y < gridDimension.y; y++ )
			{
				updateTileConfiguration( olc::vi2d{ x, y }, true );
			}
		}

		configurations = new int[256];
		configurations[0] = 0;		configurations[1] = 0;		configurations[2] = 1;		configurations[3] = 1;		configurations[4] = 0;		configurations[5] = 0;		configurations[6] = 1;		configurations[7] = 1;
		configurations[8] = 2;		configurations[9] = 2;		configurations[10] = 3;		configurations[11] = 3;		configurations[12] = 2;		configurations[13] = 2;		configurations[14] = 4;		configurations[15] = 4;
		configurations[16] = 0;		configurations[17] = 0;		configurations[18] = 1;		configurations[19] = 1;		configurations[20] = 0;		configurations[21] = 0;		configurations[22] = 1;		configurations[23] = 1;
		configurations[24] = 2;		configurations[25] = 2;		configurations[26] = 3;		configurations[27] = 3;		configurations[28] = 2;		configurations[29] = 2;		configurations[30] = 4;		configurations[31] = 4;
		configurations[32] = 5;		configurations[33] = 5;		configurations[34] = 6;		configurations[35] = 6;		configurations[36] = 5;		configurations[37] = 5;		configurations[38] = 6;		configurations[39] = 6;
		configurations[40] = 7;		configurations[41] = 7;		configurations[42] = 8;		configurations[43] = 8;		configurations[44] = 7;		configurations[45] = 7;		configurations[46] = 9;		configurations[47] = 9;
		configurations[48] = 5;		configurations[49] = 5;		configurations[50] = 6;		configurations[51] = 6;		configurations[52] = 5;		configurations[53] = 5;		configurations[54] = 6;		configurations[55] = 6;
		configurations[56] = 10;	configurations[57] = 10;	configurations[58] = 11;	configurations[59] = 11;	configurations[60] = 10;	configurations[61] = 10;	configurations[62] = 12;	configurations[63] = 12;
		configurations[64] = 0;		configurations[65] = 0;		configurations[66] = 1;		configurations[67] = 1;		configurations[68] = 0;		configurations[69] = 0;		configurations[70] = 1;		configurations[71] = 1;
		configurations[72] = 2;		configurations[73] = 2;		configurations[74] = 3;		configurations[75] = 3;		configurations[76] = 2;		configurations[77] = 2;		configurations[78] = 4;		configurations[79] = 4;
		configurations[80] = 0;		configurations[81] = 0;		configurations[82] = 1;		configurations[83] = 1;		configurations[84] = 0;		configurations[85] = 0;		configurations[86] = 1;		configurations[87] = 1;
		configurations[88] = 2;		configurations[89] = 2;		configurations[90] = 3;		configurations[91] = 3;		configurations[92] = 2;		configurations[93] = 2;		configurations[94] = 4;		configurations[95] = 4;
		configurations[96] = 5;		configurations[97] = 5;		configurations[98] = 6;		configurations[99] = 6;		configurations[100] = 5;	configurations[101] = 5;	configurations[102] = 6;	configurations[103] = 6;
		configurations[104] = 7;	configurations[105] = 7;	configurations[106] = 8;	configurations[107] = 8;	configurations[108] = 7;	configurations[109] = 7;	configurations[110] = 9;	configurations[111] = 9;
		configurations[112] = 5;	configurations[113] = 5;	configurations[114] = 6;	configurations[115] = 6;	configurations[116] = 5;	configurations[117] = 5;	configurations[118] = 6;	configurations[119] = 6;
		configurations[120] = 10;	configurations[121] = 10;	configurations[122] = 11;	configurations[123] = 11;	configurations[124] = 10;	configurations[125] = 10;	configurations[126] = 12;	configurations[127] = 12;
		configurations[128] = 14;	configurations[129] = 14;	configurations[130] = 13;	configurations[131] = 16;	configurations[132] = 14;	configurations[133] = 14;	configurations[134] = 13;	configurations[135] = 16;
		configurations[136] = 17;	configurations[137] = 17;	configurations[138] = 15;	configurations[139] = 18;	configurations[140] = 17;	configurations[141] = 17;	configurations[142] = 19;	configurations[143] = 20;
		configurations[144] = 14;	configurations[145] = 14;	configurations[146] = 13;	configurations[147] = 16;	configurations[148] = 14;	configurations[149] = 14;	configurations[150] = 13;	configurations[151] = 16;
		configurations[152] = 17;	configurations[153] = 17;	configurations[154] = 15;	configurations[155] = 18;	configurations[156] = 17;	configurations[157] = 17;	configurations[158] = 19;	configurations[159] = 20;
		configurations[160] = 21;	configurations[161] = 21;	configurations[162] = 22;	configurations[163] = 23;	configurations[164] = 21;	configurations[165] = 21;	configurations[166] = 22;	configurations[167] = 23;
		configurations[168] = 24;	configurations[169] = 24;	configurations[170] = 25;	configurations[171] = 26;	configurations[172] = 24;	configurations[173] = 24;	configurations[174] = 27;	configurations[175] = 28;
		configurations[176] = 21;	configurations[177] = 21;	configurations[178] = 22;	configurations[179] = 23;	configurations[180] = 21;	configurations[181] = 21;	configurations[182] = 22;	configurations[183] = 23;
		configurations[184] = 29;	configurations[185] = 29;	configurations[186] = 30;	configurations[187] = 31;	configurations[188] = 29;	configurations[189] = 29;	configurations[190] = 32;	configurations[191] = 33;
		configurations[192] = 14;	configurations[193] = 14;	configurations[194] = 13;	configurations[195] = 16;	configurations[196] = 14;	configurations[197] = 14;	configurations[198] = 13;	configurations[199] = 16;
		configurations[200] = 17;	configurations[201] = 17;	configurations[202] = 15;	configurations[203] = 18;	configurations[204] = 17;	configurations[205] = 17;	configurations[206] = 19;	configurations[207] = 20;
		configurations[208] = 14;	configurations[209] = 14;	configurations[210] = 13;	configurations[211] = 16;	configurations[212] = 14;	configurations[213] = 14;	configurations[214] = 13;	configurations[215] = 16;
		configurations[216] = 17;	configurations[217] = 17;	configurations[218] = 15;	configurations[219] = 18;	configurations[220] = 17;	configurations[221] = 17;	configurations[222] = 19;	configurations[223] = 20;
		configurations[224] = 34;	configurations[225] = 34;	configurations[226] = 35;	configurations[227] = 36;	configurations[228] = 34;	configurations[229] = 34;	configurations[230] = 35;	configurations[231] = 36;
		configurations[232] = 37;	configurations[233] = 37;	configurations[234] = 38;	configurations[235] = 39;	configurations[236] = 37;	configurations[237] = 37;	configurations[238] = 40;	configurations[239] = 41;
		configurations[240] = 34;	configurations[241] = 34;	configurations[242] = 35;	configurations[243] = 36;	configurations[244] = 34;	configurations[245] = 34;	configurations[246] = 35;	configurations[247] = 36;
		configurations[248] = 42;	configurations[249] = 42;	configurations[250] = 43;	configurations[251] = 44;	configurations[252] = 42;	configurations[253] = 42;	configurations[254] = 45;	configurations[255] = 46;

		return;
	}


	void createSpritesAndDecals()
	{
		Assets::get().loadSprites();
		Assets::get().loadDecals();

		decalTileMap = Assets::get().getDecal( "ForestTileMap" );
		decalDirtTileConsolidationMap = Assets::get().getDecal( "ForestConsolidationTileMap" );

		return;
	}


	void updateTileConfiguration( olc::vi2d tileIndex, bool origin )
	{
		if ( !( tileIndex.x >= 0 && tileIndex.x < gridDimension.x &&
			tileIndex.y >= 0 && tileIndex.y < gridDimension.y &&
			( origin || tiles[tileIndex.y * gridDimension.x + tileIndex.x].getExist() ) ) )
		{
			return;
		}

		int tileId = tiles[tileIndex.y * gridDimension.x + tileIndex.x].getId();

		olc::vi2d topLeftIndex = olc::vi2d{ tileIndex.x - 1, tileIndex.y - 1 };
		olc::vi2d centerTopIndex = olc::vi2d{ tileIndex.x, tileIndex.y - 1 };
		olc::vi2d topRightIndex = olc::vi2d{ tileIndex.x + 1, tileIndex.y - 1 };
		olc::vi2d centerRightIndex = olc::vi2d{ tileIndex.x + 1, tileIndex.y };
		olc::vi2d bottomRightIndex = olc::vi2d{ tileIndex.x + 1, tileIndex.y + 1 };
		olc::vi2d centerBottomIndex = olc::vi2d{ tileIndex.x, tileIndex.y + 1 };
		olc::vi2d bottomLeftIndex = olc::vi2d{ tileIndex.x - 1, tileIndex.y + 1 };
		olc::vi2d centerLeftIndex = olc::vi2d{ tileIndex.x - 1, tileIndex.y };


		int configuration = 0;

		if ( topLeftIndex.x >= 0 && topLeftIndex.x < gridDimension.x &&
			topLeftIndex.y >= 0 && topLeftIndex.y < gridDimension.y &&
			tiles[topLeftIndex.y * gridDimension.x + topLeftIndex.x].getExist() &&
			tileId == tiles[topLeftIndex.y * gridDimension.x + topLeftIndex.x].getId()
			)
		{
			configuration += 1;
		}
		if ( centerTopIndex.x >= 0 && centerTopIndex.x < gridDimension.x &&
			centerTopIndex.y >= 0 && centerTopIndex.y < gridDimension.y &&
			tiles[centerTopIndex.y * gridDimension.x + centerTopIndex.x].getExist() &&
			tileId == tiles[centerTopIndex.y * gridDimension.x + centerTopIndex.x].getId()
			)
		{
			configuration += 2;
		}
		if ( topRightIndex.x >= 0 && topRightIndex.x < gridDimension.x &&
			topRightIndex.y >= 0 && topRightIndex.y < gridDimension.y &&
			tiles[topRightIndex.y * gridDimension.x + topRightIndex.x].getExist() &&
			tileId == tiles[topRightIndex.y * gridDimension.x + topRightIndex.x].getId()

			)
		{
			configuration += 4;
		}
		if ( centerRightIndex.x >= 0 && centerRightIndex.x < gridDimension.x &&
			centerRightIndex.y >= 0 && centerRightIndex.y < gridDimension.y &&
			tiles[centerRightIndex.y * gridDimension.x + centerRightIndex.x].getExist() &&
			tileId == tiles[centerRightIndex.y * gridDimension.x + centerRightIndex.x].getId()
			)
		{
			configuration += 8;
		}
		if ( bottomRightIndex.x >= 0 && bottomRightIndex.x < gridDimension.x &&
			bottomRightIndex.y >= 0 && bottomRightIndex.y < gridDimension.y &&
			tiles[bottomRightIndex.y * gridDimension.x + bottomRightIndex.x].getExist() &&
			tileId == tiles[bottomRightIndex.y * gridDimension.x + bottomRightIndex.x].getId()
			)
		{
			configuration += 16;
		}
		if ( centerBottomIndex.x >= 0 && centerBottomIndex.x < gridDimension.x &&
			centerBottomIndex.y >= 0 && centerBottomIndex.y < gridDimension.y &&
			tiles[centerBottomIndex.y * gridDimension.x + centerBottomIndex.x].getExist() &&
			tileId == tiles[centerBottomIndex.y * gridDimension.x + centerBottomIndex.x].getId()
			)
		{
			configuration += 32;
		}
		if ( bottomLeftIndex.x >= 0 && bottomLeftIndex.x < gridDimension.x &&
			bottomLeftIndex.y >= 0 && bottomLeftIndex.y < gridDimension.y &&
			tiles[bottomLeftIndex.y * gridDimension.x + bottomLeftIndex.x].getExist() &&
			tileId == tiles[bottomLeftIndex.y * gridDimension.x + bottomLeftIndex.x].getId()
			)
		{
			configuration += 64;
		}
		if ( centerLeftIndex.x >= 0 && centerLeftIndex.x < gridDimension.x &&
			centerLeftIndex.y >= 0 && centerLeftIndex.y < gridDimension.y &&
			tiles[centerLeftIndex.y * gridDimension.x + centerLeftIndex.x].getExist() &&
			tileId == tiles[centerLeftIndex.y * gridDimension.x + centerLeftIndex.x].getId()
			)
		{
			configuration += 128;
		}

		if ( origin )
		{
			updateTileConfiguration( topLeftIndex, false );
			updateTileConfiguration( centerTopIndex, false );
			updateTileConfiguration( topRightIndex, false );
			updateTileConfiguration( centerRightIndex, false );
			updateTileConfiguration( bottomRightIndex, false );
			updateTileConfiguration( centerBottomIndex, false );
			updateTileConfiguration( bottomLeftIndex, false );
			updateTileConfiguration( centerLeftIndex, false );
		}

		// tiles[tileIndex.y * gridDimension.x + tileIndex.x].setConfiguration( configurations[configuration] ); // [!] adjust sizing 
		return;
	}

	/*
	int countQuadTree( int quadTreeIndex, int counter = 0 )
	{
		if ( this->quadTrees[quadTreeIndex].isConsolidated() )
		{
			return 0; // the whole consolidated bounds is accounted for counted by its parent
		}
		else
		{
			counter = this->quadTrees[quadTreeIndex].getCellCount();
		}

		int level = this->quadTrees[quadTreeIndex].getLevel();
		if ( level > QuadTree<Tile, TileConsolidated>::_MIN_LEVELS )
			//int* childrenNodeIndicies = this->quadTrees[quadTreeIndex].getChildrenNodeIndicies();
			//if ( childrenNodeIndicies != nullptr )
		{
			int* childrenNodeIndicies = this->quadTrees[quadTreeIndex].getChildrenNodeIndicies();
			for ( int i = 0; i < 4; i++ )
			{
				if ( childrenNodeIndicies[i] != -1 )
				{
					counter += countQuadTree( childrenNodeIndicies[i] );
				}
			}
		}

		return counter;
	}
	*/

	void drawAllSingleTiles( Tile* tiles )
	{
		int rootQuadTreeSize = 2 << QuadTree<Tile, TileConsolidated>::_MAX_LEVELS;

		for ( int x = 0; x < rootQuadTreeSize; x++ )
		{
			for ( int y = 0; y < rootQuadTreeSize; y++ )
			{
				Tile tile = tiles[y * rootQuadTreeSize + x];
				if ( tile.getExist() )
				{
					olc::vi2d decalSourcePos = olc::vi2d{ tile.getId() == 0 ? 7 : 15, 7 }; // dirt, stone

					DrawPartialDecal(
						olc::vi2d{ tile.getX(), tile.getY() } *tileSize,
						decalTileMap,
						olc::vi2d{ decalSourcePos }  *tileSize * pixelSize,
						olc::vi2d{ 1, 1 } *tileSize,
						olc::vf2d{ 1.0f,1.0f }
					);
				}
			}
		}

		return;
	}


	void drawTileIndexString( const olc::vi2d& tileIndex )
	{
		DrawStringDecal(
			olc::vi2d( GetMouseX(), GetMouseY() ),
			"[" + std::to_string( tileIndex.x ) + "," + std::to_string( -tileIndex.y ) + "]",
			olc::WHITE,
			olc::vf2d( 2.0f, 2.0f )
		);
	}
};



int main()
{
	Example demo;
	//if ( demo.Construct( 1920, 1200, 1, 1 ) )
	//if ( demo.Construct( 1920/2, 1200/2, 2, 2 ) )
	if ( demo.Construct( Example::screenWidth, Example::screenHeight, Example::pixelSize, Example::pixelSize ) )
		//if ( demo.Construct( 768, 432, 1, 1 ) )
		//if ( demo.Construct( 960, 600, 1, 1 ) )
	//if ( demo.Construct( 1600, 900, 1, 1 ) )
		demo.Start();
	return 0;
}