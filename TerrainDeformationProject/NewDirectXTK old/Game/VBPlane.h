#ifndef _VB_Plane_H_
#define _VB_Plane_H_
#include "VBGO.h"
#include "vertex.h"
#include "drawdata.h"

//=================================================================
//procedurally generate a VBGO Plane
//=================================================================

class VBPlane : public VBGO
{
public:
	VBPlane() {};
	virtual ~VBPlane() {};

	//initialise the Veretx and Index buffers for the Plane
	void init(int _size, int _width, int _height, ID3D11Device* GD);
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);

	//void movePoint(int _thisPointNo, int changeMulti);
	bool checkMoveSafe(int _i, float _changeVal);

protected:
	//this is to allow custom versions of this which create the basic Plane and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	virtual void Transform() {};

	float m_minY;
	float m_maxY;

	int m_size;
	int m_width;
	int m_height;
	float m_changeRate = 0.001f;
	myVertex* m_vertices;
};

#endif