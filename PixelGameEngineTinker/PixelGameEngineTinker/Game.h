#pragma once
#include "olcPixelGameEngine.h"
#include "GameState.h"
#include "Screen.h"

namespace olc {
	class Game
	{
	private:
		GameState _gameState;
		Screen *_pScreen;


	public:
		Game();
		~Game();

		void initializeGame();
		void destroyGame();

		void runGameStateLoading();
		void runGameStateTinker();

	};
}