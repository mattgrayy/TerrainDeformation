#include "game.h"
#include "GameData.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include "gameobject.h"
#include "ObjectList.h"
#include "helper.h"
#include <windows.h>
#include <time.h>
#include "DDSTextureLoader.h"
#include <d3d11shader.h>

using namespace DirectX;

Game::Game(ID3D11Device* _pd3dDevice, HWND _hWnd, HINSTANCE _hInstance) :m_playTime(0), m_fxFactory(nullptr), m_states(nullptr)
{
	//Create DirectXTK spritebatch
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));

	//seed the random number generator
	srand((UINT)time(NULL));	

	//Direct Input
	m_hWnd = _hWnd;
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	hr = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//create GameData struct and populate its pointers
	m_GD = new GameData;
	m_GD->m_keyboardState = m_keyboardState;
	m_GD->m_prevKeyboardState = m_prevKeyboardState;
	m_GD->m_GS = GS_PLAY_MAIN_CAM;
	m_GD->m_mouseState = &m_mouseState;
	m_GD->m_prevMouseState = &m_prevMouseState;
	m_GD->m_pd3dDevice = _pd3dDevice;

	m_GD->m_hwnd = &m_hWnd;

	//set up DirectXTK Effects system
	m_fxFactory = new EffectFactory(_pd3dDevice);

	//Tell the fxFactory to look to the correct build directory to pull in from
#ifdef DEBUG
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Debug");
#else
	((EffectFactory*)m_fxFactory)->SetDirectory(L"../Release");
#endif

	// Create other render resources here
	m_states = new CommonStates(_pd3dDevice);

	//init render system for VBGOs
	VBGO::Init(_pd3dDevice);

	//find how big my window is to correctly calculate my aspect ratio
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	float AR = (float)width / (float)height;

	//create a base light
	m_light = new Light(Vector3(0.0f, 200.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.8f, 0.8f, 0.8f, 1.0f));
	m_GameObjects.push_back(m_light);

	//create DrawData struct and populate its pointers
	m_DD = new DrawData;
	m_DD->m_pd3dImmediateContext = nullptr;
	m_GD->m_ImmediateContext = pd3dImmediateContext;
	m_DD->m_states = m_states;
	m_DD->m_cam = m_cam;
	m_DD->m_light = m_light;

	Circle* _cir = new Circle();
	_cir->init(5, _pd3dDevice);
	m_GD->m_Circle = _cir;
	m_GameObjects.push_back(_cir);

	m_plane = new VBPlane();
	m_plane->init(_pd3dDevice);
	m_plane->SetScale(4.0f);
	m_GameObjects.push_back(m_plane);
	m_plane->DrawRenderTarget(m_DD2D, m_GD);

	//create a base camera
	m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
	m_cam->SetPos(Vector3(0.0f, 150, 400));
	m_GameObjects.push_back(m_cam);

	m_GD->m_clickState = IMPRINT;
	m_GD->m_displyState = HEIGHT;
}

Game::~Game()
{
	//delete Game Data & Draw Data
	delete m_GD;
	delete m_DD;

	//tidy up VBGO render system
	VBGO::CleanUp();

	//tidy away Direct Input
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
	}

	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();


	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}
	m_GameObject2Ds.clear();

	//clear away CMO render system
	delete m_states;
	delete m_fxFactory;

	delete m_DD2D;
}

bool Game::Update()
{
	//Poll Keyboard & Mouse
	ReadInput();

	//Upon pressing escape QUIT
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}

	// For demonstration input purposes
	handleDemoInput();

	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->m_dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;

	//update all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick( m_GD);
	}
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Tick(m_GD);
	}

	RECT window;
	GetWindowRect(m_hWnd, &window);
	return true;
}

// For demonstration input purposes
void Game::handleDemoInput()
{
	if ((m_keyboardState[DIK_1] & 0x80) && !(m_prevKeyboardState[DIK_1] & 0x80))
	{
		m_GD->m_clickState = IMPRINT;
	}
	if ((m_keyboardState[DIK_2] & 0x80) && !(m_prevKeyboardState[DIK_2] & 0x80))
	{
		m_GD->m_clickState = EXPLODE;
	}
	if ((m_keyboardState[DIK_3] & 0x80) && !(m_prevKeyboardState[DIK_3] & 0x80))
	{
		m_GD->m_clickState = RAIN;
	}
	if ((m_keyboardState[DIK_4] & 0x80) && !(m_prevKeyboardState[DIK_4] & 0x80))
	{
		m_GD->m_clickState = SNOW;
	}
	if ((m_keyboardState[DIK_Z] & 0x80) && !(m_prevKeyboardState[DIK_Z] & 0x80))
	{
		m_GD->m_displyState = HEIGHT;
		m_plane->verticesUpdated = true;
	}
	if ((m_keyboardState[DIK_X] & 0x80) && !(m_prevKeyboardState[DIK_X] & 0x80))
	{
		m_GD->m_displyState = SOFTNESS;
		m_plane->verticesUpdated = true;
	}
	if ((m_keyboardState[DIK_C] & 0x80) && !(m_prevKeyboardState[DIK_C] & 0x80))
	{
		m_GD->m_displyState = DISPLACEMENT;
		m_plane->verticesUpdated = true;
	}
}

void Game::Render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	//set immediate context of the graphics device
	m_DD->m_pd3dImmediateContext = _pd3dImmediateContext;

	//set which camera to be used
	m_DD->m_cam = m_cam;

	//update the constant buffer for the rendering of VBGOs
	VBGO::UpdateConstantBuffer(m_DD);

	//draw all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}

	m_plane->DrawTerrainElements(m_DD2D, m_GD);

	// Draw sprite batch 
	m_DD2D->m_Sprites->Begin();
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->Draw(m_DD2D);
	}
	m_DD2D->m_Sprites->End();

	//repair stencil state
	_pd3dImmediateContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);
}

bool Game::ReadInput()
{	
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(unsigned char)*256 );

	m_prevMouseState = m_mouseState;

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(unsigned char) * 256);
	ZeroMemory(&m_mouseState, sizeof(DIMOUSESTATE));

	// Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	// Read the Mouse device.
	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(hr))
	{
		// If the Mouse lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}
