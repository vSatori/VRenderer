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
	Vector3f sample(float u, float v);
	unsigned int sampleValue(float u, float v);
private:
	void fillData(unsigned char* data, int comp = 4);
	void setSize(int width, int height);
private:
	std::vector<Vector3f> m_components;
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
	void setRowData(float* data, int width, int height);
	float sample(float u, float v);
private:
	void setSize(int w, int h);
private:
	float* m_rowData;
	int m_width;
	int m_height;
	int m_maxWidthIndex;
	int m_maxHeightIndex;
};


class CubeMap
{
public:
	CubeMap();
	enum class Direction{LEFT,RIGHT,TOP,BOTTOM,FRONT,BACK};
protected:
	bool computeDirectionAndUV(const Vector3f& vec, Direction& dir, float& u, float& v);
protected:
	std::vector<std::unique_ptr<Texture>> m_textures;
};


class StaticCubeMap : public CubeMap
{
public:
	StaticCubeMap(const std::vector<std::string>& files);
public:
	Vector3f sample(const Vector3f& dir);
	
};


class DynamicCubeMap : public CubeMap
{
public:
	DynamicCubeMap();
public:
	void setRowData(Direction dir, unsigned int* data, unsigned int width, unsigned int height);
	unsigned int sample(const Vector3f& dir);
};
