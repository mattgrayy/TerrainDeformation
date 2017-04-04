#include "Circle.h"
#include "GameData.h"

void Circle::init(float _radius, ID3D11Device* GD)
{
	m_radius = _radius;
}

void Circle::Tick(GameData* _GD)
{
	m_pos.x +=  0.1f * _GD->m_mouseState->lX;
	m_pos.z +=  0.1f * _GD->m_mouseState->lY;

	m_radius -= 0.01f * _GD->m_mouseState->lZ;

	GameObject::Tick(_GD);
}

void Circle::Draw(DrawData* _DD)
{
}