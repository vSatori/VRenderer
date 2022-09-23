#pragma once
#include "MathUtil.h"
#include <vector>
#include <string>
#include <map>
#include <memory>


template<typename T>
class TextureBase
{
public:
	TextureBase(){}
	virtual ~TextureBase(){}
public:
	inline int getWidth() { return m_width; }
	inline int getHeight() { return m_height; }
public:
	virtual void setRowData(T* data, int width, int height) = 0;
	virtual T* sample(float u, float v) = 0;
	virtual T sample0(float u, float v) = 0;
protected:
	T* m_rowData{ nullptr };
	int m_width{ 0 };
	int m_height{ 0 };
	int m_maxWidthIndex{ 0 };
	int m_maxHeightIndex{ 0 };

};






template<typename T, int comp>
class Texture : public TextureBase<T>
{
public:
	Texture() {}
	~Texture() { if (m_rowData)delete m_rowData; }
public:
	void setRowData(T* data, int width, int height)
	{
		int size = width * height * comp;
		setSize(width, height);
		if (m_rowData)
		{
			delete m_rowData;
		}
		m_rowData = new T[size];
		memcpy(m_rowData, data, size * sizeof(T));
	}
	T* sample(float u, float v)override
	{
		if (u < 0 || u > 1.f)
		{
			u = u < 0 ? 1.f - ((int)u - u) : u - (int)u;
		}
		if (v < 0 || v > 1.f)
		{
			v = v < 0 ? 1.f - ((int)v - v) : v - (int)v;
		}
		int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
		int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
		return m_rowData + ((y * m_width + x) * comp);
	}
	T  sample0(float u, float v)override
	{
		if (u < 0 || u > 1.f)
		{
			u = u < 0 ? 1.f - ((int)u - u) : u - (int)u;
		}
		if (v < 0 || v > 1.f)
		{
			v = v < 0 ? 1.f - ((int)v - v) : v - (int)v;
		}
		int x = static_cast<int>(u * m_maxWidthIndex  + 0.5f);
		int y = static_cast<int>(v * m_maxHeightIndex + 0.5f);
		return m_rowData[(y * m_width + x) * comp];
	}
private:
	void setSize(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_maxWidthIndex = width - 1;
		m_maxHeightIndex = height - 1;
	}
};

using Texture1i = Texture<unsigned int, 1>;
using Texture1f = Texture<float, 1>;
using Texture3f = Texture<float, 3>;

void loadTexutre(const char* textureFile, Texture1i* texture);

class CubeMap
{
public:
	CubeMap();
	CubeMap(const std::vector<std::string>& files);
	enum class Direction{LEFT,RIGHT,TOP,BOTTOM,FRONT,BACK};
public:
	unsigned int sample(const Vector3f& dir);
	void setRowData(Direction dir, unsigned int* data, int width, int height);
protected:
	std::vector<std::unique_ptr<Texture1i>> m_textures;
};


