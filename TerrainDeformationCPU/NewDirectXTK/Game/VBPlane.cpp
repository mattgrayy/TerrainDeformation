#include "VBPlane.h"

#include <iostream>
#include "GameData.h"

void VBPlane::init(int _width, int _height, ID3D11Device* GD)
{

	m_width = _width;
	if (m_width != 00 && m_width % 2 == 0)
	{
		m_width--;
	}

	m_height = _height;
	if (m_height != 00 && m_height % 2 == 0)
	{
		m_height--;
	}

	//calculate number of vertices and primatives
	numVerts = 6 * (m_width - 1) * (m_height - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i<numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;
	for (int i = -(m_width - 1) / 2; i<(m_width - 1) / 2; i++)
	{
		for (int j = -(m_height - 1) / 2; j<(m_height - 1) / 2; j++)
		{
			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.1f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);

			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.1f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(0.0f, 0.5f, 0.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)(j + 1));
		}
	}

	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i<m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(GD, indices);
	BuildVB(GD, numVerts, m_vertices);

	m_minY = m_vertices[0].Pos.y;
	m_maxY = m_vertices[0].Pos.y + 5;


	m_snowBounds.clear();
	m_snowBounds.push_back(Vector2(0, 0));
	m_snowBounds.push_back(Vector2(0, 0));
	m_snowBounds.push_back(Vector2(0, 0));
	m_snowBounds.push_back(Vector2(0, 0));
}

void VBPlane::Tick(GameData* _GD)
{
	if (snowing)
	{
		calculateSnowfall();
	}

	timer += _GD->m_dt;

	if (timer > 2)
	{
		if (snowing)
		{
			snowing = false;
		}

		int rndSnow = rand() % (5 - 0 + 1) + 0;
		if (rndSnow == 1)
		{

			rndSnow = rand() % (5 - 1 + 1) + 1;
			float newSnowRate = rndSnow/1000;

			std::vector<Vector2> bounds;

			float rndBoundx = rand() % (0 - -20 + 1) + -20;
			float rndBoundy = rand() % (0 - -20 + 1) + -20;
			bounds.push_back(Vector2(m_pos.x + rndBoundx, m_pos.y + rndBoundy));

			std::cout << rndBoundx << " - " << rndBoundy;

			rndBoundx = rand() % (20 - 0 + 1) + 0;
			rndBoundy = rand() % (0 - -20 + 1) + -20;
			bounds.push_back(Vector2(m_pos.x + rndBoundx, m_pos.y + rndBoundy));

			std::cout << rndBoundx << " - " << rndBoundy;

			rndBoundx = rand() % (20 - 0 + 1) + 0;
			rndBoundy = rand() % (20 - 0 + 1) + 0;
			bounds.push_back(Vector2(m_pos.x + rndBoundx, m_pos.y + rndBoundy));

			std::cout << rndBoundx << " - " << rndBoundy;

			rndBoundx = rand() % (0 - -20 + 1) + -20;
			rndBoundy = rand() % (20 - 0 + 1) + 0;
			bounds.push_back(Vector2(m_pos.x + rndBoundx, m_pos.y + rndBoundy));

			std::cout << rndBoundx << " - " << rndBoundy;


			toggleSnow(true, newSnowRate, bounds);
			timer = -5;
		}
		else
		{
			timer = 0;
		}

		int rndPoint = rand() % (numVerts - 0 + 1) + 0;
		float rndRadius = rand() % (20 - 5 + 1) + 5;
		float rndHeight = rand() % (3 - 1 + 1) + 1;

		moveSphere(true, m_vertices[rndPoint].Pos, rndRadius, rndHeight);

		rndPoint = rand() % (numVerts - 0 + 1) + 0;
		rndRadius = rand() % (20 - 5 + 1) + 5;
		rndHeight = rand() % (3 - 1 + 1) + 1;

		moveSphere(false, m_vertices[rndPoint].Pos, rndRadius, rndHeight);

		rndPoint = rand() % (numVerts - 0 + 1) + 0;
		rndRadius = rand() % (20 - 5 + 1) + 5;
		rndHeight = rand() % (3 - 1 + 1) + 1;

		moveSphere(false, m_vertices[rndPoint].Pos, rndRadius, rndHeight);
	}

	VBGO::Tick(_GD);
}

void VBPlane::toggleSnow(bool onOrOff, float valuePerTick, std::vector<Vector2> _bounds)
{
	if (_bounds.size() != 4)
	{
		m_snowBounds.clear();
		m_snowBounds.push_back(Vector2(0, 0));
		m_snowBounds.push_back(Vector2(0, 0));
		m_snowBounds.push_back(Vector2(0, 0));
		m_snowBounds.push_back(Vector2(0, 0));
	}
	else
	{
		m_snowBounds.clear();
		for (int i = 0; i < _bounds.size(); i++)
		{
			m_snowBounds.push_back(_bounds[i]);
		}
	}

	snowing = onOrOff;
	snowRate = valuePerTick;

	std::cout << "Toggled Snow";
}

void VBPlane::calculateSnowfall()
{
	bool hasBounds = false;
	for (int i = 0; i < m_snowBounds.size() - 1; i++)
	{
		if (m_snowBounds[i] != Vector2(0,0))
		{
			hasBounds = true;
			break;
		}
	}

	for (int i = 0; i < numVerts; i++)
	{
		if (m_vertices[i].Pos.x < m_snowBounds[0].x || m_vertices[i].Pos.y < m_snowBounds[0].y)
		{
			continue;
		}
		if (m_vertices[i].Pos.x > m_snowBounds[1].x || m_vertices[i].Pos.y < m_snowBounds[1].y)
		{
			continue;
		}
		if (m_vertices[i].Pos.x > m_snowBounds[2].x || m_vertices[i].Pos.y > m_snowBounds[2].y)
		{
			continue;
		}
		if (m_vertices[i].Pos.x < m_snowBounds[3].x || m_vertices[i].Pos.y > m_snowBounds[3].y)
		{
			continue;
		}

		m_vertices[i].Color += Color(0.0001f, 0.0001f, 0.0001f, 0.0f);
		m_vertices[i].Pos += Vector3(0, snowRate, 0);
	}
}

void VBPlane::moveSphere(bool _additive, Vector3 _center, float _radius, float _maxDisplacement)
{
	for (int i = 0; i < numVerts; i++)
	{
		float distance = sqrt(pow(_center.x - m_vertices[i].Pos.x, 2) +
			pow(_center.y - m_vertices[i].Pos.y, 2) +
			pow(_center.z - m_vertices[i].Pos.z, 2));

		if (distance < (_radius * _radius))
		{
			float displacement = _maxDisplacement - ((_maxDisplacement / _radius) * distance);
			float colourChange = 0.05f - ((0.05f / _radius) * distance);

			if (displacement < 0)
			{
				displacement = 0;
			}
			if (colourChange < 0)
			{
				colourChange = 0;
			}

			if (_additive && displacement > 0)
			{
				m_vertices[i].Color += Color(colourChange, colourChange, colourChange, 0.0f);
				m_vertices[i].Pos += Vector3(0, displacement, 0);
			}
			else if(displacement > 0)
			{
				m_vertices[i].Color -= Color(colourChange, colourChange, colourChange, 0.0f);
				m_vertices[i].Pos -= Vector3(0, displacement, 0);
			}
		}
	}
}

void VBPlane::Draw(DrawData* _DD)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	_DD->m_pd3dImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_vertices, sizeof(myVertex) * 6 * (m_width - 1) * (m_height - 1));
	_DD->m_pd3dImmediateContext->Unmap(m_VertexBuffer, 0);
	VBGO::Draw(_DD);
}