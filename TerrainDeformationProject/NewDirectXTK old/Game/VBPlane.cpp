#include "VBPlane.h"

#include <iostream>

void VBPlane::init(int _size, int _width, int _height, ID3D11Device* GD)
{
	m_size = _size;
	if (m_size != 00 && m_size % 2 == 0)
	{
		m_size--;
	}

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
	int numVerts = 6 * (m_width - 1) * (m_height - 1);
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
			m_vertices[vert].Color = Color(0.0f, 0.3f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(0.0f, 0.3f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);

			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)(i + 1), 0.0f, (float)j);
			m_vertices[vert].Color = Color(0.0f, 0.3f, 1.0f, 1.0f);
			m_vertices[vert++].Pos = Vector3((float)i, 0.0f, (float)(j + 1));
			m_vertices[vert].Color = Color(0.0f, 0.0f, 1.0f, 1.0f);
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
}

void VBPlane::Tick(GameData* _GD)
{
	VBGO::Tick(_GD);
}

/*
void VBPlane::movePoint(int _thisPointNo, int changeMulti)
{
	// double check it's safe
	if (_thisPointNo <= (m_size)*(m_size)-1)
	{
		// check if it's a corner
		if (_thisPointNo == 0 || _thisPointNo == m_size - 1 || _thisPointNo == (m_size)*(m_size - 1) || _thisPointNo == (m_size)*(m_size)-1)
		{
			if (!checkMoveSafe(0, m_changeRate * changeMulti))
			{
				return;
			}
			m_vertices[0].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[0].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			int vert = ((m_size - 2) * 6) + 1;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 3].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 3].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			vert = (((m_size - 1)*(m_size - 2)) * 6) + 2;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 1].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 1].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			vert = (((m_size - 1)*(m_size - 1)) * 6) - 1;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);


			return;
		}
		// check if it's top/bottom
		else if (_thisPointNo % m_size == 0 || _thisPointNo % m_size == m_size - 1)
		{
			//convert bottom to top for easier maths
			if (_thisPointNo % m_size == m_size - 1)
			{
				_thisPointNo -= m_size - 1;
			}
			// calculate top based on size, point and +2 because column 1 starts with 2
			int vert = (_thisPointNo / (m_size - 1) - 1) * ((m_size - 1) * 6) + 2;

			if (!checkMoveSafe(vert, m_changeRate * changeMulti))
			{
				return;
			}

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 1].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 1].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			// calculate the higher numbers, 1 for top and 1 for bottom
			vert = (_thisPointNo / (m_size - 1)) * ((m_size - 1) * 6) - 1;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 1].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 1].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			// calculate higher numbers (bottom right)
			vert = ((_thisPointNo / (m_size - 1) + 1) * (((m_size - 1) * 6))) - 5;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 3].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 3].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			return;
		}
		// check if it's left/right
		else if (_thisPointNo < m_size || _thisPointNo >(m_size)*(m_size - 1))
		{
			// convert right to left for easier maths
			if (_thisPointNo > (m_size)*(m_size - 1))
			{
				_thisPointNo = _thisPointNo % m_size;
			}

			int vert = ((_thisPointNo - 1) * 6) + 1;

			if (!checkMoveSafe(vert, m_changeRate * changeMulti))
			{
				return;
			}

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 3].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 5].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 3].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 5].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			vert = (((m_size - 1)*(m_size - 2)) * 6) + ((_thisPointNo - 1) * 6) + 5;

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 3].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 4].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 3].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 4].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

		}
		else
		{
			// use trncate to find whole number of rows before this one
			int vert = (_thisPointNo - (trunc(_thisPointNo / (m_size)))) * 6;

			if (!checkMoveSafe(vert, m_changeRate * changeMulti))
			{
				return;
			}

			m_vertices[vert].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert - 2].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert - 5].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert - 2].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert - 5].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);

			vert -= (m_size - 1) * 6;

			m_vertices[vert - 1].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 2].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);
			m_vertices[vert + 3].Pos += Vector3(0.0f, m_changeRate * changeMulti, 0.0f);

			m_vertices[vert - 1].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 1].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
			m_vertices[vert + 3].Color += Color(0.0f, 0.0002f * changeMulti, 0.0f, 0.0f);
		}
	}
}*/

bool VBPlane::checkMoveSafe(int _i, float changeVal)
{
	if (m_vertices[_i].Pos.y + changeVal > m_minY && m_vertices[_i].Pos.y + changeVal < m_maxY)
	{
		return true;
	}
	return false;
}

void VBPlane::Draw(DrawData* _DD)
{
	//D3D11_MAPPED_SUBRESOURCE resource;
	//_DD->m_pd3dImmediateContext->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//memcpy(resource.pData, m_vertices, sizeof(myVertex) * 6 * (m_size - 1) * (m_size - 1));
	//_DD->m_pd3dImmediateContext->Unmap(m_VertexBuffer, 0);
	VBGO::Draw(_DD);
}