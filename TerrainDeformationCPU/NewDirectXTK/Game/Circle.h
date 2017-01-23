#ifndef _CIRCLE_H_
#define _CIRCLE_H_
#include "gameObject.h"

class Circle : public GameObject
{

public:
	Circle() {};
	virtual ~Circle() {};

	void init(float _radius, ID3D11Device* GD);
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);

	float m_radius;
};

#endif