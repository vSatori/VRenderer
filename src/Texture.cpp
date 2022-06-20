#include "Texture.h"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
Texture::Texture() : 
	m_rowData(nullptr), 
	m_width(0), 
	m_height(0),
	m_maxWidthIndex(0),
	m_maxHeightIndex(0)
{
	
}
Texture::Texture(const char* fileName) : 
	m_rowData(nullptr),
	m_width(0),
	m_height(0),
	m_maxWidthIndex(0),
	m_maxHeightIndex(0)
{
	int comp = 0;
	int width = 0;
	int height = 0;
	unsigned char* data = stbi_load(fileName, &width, &height, &comp, 0);
	setSize(width, height);
	fillData(data, comp);
	stbi_image_free(data);
}

Texture::~Texture()
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
}



void Texture::setRowData(unsigned int * data, int width, int height)
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
	setSize(width, height);
	int size = m_width * m_height;
	m_rowData = new unsigned int[size];
	memcpy(m_rowData, data, size * sizeof(unsigned int));
}



unsigned int Texture::sample(float u, float v)
{
	int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
	int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
	return m_rowData[y * m_width + x];
}

void Texture::fillData(unsigned char * data, int comp)
{
	unsigned int size = m_width * m_height;
	m_rowData = new unsigned int[size];
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			int index = y * m_width + x;
			unsigned char* color = data + index * comp;
			unsigned int val = (color[0] << 16) + (color[1] << 8) + color[2];
			m_rowData[index] = val;
		}
	}
	
}

void Texture::setSize(int width, int height)
{
	m_width = width;
	m_height = height;
	m_maxWidthIndex = width - 1;
	m_maxHeightIndex = height - 1;
}

DepthTexture::DepthTexture() :
	m_rowData(nullptr),
	m_width(0),
	m_height(0),
	m_maxWidthIndex(0),
	m_maxHeightIndex(0)
{
}

DepthTexture::~DepthTexture()
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
}

void DepthTexture::setRowData(float* data, int width, int height)
{
	if (m_rowData)
	{
		delete[] m_rowData;
	}
	setSize(width, height);
	int size = m_width * m_height;
	m_rowData = new float[size];
	memcpy(m_rowData, data, size * sizeof(float));
}

float DepthTexture::sample(float u, float v)
{
	int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
	int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
	return m_rowData[y * m_width + x];
}

void DepthTexture::setSize(int w, int h)
{
	m_width = w;
	m_height = h;
	m_maxWidthIndex = w - 1;
	m_maxHeightIndex = h - 1;
}


CubeMap::CubeMap()
{
	m_textures.resize(6);
	for (int i = 0; i < 6; ++i)
	{
		m_textures[i] = std::make_unique<Texture>();
	}
}

CubeMap::CubeMap(const std::vector<std::string>& files)
{
	m_textures.resize(6);
	for (int i = 0; i < 6; ++i)
	{
		m_textures[i] = std::make_unique<Texture>(files[i].c_str());
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
	return m_textures[static_cast<int>(direction)]->sample(x, y);
}

void CubeMap::setRowData(Direction dir, unsigned int* data, int width, int height)
{
	m_textures[static_cast<int>(dir)]->setRowData(data, width, height);
}