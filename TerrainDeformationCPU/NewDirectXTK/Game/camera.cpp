#include "camera.h"
#include "GameData.h"
#include <iostream>

Camera::Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up, Vector3 _target)
{
	m_fieldOfView = _fieldOfView;
	m_aspectRatio = _aspectRatio;
	m_nearPlaneDistance = _nearPlaneDistance;
	m_farPlaneDistance = _farPlaneDistance;

	m_target = _target;
	m_up = _up;
}

Camera::~Camera()
{
}

void Camera::Tick(GameData* _GD)
{
	// Temporary demonstration movement detection
	if (_GD->m_keyboardState[DIK_A] & 0x80)
	{
		m_pos -= Vector3::Right * 3;
		m_target -= Vector3::Right * 3;
	}
	if (_GD->m_keyboardState[DIK_D] & 0x80)
	{
		m_pos += Vector3::Right * 3;
		m_target += Vector3::Right * 3;
	}
	if (_GD->m_keyboardState[DIK_W] & 0x80)
	{
		m_pos += Vector3::Forward * 3;
		m_target += Vector3::Forward * 3;
	}
	if (_GD->m_keyboardState[DIK_S] & 0x80)
	{
		m_pos -= Vector3::Forward * 3;
		m_target -= Vector3::Forward * 3;
	}

	// Temporary demonstration rotation detection
	if (_GD->m_keyboardState[DIK_Q] & 0x80)
	{
		m_target -= Vector3::Right;
	}
	if (_GD->m_keyboardState[DIK_E] & 0x80)
	{
		m_target += Vector3::Right;
	}
	if (_GD->m_keyboardState[DIK_F] & 0x80)
	{
		m_target -= Vector3::Forward;
	}
	if (_GD->m_keyboardState[DIK_R] & 0x80)
	{
		m_target += Vector3::Forward;
	}

	//calculate standard transforms for a camera
	m_projMat = Matrix::CreatePerspectiveFieldOfView(m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance);
	m_viewMat = Matrix::CreateLookAt(m_pos, m_target, m_up);
	GameObject::Tick(_GD);
}

void Camera::Draw(DrawData* _DD)
{
}