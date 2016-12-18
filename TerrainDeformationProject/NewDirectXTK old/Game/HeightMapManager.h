#ifndef _HEIGHT_MAP_MANAGER_H_
#define _HEIGHT_MAP_MANAGER_H_

#include <windows.h>
#include <vector>

class HeightMapManager
{
public:
	HeightMapManager();
	~HeightMapManager();

	void generateNewMap(int _width, int _height);
	void saveHeightMap();

private:

	int m_width;
	int m_height;

	std::vector<int*> greyscaleHeights;
};

#endif