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

	if (_GD->m_keyboardState[DIK_A] & 0x80)
	{
		m_pos -= Vector3::Right;
		m_target -= Vector3::Right;
	}
	if (_GD->m_keyboardState[DIK_D] & 0x80)
	{
		m_pos += Vector3::Right;
		m_target += Vector3::Right;
	}

	//calculate standard transforms for a camera
	m_projMat = Matrix::CreatePerspectiveFieldOfView(m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance);
	m_viewMat = Matrix::CreateLookAt(m_pos, m_target, m_up);
	GameObject::Tick(_GD);
}

void Camera::Draw(DrawData* _DD)
{
	//standard camera doesn't draw ANYTHING
}