#ifndef _VB_Plane_H_
#define _VB_Plane_H_
#include "VBGO.h"
#include "vertex.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include "RenderTarget.h"
#include <vector>

//=================================================================
//procedurally generate a VBGO Plane
//=================================================================

class VBPlane : public VBGO
{
public:
	VBPlane(){};
	virtual ~VBPlane(){};

	struct HeightMapInfo {        // Heightmap structure
		int terrainWidth;        // Width of heightmap
		int terrainHeight;        // Height (Length) of heightmap
		XMFLOAT3 *heightMap;    // Array to store terrain's vertex positions
	};

	struct DeformStencil {      
		ID3D11ShaderResourceView* texture;
		Vector2 textureSize;
		Vector2 position;
		float scale;
		float yRotation;
		Color colour;
	};

	//initialise the Veretx and Index buffers for the Plane
	void init(ID3D11Device* GD);
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);

	void DrawRenderTarget(DrawData2D* _DD, GameData* _GD);
	void DrawTerrainElements(DrawData2D* _DD, GameData* _GD);
	void updateVerts(GameData* _GD);

	Color levelSurfaceForStencil(ID3D11ShaderResourceView* _texture, Vector2 _position, float _scale);
	void makeStencil(ID3D11ShaderResourceView* _texture, Vector2 _position, float _scale, float _yRotation, float _depth, bool _levelSurface, bool _toRaise);

	bool loadHeightMap(char* filename, HeightMapInfo &hminfo);

	bool verticesUpdated = true;

protected:
	virtual void Transform(){};

	int m_width;
	int m_height;

	ID3D11ShaderResourceView* m_heightMap;

	// temporary demo input textures
	ID3D11ShaderResourceView* m_circleTex;
	ID3D11ShaderResourceView* m_footPrint;

	std::vector<DeformStencil> m_stencils;
	std::vector<DeformStencil> m_stencilsToLevel;
	std::vector<DeformStencil> m_stencilsToOverflow;

	int numVerts;
	myVertex* m_vertices;
	RenderTarget* m_renderTarget;
};

#endif