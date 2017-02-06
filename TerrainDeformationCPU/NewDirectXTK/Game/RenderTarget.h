#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_
#include "SpriteBatch.h"
#include "SimpleMath.h"

struct DrawData2D;

using namespace DirectX;
using namespace DirectX::SimpleMath;

class RenderTarget
{
public:

	RenderTarget(ID3D11Device* _device, int _width, int _height);
	~RenderTarget();

	ID3D11ShaderResourceView* GetShaderResourceView(){ return m_textureSRV; }

	//clear render target
	void ClearRenderTarget(ID3D11DeviceContext* _pd3dImmediateContext, float red, float green, float blue, float alpha);


	// Begin/End a rendering to the target
	void Begin(ID3D11DeviceContext* _pd3dImmediateContext, bool _deletePass=false);
	void End(ID3D11DeviceContext* _pd3dImmediateContext);

	//map and unmap the texture
	void Map(ID3D11DeviceContext* _pd3dImmediateContext);
	void Unmap(ID3D11DeviceContext* _pd3dImmediateContext);
	UINT GetPitch();
	Color* GetPixel(int x, int y);

	ID3D11BlendState* GetDigBlend()
	{ 
		return m_digBlendState; 
	}

protected:

	int m_width, m_height;
	bool m_drawing = true;	
	bool m_mapped = false;

	//stuff needed to use the staging texture
	UINT m_pitch;
	BYTE* m_byte = nullptr;
	D3D11_MAP eMapType = D3D11_MAP_READ;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	
	//the actual texture
	ID3D11Texture2D* m_renderTargetTexture = nullptr;

	//the staging texture for the collisions
	ID3D11Texture2D* m_stageTexture = nullptr;

	//pointer to this texture as a shader resource view
	ID3D11ShaderResourceView* m_textureSRV = nullptr;

	//pointer to this object as a render target view
	ID3D11RenderTargetView* m_renderTargetView = nullptr;

	//pointers to the old render target and depth stencil views
	ID3D11RenderTargetView* m_oldRenderTargetView = nullptr;
	ID3D11DepthStencilView* m_oldDepthStencilView = nullptr;

	//digging Blendstate
	ID3D11BlendState* m_digBlendState = nullptr;

};

#endif