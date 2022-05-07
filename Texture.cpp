#include "Texture.h"
#define LOADBMP_IMPLEMENTATION
#include "loadbmp.h"
#include <algorithm>
Texture::Texture() : m_rowData(nullptr), m_width(0), m_height(0)
{
}
Texture::Texture(const char * fileName)
{
	unsigned char* rowData;
	loadbmp_decode_file(fileName, &rowData, &m_width, &m_height, LOADBMP_RGBA);
	unsigned int* data = (unsigned int*)rowData;
	m_components.resize(m_width * m_height);
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			int index = y * m_width + x;
			unsigned int color = data[index];

			float r = ((float)((color << 8) >> 24)) / 255.f;
			float g = ((float)((color << 16) >> 24)) / 255.f;
			float b = ((float)((color << 24) >> 24)) / 255.f;
			m_components[index] = { r, g, b };
		}
	}
	unsigned int size = m_width * m_height;
	m_rowData = new unsigned int[size];
	memcpy(m_rowData, data, size * sizeof(unsigned int));
}

Texture::~Texture()
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
}



void Texture::setRowData(unsigned int * data, unsigned int width, unsigned int height)
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
	m_width = width;
	m_height = height;
	unsigned int size = m_width * m_height;
	m_rowData = new unsigned int[size];
	memcpy(m_rowData, data, size * sizeof(unsigned int));
	m_maxIndex = size - 1;
}

Vector3f Texture::sample(float u, float v)
{
	int x = u * m_width + 0.5;
	int y = v * m_height + 0.5;
	int index = y * m_width + x;
	if (index > m_components.size() - 1)
	{
		index = m_components.size() - 1;
	}
	return m_components[index];
}
#include <qdebug.h>
unsigned int Texture::sampleValue(float u, float v)
{
	int x = u * m_width + 0.5;
	int y = v * m_height + 0.5;
	if (y >= m_height)
	{
		y = m_height - 1;
	}
	return m_rowData[y * m_width + x];
}


CubeMap::CubeMap(const std::vector<std::string>& files)
{
	
	for (int i = 0; i < 6; ++i)
	{
		m_textures[static_cast<Direction>(i)] = std::make_unique<Texture>(files[i].c_str());

	}
}

#include "Renderer.h"
Vector3f CubeMap::sample(const Vector3f & dir)
{
	float absx = abs(dir.x);
	float absy = abs(dir.y);
	float absz = abs(dir.z);
	float maxValue = std::max(std::max(absx, absy), abs(absz));
	
	if (maxValue == 0.f)
	{
		return Vector3f();
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
		if (dir.y> 0.f)
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
			x = (dir.x/ maxValue + 1.f) / 2.f;
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
	
	return m_textures[direction]->sample(x, y);
}



DynamicCubeMap::DynamicCubeMap()
{
	for (int i = 0; i < 6; ++i)
	{
		m_textures[static_cast<Direction>(i)] = std::make_unique<Texture>();
	}
}

void DynamicCubeMap::setRowData(Direction dir, unsigned int * data, unsigned int width, unsigned int height)
{
	m_textures[dir]->setRowData(data, width, height);
}

unsigned int DynamicCubeMap::sample(const Vector3f & dir)
{
	float absx = abs(dir.x);
	float absy = abs(dir.y);
	float absz = abs(dir.z);
	float maxValue = std::max(std::max(absx, absy), abs(absz));

	if (maxValue == 0.f)
	{
		return 0;
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
	return m_textures[direction]->sampleValue(x, y);
}
