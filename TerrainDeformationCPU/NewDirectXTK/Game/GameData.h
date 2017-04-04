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
#include "camera.h"
#include "Circle.h"

using namespace DirectX;
using namespace SimpleMath;

// For demonstration input purposes
enum ClickState
{
	IMPRINT = 0,
	EXPLODE = 1,
	RAIN = 2,
	SNOW = 3
};
// For demonstration input purposes
enum DisplayState
{
	HEIGHT = 0,
	SOFTNESS = 1,
	DISPLACEMENT
};

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_ImmediateContext;

	//player input
	unsigned char* m_keyboardState; //current state of the Keyboard
	unsigned char* m_prevKeyboardState; //previous frame's state of the keyboard
	DIMOUSESTATE* m_mouseState; //current state of the mouse
	DIMOUSESTATE* m_prevMouseState; //previous state of the mouse

	HWND* m_hwnd; // Demontration Input usage for window size

	Circle* m_Circle; // Demontration Input usage for displaying input
	ClickState m_clickState; // Demontration Input for the type of input
	DisplayState m_displyState; // Demontration Input for the display of the terrain type
};
#endif
