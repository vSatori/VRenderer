#pragma once
#include <vector>
#include "MathUtility.h"
#include "Texture.h"
#include <memory>
struct Vertex
{
	Vector3f pos;
	Vector3f normal;
	Vector2f tex;
};

struct VertexOut
{
	Vertex vin;
	Vector2f point;
	float z;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Vector3i> indices;
	std::shared_ptr<Texture> texture;
};

