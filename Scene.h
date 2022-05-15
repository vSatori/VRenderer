#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "VertexShader.h"
#include "PixelShader.h"
struct Light;
struct SkyBox;
/*
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
*/

class Scene
{
public:
	Scene();
	~Scene();
public:
	virtual void render() = 0;
	Matrix4 getProjectionMatrix();
	Matrix4 getOrthogonalMatrix(float w, float h, float n, float f);
protected:
	void drawMesh(const Mesh& mesh);
public:
	Camera camera;
	float nearPlane;
	float farPlane;
	float fov;
	bool useOrthogonal;
	unsigned int frameFactor;
};

class DynamicEnviromentMappingScene : public Scene
{
public:
	DynamicEnviromentMappingScene();
	~DynamicEnviromentMappingScene();
public:
	virtual void render()override;
private:
	void render(bool drawRelect);
	void renderShadow();

private:
	SkyBox m_sky;
	std::unique_ptr<DynamicCubeMap> m_envCubeMap;
	std::vector<Mesh> m_movingSpheres;
	std::vector<Vector3f> m_sphereColors;
	Mesh m_reflectSphere;
	Mesh m_ground;
	GenericVertexShader* m_sphereVS;
	GenericPixelShader* m_spherePS;
	SkyVertexShader* m_skyVS;
	SkyPixelShader* m_skyPS;
	ShadowMapVertexShader* m_shadowVS;
	Light* m_light;
	DepthTexture* m_depthTexture;
	Matrix4 m_mat;
};

class ShadowMappingScene : public Scene
{
public:
	ShadowMappingScene();
	~ShadowMappingScene();
public:
	virtual void render()override;
private:
	void renderShadow();
	void renderScene();
	Matrix4 getShadowProjectionMatrix(const Matrix4& matView);
private:
	Mesh m_sphere;
	Mesh m_ground;
	GenericVertexShader* m_sphereVS;
	GenericPixelShader* m_spherePS;
	ShadowMapVertexShader* m_shadowVS;
	Light* m_light;
	DepthTexture* m_depthTexture;
};

