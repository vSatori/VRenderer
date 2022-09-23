#include "Texture.h"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

CubeMap::CubeMap()
{
	m_textures.resize(6);
	for (int i = 0; i < 6; ++i)
	{
		m_textures[i] = std::make_unique<Texture1i>();
	}
}

CubeMap::CubeMap(const std::vector<std::string>& files)
{
	m_textures.resize(6);
	for (int i = 0; i < 6; ++i)
	{
		m_textures[i] = std::make_unique<Texture1i>();
		loadTexutre(files[i].c_str(), m_textures[i].get());
	}
}

unsigned int CubeMap::sample(const Vector3f& dir)
{
	float absx = abs(dir.x);
	float absy = abs(dir.y);
	float absz = abs(dir.z);
	float maxValue = std::max(std::max(absx, absy), abs(absz));
	if (maxValue == 0.f)
	{
		return false;
	}
	float x, y;
	Direction direction;
	if (maxValue == absx)
	{
		if (dir.x > 0.f)
		{
			x = 1.f - (dir.z / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.y / maxValue + 1.f) / 2.f;
			direction = Direction::RIGHT;
		}
		else
		{
			x = (dir.z / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.y / maxValue + 1.f) / 2.f;
			direction = Direction::LEFT;
		}
	}
	else if (maxValue == absy)
	{
		if (dir.y > 0.f)
		{
			x = (dir.x / maxValue + 1.f) / 2.f;
			y = (dir.z / maxValue + 1.f) / 2.f;
			direction = Direction::TOP;
		}
		else
		{
			x = (dir.x / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.z / maxValue + 1.f) / 2.f;
			direction = Direction::BOTTOM;
		}
	}
	else
	{
		if (dir.z > 0.f)
		{
			x = (dir.x / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.y / maxValue + 1.f) / 2.f;
			direction = Direction::BACK;
		}

		else
		{
			x = 1.f - (dir.x / maxValue + 1.f) / 2.f;
			y = (1.f - dir.y / maxValue) / 2.f;
			direction = Direction::FRONT;
		}
	}
	return m_textures[static_cast<int>(direction)]->sample0(x, y);
}

void CubeMap::setRowData(Direction dir, unsigned int* data, int width, int height)
{
	m_textures[static_cast<int>(dir)]->setRowData(data, width, height);
}

void loadTexutre(const char* textureFile, Texture1i* texture)
{
	int comp   = 0;
	int width  = 0;
	int height = 0;
	unsigned char* data = stbi_load(textureFile, &width, &height, &comp, 0);
	if (!data)
	{
		return;
	}
    int size = width * height;
	auto rowData = new unsigned int[size];
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = y * width + x;
			unsigned char* color = data + index * comp;
			unsigned int val = (color[0] << 16) + (color[1] << 8) + color[2];
			rowData[index] = val;
		}
	}
	texture->setRowData(rowData, width, height);
	delete[] rowData;
	stbi_image_free(data);
}
