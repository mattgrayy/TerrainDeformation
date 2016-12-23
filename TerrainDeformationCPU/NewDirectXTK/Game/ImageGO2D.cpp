#include "ImageGO2D.h"
#include "DDSTextureLoader.h"
#include "DrawData2D.h"
#include "GameData.h"
#include "helper.h"

#include <iostream>

ImageGO2D::ImageGO2D(string _fileName, ID3D11Device* _GD) :m_pTextureRV(nullptr)
{
	string fullfilename =
#if DEBUG
		"../Debug/"
#else
		"../Release/"
#endif
		+ _fileName + ".dds";
	HRESULT hr = CreateDDSTextureFromFile(_GD, Helper::charToWChar(fullfilename.c_str()), nullptr, &m_pTextureRV);

	//this nasty thing is required to find out the size of this image!
	ID3D11Resource *pResource;
	D3D11_TEXTURE2D_DESC Desc;
	m_pTextureRV->GetResource(&pResource);
	((ID3D11Texture2D *)pResource)->GetDesc(&Desc);

	m_origin = 0.5f*Vector2(Desc.Width, Desc.Height);//around which rotation and scaing is done

}

ImageGO2D::~ImageGO2D()
{
	if (m_pTextureRV)
	{
		m_pTextureRV->Release();
		m_pTextureRV = nullptr;
	}
}

void ImageGO2D::Tick(GameData* _GD)
{
	if (_GD->m_mouseState->rgbButtons[0] && !_GD->m_prevMouseState->rgbButtons[0])
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(*_GD->m_hwnd, &p);

		//this nasty thing is required to find out the size of this image!
		ID3D11Resource *pResource;
		D3D11_TEXTURE2D_DESC Desc;
		m_pTextureRV->GetResource(&pResource);
		((ID3D11Texture2D *)pResource)->GetDesc(&Desc);
		
		if (p.x >= m_pos.x - ((Desc.Width  * m_scale.x / 2)) && p.x <= m_pos.x + ((Desc.Width * m_scale.x / 2))
			&& p.y >= m_pos.y - ((Desc.Height  * m_scale.y / 2)) && p.y <= m_pos.y + ((Desc.Height  * m_scale.y / 2)))
		{
		}
	}

	GameObject2D::Tick(_GD);
}



void ImageGO2D::Draw(DrawData2D* _DD)
{
	//nullptr can be changed to a RECT* to define what area of this image to grab
	//you can also add an extra value at the end to define layer depth
	_DD->m_Sprites->Draw(m_pTextureRV, m_pos, nullptr, m_colour, m_rotation, m_origin, m_scale, SpriteEffects_None);
}