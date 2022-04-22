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
			char cr = color >> 16;
			char cg = color >> 8;
			char cb = color;
			float r = ((float)(cr)) / 255.f;
			float g = ((float)(cg)) / 255.f;
			float b = ((float)(cb)) / 255.f;
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
	return m_components[y * m_width + x];
}


CubeTexture::CubeTexture(const std::vector<std::string>& files)
{
	
	for (int i = 0; i < 6; ++i)
	{
		m_textures[static_cast<Direction>(i)] = std::make_unique<Texture>(files[i].c_str());

	}
}

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
	if (maxValue == absx)
	{
		if (dir.x > 0)
		{
			return m_textures[Direction::RIGHT]->sample(1.f - (dir.z + 1.f) / 2.f, 1.f - (dir.y + 1.f) / 2.f);
		}
		return m_textures[Direction::LEFT]->sample(1.f - (dir.z + 1.f) / 2.f, 1.f - (dir.y / maxValue + 1.f) / 2.f);
	}
	if (maxValue == absy)
	{
		if (dir.y> 0)
		{
			return m_textures[Direction::TOP]->sample(1.f - (dir.x + 1.f) / 2.f, 1.f - (dir.z + 1.f) / 2.f);
		}
		return m_textures[Direction::BOTTOM]->sample(1.f - (dir.x + 1.f) / 2.f, 1.f - (dir.z + 1.f) / 2.f);
	}
	if (maxValue == absz)
	{
		if (dir.z > 0)
		{
			return m_textures[Direction::FRONT]->sample(1.f - (dir.x + 1.f) / 2.f, 1.f - (dir.y + 1.f) / 2.f);
		}
		return m_textures[Direction::BACK]->sample(1.f - (dir.x + 1.f) / 2.f, 1.f - (dir.y + 1.f) / 2.f);
	}
	return Vector3f();
}
