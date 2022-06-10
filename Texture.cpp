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
Texture::Texture(const char* fileName)
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

Vector3f Texture::sample(float u, float v)
{
	int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
	int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
	return m_components[y * m_width + x];
}

unsigned int Texture::sampleValue(float u, float v)
{
	int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
	int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
	return m_rowData[y * m_width + x];
}

void Texture::fillData(unsigned char * data, int comp)
{
	unsigned int size = m_width * m_height;
	m_components.resize(m_width * m_height);
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			int index = y * m_width + x;
			unsigned char* color = data + index * comp;
			float r = (float)(color[0]) / 255.f;
			float g = (float)(color[1]) / 255.f;
			float b = (float)(color[2]) / 255.f;
			m_components[index] = { r, g, b };
		}
	}
	m_rowData = new unsigned int[size];
	memcpy(m_rowData, data, size * sizeof(char) * comp);
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
	int x = static_cast<int>(u * m_maxWidthIndex + 0.5f);
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

bool CubeMap::computeDirectionAndUV(const Vector3f& vec, Direction& direction, float& u, float& v)
{
	float absx = abs(vec.x);
	float absy = abs(vec.y);
	float absz = abs(vec.z);
	float maxValue = std::max(std::max(absx, absy), abs(absz));
	if (maxValue == 0.f)
	{
		return false;
	}
	float x, y;
	//Direction direction;
	if (maxValue == absx)
	{
		if (vec.x > 0.f)
		{
			x = 1.f - (vec.z / maxValue + 1.f) / 2.f;
			y = 1.f - (vec.y / maxValue + 1.f) / 2.f;
			direction = Direction::RIGHT;
		}
		else
		{
			x = (vec.z / maxValue + 1.f) / 2.f;
			y = 1.f - (vec.y / maxValue + 1.f) / 2.f;
			direction = Direction::LEFT;
		}
	}
	else if (maxValue == absy)
	{
		if (vec.y > 0.f)
		{
			x = (vec.x / maxValue + 1.f) / 2.f;
			y = (vec.z / maxValue + 1.f) / 2.f;
			direction = Direction::TOP;
		}
		else
		{
			x = (vec.x / maxValue + 1.f) / 2.f;
			y = 1.f - (vec.z / maxValue + 1.f) / 2.f;
			direction = Direction::BOTTOM;
		}
	}
	else
	{
		if (vec.z > 0.f)
		{
			x = (vec.x / maxValue + 1.f) / 2.f;
			y = 1.f - (vec.y / maxValue + 1.f) / 2.f;
			direction = Direction::BACK;
		}

		else
		{
			x = 1.f - (vec.x / maxValue + 1.f) / 2.f;
			y = (1.f - vec.y / maxValue) / 2.f;
			direction = Direction::FRONT;
		}
	}
	u = x;
	v = y;
	return true;
}



DynamicCubeMap::DynamicCubeMap()
{
	
}

void DynamicCubeMap::setRowData(Direction dir, unsigned int * data, unsigned int width, unsigned int height)
{
	m_textures[static_cast<int>(dir)]->setRowData(data, width, height);
}

unsigned int DynamicCubeMap::sample(const Vector3f & dir)
{
	Direction direction;
	float u, v;
	if (!computeDirectionAndUV(dir, direction, u, v))
	{
		return 0;
	}
	
	return m_textures[static_cast<int>(direction)]->sampleValue(u, v);
}



StaticCubeMap::StaticCubeMap(const std::vector<std::string>& files)
{
	for (int i = 0; i < 6; ++i)
	{
		m_textures[i] = std::make_unique<Texture>(files[i].c_str());
	}
}

Vector3f StaticCubeMap::sample(const Vector3f& dir)
{
	Direction direction;
	float u, v;
	if (!computeDirectionAndUV(dir, direction, u, v))
	{
		return {0.f, 0.f, 0.f};
	}
	return m_textures[static_cast<int>(direction)]->sample(u, v);
}
