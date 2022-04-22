#pragma once
#include "MathUtility.h"
#include <vector>
#include <string>
#include <map>
#include <memory>
class Texture
{
public:
	Texture(const char* fileName);
	~Texture();
public:
	Vector3f sample(float u, float v);
private:
	std::vector<Vector3f> m_components;
	unsigned int m_width;
	unsigned int m_height;
};


class CubeTexture
{
public:
	CubeTexture(const std::vector<std::string>& files);
	enum class Direction{LEFT,RIGHT,TOP,BOTTOM,FRONT,BACK};
public:
	Vector3f sample(const Vector3f& dir);
private:
	std::map<Direction, std::unique_ptr<Texture>> m_textures;
};

