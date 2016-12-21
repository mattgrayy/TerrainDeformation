#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================
#include <dinput.h>
#include "Keyboard.h"
#include "Mouse.h"
#include "GameState.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace SimpleMath;

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState

	ID3D11Device* m_pd3dDevice;

	//player input
	unsigned char* m_keyboardState; //current state of the Keyboard
	unsigned char* m_prevKeyboardState; //previous frame's state of the keyboard
	DIMOUSESTATE* m_mouseState; //current state of the mouse
	DIMOUSESTATE* m_prevMouseState; //previous state of the mouse

	HWND* m_hwnd;
};
#endif
