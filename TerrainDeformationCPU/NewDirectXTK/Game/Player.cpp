#include "Player.h"
#include <dinput.h>
#include "GameData.h"

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName,_pd3dDevice,_EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 10.0f;
}

Player::~Player()
{
	//tidy up anythign I've created
}


void Player::Tick(GameData* _GD)
{
	switch (_GD->m_GS)
	{
	case GS_PLAY_MAIN_CAM:
	{
		{
			//MOUSE CONTROL SCHEME HERE
			float speed = 0.1f;
			m_pos.x += speed * _GD->m_mouseState->lX;
			m_pos.z += speed * _GD->m_mouseState->lY;
		}
		break;
	}
	case GS_PLAY_TPS_CAM:
		//TURN AND FORWARD CONTROL HERE
		Vector3 forwardMove = 20.0f * _GD->m_dt * Vector3::Forward;
		Matrix rotMove = Matrix::CreateRotationY(m_yaw);
		forwardMove = Vector3::Transform(forwardMove, rotMove);
		if (_GD->m_keyboardState[DIK_W] & 0x80)
		{
			m_pos += forwardMove;
		}
		if (_GD->m_keyboardState[DIK_S] & 0x80)
		{
			m_pos -= forwardMove;
		}

		float rotSpeed = 2.0f * _GD->m_dt;
		if (_GD->m_keyboardState[DIK_A] & 0x80)
		{
			m_yaw += rotSpeed;
		}
		if (_GD->m_keyboardState[DIK_D] & 0x80)
		{
			m_yaw -= rotSpeed;
		}
		break;
	}

	if (_GD->m_keyboardState[DIK_R] & 0x80)
	{
		m_pos.y += _GD->m_dt * 10.0f;
	}

	if (_GD->m_keyboardState[DIK_F] & 0x80)
	{
		m_pos.y -= _GD->m_dt * 10.0f;
	}

	float length = m_pos.Length();
	float maxLength = 500.0f;
	if (length > maxLength)
	{
		m_pos *= maxLength / length;
	}

	//apply my base behaviour
	CMOGO::Tick(_GD);
}