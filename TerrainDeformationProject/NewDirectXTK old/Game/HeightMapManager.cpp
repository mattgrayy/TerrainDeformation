#include "HeightMapManager.h"
#include "EasyBMP.h"
using namespace std;

HeightMapManager::HeightMapManager()
{
	saveHeightMap();
}

HeightMapManager::~HeightMapManager()
{
}

void HeightMapManager::generateNewMap(int _width, int _height)
{
	m_width = _width;
	m_height = _height;
}

void HeightMapManager::saveHeightMap()
{
	BMP AnImage;
	// Set size to 640 × 480
	AnImage.SetSize(10, 10);
	// Set its color depth to 32-bits
	AnImage.SetBitDepth(32);

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (i % 2 == 0 && j % 2 == 1) 
			{
				AnImage(i, j)->Red = 0;
				AnImage(i, j)->Green = 0;
				AnImage(i, j)->Blue = 0;
				AnImage(i, j)->Alpha = 0;
			}
			else if(i % 2 == 1 && j % 2 == 0)
			{
				AnImage(i, j)->Red = 0;
				AnImage(i, j)->Green = 0;
				AnImage(i, j)->Blue = 0;
				AnImage(i, j)->Alpha = 0;
			}
		}
	}

	AnImage.WriteToFile("Output.bmp");
}