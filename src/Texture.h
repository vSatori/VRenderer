#pragma once
#include "MathUtil.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

class Texture
{
public:
	Texture();
	Texture(const char* fileName);
	~Texture();
public:
	void setRowData(unsigned int* data, int width, int height);
	unsigned int sample(float u, float v);
private:
	void fillData(unsigned char* data, int comp = 4);
	void setSize(int width, int height);
private:
	unsigned int* m_rowData;
	int m_width;
	int m_height;
	int m_maxWidthIndex;
	int m_maxHeightIndex;
};

class DepthTexture
{
public:
	DepthTexture();
	~DepthTexture();
public:
	void setRowData(float* data, int width, int height, int levels);
	float sample(float u, float v, int level = 0);
private:
	void setSize(int w, int h);
private:
	
	float* m_rowData;
	int m_levels;
	int m_width;
	int m_height;
	int m_maxWidthIndex;
	int m_maxHeightIndex;
};


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
	std::vector<std::unique_ptr<Texture>> m_textures;
};


