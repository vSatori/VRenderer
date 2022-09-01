#pragma once
#include "Mesh.h"
class MeshFactory
{
public:
	static void createCube(Mesh& mesh, float length = 1.f, float width = 1.f, float height = 1.f);

	static void createSphere(Mesh& mesh, float radius, int level, int slice);

	static void flipMesh(Mesh& mesh);
};

