#include "VBPlane.h"

#include <iostream>
#include "GameData.h"
#include "WICTextureLoader.h"
#include "helper.h"

void VBPlane::init(ID3D11Device* GD)
{
	HeightMapInfo hmInfo;
	if (loadHeightMap("heightmap.bmp", hmInfo))
	{
		m_width = hmInfo.terrainWidth;
		m_height = hmInfo.terrainWidth;
	}
	else
	{
		m_width = 2;
		m_height = 2;
	}


	string fullfilename = "heightmap.bmp";
	HRESULT hr = CreateWICTextureFromFile(GD, Helper::charToWChar(fullfilename.c_str()), nullptr, &m_heightMap);


	fullfilename = "Explosion.png";
	hr = CreateWICTextureFromFile(GD, Helper::charToWChar(fullfilename.c_str()), nullptr, &m_circleTex);
	//this nasty thing is required to find out the size of this image!
	ID3D11Resource *pResource;
	D3D11_TEXTURE2D_DESC Desc;
	m_circleTex->GetResource(&pResource);
	((ID3D11Texture2D *)pResource)->GetDesc(&Desc);

	m_circleSize.x = Desc.Width;
	m_circleSize.y = Desc.Height;



	m_renderTarget = new RenderTarget(GD, m_width, m_height);

	numVerts = m_width * m_height;
	m_numPrims = (m_width - 1)*(m_height - 1) * 2;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[m_numPrims * 3];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].Pos = Vector3(hmInfo.heightMap[i].x, hmInfo.heightMap[i].y - 25, hmInfo.heightMap[i].z);
		m_vertices[i].baseColor = Color(0.0f,0.3f, 1.0f, 1.0f);
		m_vertices[i].texCoord = Vector2::One;
	}

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;
	for (DWORD i = 0; i < m_width - 1; i++)
	{
		for (DWORD j = 0; j < m_height - 1; j++)
		{
			indices[k] = i* m_height + j;        // Bottom left of quad
			m_vertices[i*m_height + j].texCoord = Vector2(texUIndex + 0.0f, texVIndex + 1.0f);

			indices[k + 1] = i*m_height + j + 1;        // Bottom right of quad
			m_vertices[i*m_height + j + 1].texCoord = Vector2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 2] = (i + 1)*m_height + j;    // Top left of quad
			m_vertices[(i + 1)*m_height + j].texCoord = Vector2(texUIndex + 0.0f, texVIndex + 0.0f);


			indices[k + 3] = (i + 1)*m_height + j;    // Top left of quad
			m_vertices[(i + 1)*m_height + j].texCoord = Vector2(texUIndex + 0.0f, texVIndex + 0.0f);

			indices[k + 4] = i*m_height + j + 1;        // Bottom right of quad
			m_vertices[i*m_height + j + 1].texCoord = Vector2(texUIndex + 1.0f, texVIndex + 1.0f);

			indices[k + 5] = (i + 1)*m_height + j + 1;    // Top right of quad
			m_vertices[(i + 1)*m_height + j + 1].texCoord = Vector2(texUIndex + 1.0f, texVIndex + 0.0f);

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}
	
	//////////////////////Compute Normals///////////////////////////
	//Now we will compute the normals for each vertex using normal averaging
	std::vector<XMFLOAT3> tempNormal;

	//normalized and unnormalized normals
	XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

	//Used to get vectors (sides) from the position of the verts
	float vecX, vecY, vecZ;

	//Two edges of our triangle
	XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//Compute face normals
	for (int i = 0; i < m_numPrims; ++i)
	{
		//Get the vector describing one edge of our triangle (edge 0,2)
		vecX = m_vertices[indices[(i * 3)]].Pos.x - m_vertices[indices[(i * 3) + 2]].Pos.x;
		vecY = m_vertices[indices[(i * 3)]].Pos.y - m_vertices[indices[(i * 3) + 2]].Pos.y;
		vecZ = m_vertices[indices[(i * 3)]].Pos.z - m_vertices[indices[(i * 3) + 2]].Pos.z;
		edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

														//Get the vector describing another edge of our triangle (edge 2,1)
		vecX = m_vertices[indices[(i * 3) + 2]].Pos.x - m_vertices[indices[(i * 3) + 1]].Pos.x;
		vecY = m_vertices[indices[(i * 3) + 2]].Pos.y - m_vertices[indices[(i * 3) + 1]].Pos.y;
		vecZ = m_vertices[indices[(i * 3) + 2]].Pos.z - m_vertices[indices[(i * 3) + 1]].Pos.z;
		edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

														//Cross multiply the two edge vectors to get the un-normalized face normal
		DirectX::XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
		tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
	}

	//Compute vertex normals (normal Averaging)
	XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	//Go through each vertex
	for (int i = 0; i < numVerts; ++i)
	{
		//Check which triangles use this vertex
		for (int j = 0; j < m_numPrims; ++j)
		{
			if (indices[j * 3] == i ||
				indices[(j * 3) + 1] == i ||
				indices[(j * 3) + 2] == i)
			{
				tX = XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}

		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		normalSum = normalSum / facesUsing;

		//Normalize the normalSum vector
		normalSum = XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		m_vertices[i].Norm.x = XMVectorGetX(normalSum) + m_vertices[i].Pos.x;
		m_vertices[i].Norm.y = XMVectorGetY(normalSum) + m_vertices[i].Pos.y;
		m_vertices[i].Norm.z = XMVectorGetZ(normalSum) + m_vertices[i].Pos.z;

		//Clear normalSum and facesUsing for next vertex
		normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}

	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);
}

void VBPlane::Tick(GameData* _GD)
{
	int closestVertIndex = -1;
	float closestVertDistance = 300;

	for (int i = 0; i < numVerts; i++)
	{
		float distance = sqrt(pow(_GD->m_Circle->GetPos().x - m_vertices[i].Pos.x, 2) +
			pow(_GD->m_Circle->GetPos().z - m_vertices[i].Pos.z, 2));

		if (distance < _GD->m_Circle->m_radius)
		{
			if (distance < closestVertDistance)
			{
				closestVertDistance = distance;
				closestVertIndex = i;
			}

			m_vertices[i].color = Color(0.5f, 0.5f, 0.0f, 1.0f);
		}
		else if(m_vertices[i].color != m_vertices[i].baseColor)
		{
			m_vertices[i].color = m_vertices[i].baseColor;
		}
	}

	if (closestVertIndex != -1)
	{
		if (!(_GD->m_prevMouseState->rgbButtons[0] & 0x80) && _GD->m_mouseState->rgbButtons[0] & 0x80)
		{
			MakeHole(Vector2(_GD->m_Circle->GetPos().x, _GD->m_Circle->GetPos().z), _GD->m_Circle->m_radius);
		}
		if (!(_GD->m_prevMouseState->rgbButtons[1] & 0x80) && _GD->m_mouseState->rgbButtons[1] & 0x80)
		{

		}
	}

	updateVerts();

	VBGO::Tick(_GD);
}

void VBPlane::DrawRenderTarget(DrawData2D* _DD, GameData* _GD)
{
	m_renderTarget->Begin(_GD->m_ImmediateContext, false);

	_DD->m_Sprites->Begin();

	_DD->m_Sprites->Draw(m_heightMap, m_pos, nullptr, Color(1.0f, 0.0f, 0.0f, 1.0f), 0.0f, Vector2::Zero, Vector2::One);

	_DD->m_Sprites->End();

	m_renderTarget->End(_GD->m_ImmediateContext);

	m_renderTarget->Map(_GD->m_ImmediateContext);
}

void VBPlane::DrawTerrainElements(DrawData2D* _DD, GameData* _GD)
{
	if (m_holes.size() > 0)
	{
		//Unmap
		m_renderTarget->Unmap(_GD->m_ImmediateContext);

		//Begin render target
		m_renderTarget->Begin(_GD->m_ImmediateContext, true);

		if (m_holes.size() > 0)
		{
			//begin sprites WITH IMMEDIATE AND THE SECOND POINTER THINGY
			_DD->m_Sprites->Begin(DirectX::SpriteSortMode::SpriteSortMode_Immediate, m_renderTarget->GetDigBlend());

			for (auto it = m_holes.begin(); it != m_holes.end(); ++it)
			{
				//draw
				_DD->m_Sprites->Draw(m_circleTex, it->first, nullptr, Color(1, 0, 0, 0.2), 0, m_circleSize * 0.5f, it->second / m_circleSize.x);
			}
			m_holes.clear();

			//end sprites
			_DD->m_Sprites->End();
		}

		//end render target
		m_renderTarget->End(_GD->m_ImmediateContext);

		//remap
		m_renderTarget->Map(_GD->m_ImmediateContext);
	}
}

void VBPlane::updateVerts()
{
	for (int i = 0; i < m_width; i++)
	{
		for (int j = 0; j < m_height; j++)
		{
			Color* color = m_renderTarget->GetPixel(i, j);
			m_vertices[(j * m_width) + i].Pos = Vector3(m_vertices[(j * m_width) + i].Pos.x, color->x * 20, m_vertices[(j * m_width) + i].Pos.z);
			m_vertices[(j * m_width) + i].baseColor = *new Color(color->x, 0,0,1);
		}
	}
}

void VBPlane::MakeHole(const Vector2& pos, const float& radius)
{
	m_holes.push_back(std::pair<Vector2, float>(pos, radius));
}

void VBPlane::Draw(DrawData* _DD)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	_DD->m_pd3dImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_vertices, sizeof(myVertex) * m_width * m_height);
	_DD->m_pd3dImmediateContext->Unmap(m_VertexBuffer, 0);
	VBGO::Draw(_DD);
}

bool VBPlane::loadHeightMap(char* filename, HeightMapInfo &hminfo)
{
	FILE *filePtr;                            // Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	// Open the file
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return 0;

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for (int j = 0; j< hminfo.terrainHeight; j++)
	{
		for (int i = 0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}