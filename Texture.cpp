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
	int x = static_cast<int>(u * m_maxWidthIndex + 0.5f);
	int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
	return m_components[y * m_width + x];
}

unsigned int Texture::sampleValue(float u, float v)
{
	int x = u * m_maxWidthIndex + 0.5;
	int y = v * m_maxHeightIndex + 0.5;
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


CubeMap::CubeMap(const std::vector<std::string>& files)
{
	
	for (int i = 0; i < 6; ++i)
	{
		m_textures[static_cast<Direction>(i)] = std::make_unique<Texture>(files[i].c_str());

	}
	m_back = m_textures[Direction::BACK].get();
	m_front = m_textures[Direction::FRONT].get();
	m_left = m_textures[Direction::LEFT].get();
	m_right = m_textures[Direction::RIGHT].get();
	m_top = m_textures[Direction::TOP].get();
	m_bottom = m_textures[Direction::BOTTOM].get();
}

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
			//return m_right->sample(x, y);
		}
		else
		{
			x = (dir.z / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.y / maxValue + 1.f) / 2.f;
			direction = Direction::LEFT;
			//return m_left->sample(x, y);
		}
	}
	else if (maxValue == absy)
	{
		if (dir.y> 0.f)
		{
			x = (dir.x / maxValue + 1.f) / 2.f;
			y = (dir.z / maxValue + 1.f) / 2.f;
			direction = Direction::TOP;
			//return m_top->sample(x, y);
		}
		else
		{
			x = (dir.x / maxValue + 1.f) / 2.f;
			y = 1.f - (dir.z / maxValue + 1.f) / 2.f;
			direction = Direction::BOTTOM;
			//return m_bottom->sample(x, y);
		}
	}
	else 
	{
		if (dir.z > 0.f)
		{
			x = (dir.x/ maxValue + 1.f) / 2.f;
			y = 1.f - (dir.y / maxValue + 1.f) / 2.f;
			direction = Direction::BACK;
			//return m_back->sample(x, y);
		}
		
		else
		{
			x = 1.f - (dir.x / maxValue + 1.f) / 2.f;
			y = (1.f - dir.y / maxValue) / 2.f;
			direction = Direction::FRONT;
			//return m_front->sample(x, y);
		}
	}

	auto tex = m_textures[direction].get();
	return tex->sample(x, y);
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

void DepthTexture::setRowData(float * data, int width, int height)
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
	int x = u * m_maxWidthIndex + 0.5;
	int y = v * m_maxHeightIndex + 0.5;
	return m_rowData[y * m_width + x];
}

void DepthTexture::setSize(int w, int h)
{
	m_width = w;
	m_height = h;
	m_maxWidthIndex = w - 1;
	m_maxHeightIndex = h - 1;
}
