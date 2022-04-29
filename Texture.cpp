#include "Texture.h"
#define LOADBMP_IMPLEMENTATION
#include "loadbmp.h"
#include <algorithm>
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
	delete[] rowData;
}

Texture::~Texture()
{
	
}



Vector3f Texture::sample(float u, float v)
{
	int x = u * m_width + 0.5;
	int y = v * m_width + 0.5;
	int index = y * m_width + x;
	if (index > m_components.size() - 1)
	{
		index = m_components.size() - 1;
	}
	return m_components[index];
}


CubeTexture::CubeTexture(const std::vector<std::string>& files)
{
	
	for (int i = 0; i < 6; ++i)
	{
		m_textures[static_cast<Direction>(i)] = std::make_unique<Texture>(files[i].c_str());

	}
}

#include "Renderer.h"
Vector3f CubeTexture::sample(const Vector3f & dir)
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
			x = 1.f - (dir.z + 1.f) / 2.f;
			y = 1.f - (dir.y + 1.f) / 2.f;
			direction = Direction::RIGHT;
		}
		else
		{
			x = (dir.z + 1.f) / 2.f;
			y = 1.f - (dir.y + 1.f) / 2.f;
			direction = Direction::RIGHT;
		}
	}
	else if (maxValue == absy)
	{
		if (dir.y> 0.f)
		{
			x = 1.f - (dir.x + 1.f) / 2.f;
			y = (dir.z + 1.f) / 2.f;
			direction = Direction::TOP;
		}
		else
		{
			x = 1.f - (dir.x + 1.f) / 2.f;
			y = 1.f - (dir.z + 1.f) / 2.f;
			direction = Direction::TOP;
		}
		
	}
	
	else 
	{
		
		if (dir.z > 0.f)
		{
			x = (dir.x + 1.f) / 2.f;
			y = 1.f - (dir.y + 1.f) / 2.f;
			direction = Direction::BACK;
		}
		
		else
		{
			float x = (dir.x + 1.f) / 2.f;
			float y = (1.f - dir.y) / 2.f;
			direction = Direction::FRONT;
		}
	    
		
	}
	Renderer::s_maxX = fmaxf(Renderer::s_maxX, x);
	Renderer::s_maxX = fmaxf(Renderer::s_maxY, y);
	Renderer::s_minX = fminf(Renderer::s_minX, x);
	Renderer::s_minY = fminf(Renderer::s_minY, y);
	return m_textures[direction]->sample(x, y);
}
