#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
struct Light;
struct SkyBox;
struct Scene
{
public:
	std::vector<Mesh> meshes;
	std::vector<Light> lights;
	std::unique_ptr<SkyBox> sky;
	Camera camera;
	float nearPlane;
	float farPlane;
};

