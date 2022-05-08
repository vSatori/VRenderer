#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "VertexShader.h"
#include "PixelShader.h"
struct Light;
struct SkyBox;
struct Scene
{
public:
	std::vector<Mesh> meshes;
	std::vector<Mesh> reflectMeshes;
	std::vector<Light> lights;
	std::unique_ptr<SkyBox> sky;
	std::unique_ptr<DynamicCubeMap> envCubeMap;
	Camera camera;
	float nearPlane;
	float farPlane;
};

class Scene
{
public:
	virtual void render(unsigned int* frame) = 0;
protected:
	void drawMesh(const Mesh& mesh);
public:
	Camera camera;
	float nearPlane;
	float farPlane;
	float width;
	float height;
	float fov;
};

class DynamicEnviromentMappingScene : public Scene
{
public:
	virtual void render(unsigned int* frame);
private:
	std::unique_ptr<SkyBox> m_sky;
	std::unique_ptr<DynamicCubeMap> m_envCubeMap;
	std::vector<Mesh> m_movingSpheres;
	Mesh m_reflectSphere;
	GenericVertexShader* m_sphereVS;
	GenericPixelShader* m_spherePS;
	SkyVertexShader* m_skyVS;
	SkyPixelShader* m_skyPS;
};

