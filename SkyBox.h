#pragma once
#include <vector>
#include "MathUtility.h"
#include "Texture.h"
#include <memory>
struct SkyBox
{
public:
public:
	std::vector<Vector3f> vertices;
	std::vector<Vector3f> vertexNormals;
	std::vector<Vector3i> indices;
	std::unique_ptr<CubeTexture> texture;
};

