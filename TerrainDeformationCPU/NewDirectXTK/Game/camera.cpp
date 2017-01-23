//base camera class
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
		float distance = sqrt(pow(m_target.x - m_pos.x, 2) +
			pow(m_target.y - m_pos.y, 2) +
			pow(m_target.z - m_pos.z, 2));

		m_pos += Vector3::Transform(Vector3::Forward, Matrix::CreateRotationY(m_yaw)) * distance;
		m_yaw += 0.01f;
		m_pos -= Vector3::Transform(Vector3::Forward, Matrix::CreateRotationY(m_yaw)) * distance;
	}
	if (_GD->m_keyboardState[DIK_D] & 0x80)
	{
		float distance = sqrt(pow(m_target.x - m_pos.x, 2) +
			pow(m_target.y - m_pos.y, 2) +
			pow(m_target.z - m_pos.z, 2));

		m_pos += Vector3::Transform(Vector3::Forward, Matrix::CreateRotationY(m_yaw)) * distance;
		m_yaw -= 0.01f;
		m_pos -= Vector3::Transform(Vector3::Forward, Matrix::CreateRotationY(m_yaw)) * distance;
	}

	//calculate standard transforms for a camera
	m_projMat = Matrix::CreatePerspectiveFieldOfView( m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance );
	m_viewMat = Matrix::CreateLookAt( m_pos, m_target, m_up );
	GameObject::Tick(_GD);
}

void Camera::Draw(DrawData* _DD)
{
	//standard camera doesn't draw ANYTHING
}