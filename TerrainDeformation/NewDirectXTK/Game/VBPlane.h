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
	VBPlane(){};
	virtual ~VBPlane(){};

	//initialise the Veretx and Index buffers for the Plane
	void init(int _width, int _height, ID3D11Device* GD);
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);

	void toggleSnow(bool onOrOff, float valuePerTick);
	void calculateSnowfall();

	void moveSphere(bool _additive, Vector3 _center, float _radius, float _maxDisplacement);

protected:
	//this is to allow custom versions of this which create the basic Plane and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	virtual void Transform(){};

	float m_minY;
	float m_maxY;

	// snow Variables
	bool snowing = false;
	float snowRate = 0.003f;

	//temp
	float timer = 0;


	int m_width;
	int m_height;

	int numVerts;
	myVertex* m_vertices;
};

#endif