#pragma once

#include "olcPixelGameEngine.h"

class Animator
{
protected:
	olc::Sprite* _sprite;
	olc::Decal* _decal;

	float _localTime;
	
	static constexpr float TICK = 60.0f;
	static constexpr float REFRESH_RATE = ( 1 / TICK );
public:
	Animator() : _sprite( nullptr ), _decal( nullptr ), _localTime( 0 ) {}
	Animator( olc::Sprite* sprite, olc::Decal* decal ) : _sprite( sprite ), _decal( decal ), _localTime( 0 ) {}
	~Animator() {}
};