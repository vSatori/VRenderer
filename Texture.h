#pragma once
#include "MathUtility.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
class Texture
{
public:
	Texture();
	Texture(const char* fileName, bool isBmp = true);
	~Texture();
public:
	void setRowData(unsigned int* data, unsigned int width, unsigned int height);
	Vector3f sample(float u, float v);
	unsigned int sampleValue(float u, float v);
private:
	std::vector<Vector3f> m_components;
	unsigned int* m_rowData;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_maxIndex;
};

class DepthTexture
{
public:
	DepthTexture();
	~DepthTexture();
public:
	void setRowData(float* data, unsigned int width, unsigned int height);
	float sampleValue(float u, float v);
private:
	float* m_rowData;
	unsigned int m_width;
	unsigned int m_height;
};


class CubeMap
{
public:
	CubeMap(const std::vector<std::string>& files);
	enum class Direction{LEFT,RIGHT,TOP,BOTTOM,FRONT,BACK};
public:
	Vector3f sample(const Vector3f& dir);
private:
	std::map<Direction, std::unique_ptr<Texture>> m_textures;
};

class DynamicCubeMap
{
public:
	DynamicCubeMap();
	enum class Direction { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
public:
	void setRowData(Direction dir, unsigned int* data, unsigned int width, unsigned int height);
	unsigned int sample(const Vector3f& dir);
private:
	std::map<Direction, std::unique_ptr<Texture>> m_textures;
};
