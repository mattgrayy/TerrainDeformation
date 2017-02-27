#include "RenderTarget.h"
#include "DrawData2D.h"
#include <assert.h>

RenderTarget::RenderTarget(ID3D11Device* _device, int _width, int _height) :m_width(_width), m_height(_height)
{

	//create texture
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = _width;
	textureDesc.Height = _height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = _device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// Setup the staging texture description.
	textureDesc.Width = _width;
	textureDesc.Height = _height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.MiscFlags = 0;

	// Create the staging texture.
	result = _device->CreateTexture2D(&textureDesc, NULL, &m_stageTexture);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = _device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);	

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = _device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_textureSRV);

	//create BlendState for digging
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_ONE;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;
	result = _device->CreateBlendState(&blendDesc, &m_digOverflowBlendState);

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_ONE;
	rtbd.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	rtbd.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;
	result = _device->CreateBlendState(&blendDesc, &m_digBlendState);

	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;
	blendDesc.RenderTarget[0] = rtbd;
	result = _device->CreateBlendState(&blendDesc, &m_digToLevelBlendState);
}

RenderTarget::~RenderTarget()
{
	if (m_digBlendState)
	{
		m_digBlendState->Release();
		m_digBlendState = nullptr;
	}

	if (m_textureSRV)
	{
		m_textureSRV->Release();
		m_textureSRV = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_stageTexture)
	{
		m_stageTexture->Release();
		m_stageTexture = nullptr;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = nullptr;
	}

}

void RenderTarget::Begin(ID3D11DeviceContext* _pd3dImmediateContext, bool _deletePass)
{
	//store previous render targets etc
	_pd3dImmediateContext->OMGetRenderTargets(1, &m_oldRenderTargetView, &m_oldDepthStencilView);

	//set rendering system to aim to this texture
	_pd3dImmediateContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	//and am I drawing?
	m_drawing = !_deletePass;
}

void RenderTarget::End(ID3D11DeviceContext* _pd3dImmediateContext)
{
	//revert to old render targets etc
	_pd3dImmediateContext->OMSetRenderTargets(1, &m_oldRenderTargetView, m_oldDepthStencilView);
	m_oldRenderTargetView->Release();
	m_oldDepthStencilView->Release();
}

void RenderTarget::ClearRenderTarget(ID3D11DeviceContext* _pd3dImmediateContext, float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	if (m_renderTargetView)
	{ 
		_pd3dImmediateContext->ClearRenderTargetView(m_renderTargetView, color);
	}
	return;
}

UINT RenderTarget::GetPitch()
{
	if (m_mapped)
	{
		return m_pitch;
	}
	else
	{
		return 0;
	}
}

void RenderTarget::Map(ID3D11DeviceContext* _pd3dImmediateContext)
{
	assert(!m_mapped);

	//copy the render target texture to the staging texture
	//and then map that to allow the CPU to access it.
	if (m_renderTargetTexture && m_stageTexture)
	{ 
		_pd3dImmediateContext->CopyResource(m_stageTexture, m_renderTargetTexture);
		HRESULT hr = _pd3dImmediateContext->Map(m_stageTexture, 0, eMapType, NULL, &mappedResource);

		m_pitch = mappedResource.RowPitch;

		m_byte =  (BYTE*)mappedResource.pData;
	}
	m_mapped = true;
}


Color* RenderTarget::GetPixel(int x, int y)
{
	//grab a pixel within in the texture only when mapped
	if (m_mapped && m_byte)
	{	
		//clamp to inside the texture
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x > (m_width - 1)) x = m_width - 1;
		if (y > (m_height - 1)) y = m_height - 1;

		Color* test = (Color*)(&m_byte[y*m_pitch + 16 * x]);

		return test;
	}
	else
	{ 
		return nullptr;		
	}
}

void RenderTarget::Unmap(ID3D11DeviceContext* _pd3dImmediateContext)
{
	//unmap the staging texture to allow it to be used again
	assert(m_mapped);
	if (m_stageTexture)
	{ 
		_pd3dImmediateContext->Unmap(m_stageTexture, 0);
	}
	m_mapped = false;
}