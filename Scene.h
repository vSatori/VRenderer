#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "VertexShader.h"
#include "PixelShader.h"

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
	void drawMesh(const Mesh& mesh, const std::vector<VertexOut>& vertexBuff);
public:
	Camera camera;
	float nearPlane;
	float farPlane;
	float fov;
protected:
	int m_frameFactor;
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
private:
	SkyBox m_sky;
	std::unique_ptr<DynamicCubeMap> m_envCubeMap;
	std::vector<Mesh> m_movingSpheres;
	std::vector<Vector3f> m_sphereColors;
	Mesh m_reflectSphere;
	GenericVertexShader* m_sphereVS;
	GenericPixelShader* m_spherePS;
	SkyVertexShader* m_skyVS;
	SkyPixelShader* m_skyPS;
	Light* m_light;
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
	DirectionalLight* m_light;
	DepthTexture* m_depthTexture;
	Matrix4 m_lightWorld;
	Vector3f m_lightPosition;
};

class PmxModelScene : public Scene
{
public:
	PmxModelScene();
public:
	virtual void render()override;
private:
	std::vector<Mesh> m_keqing;
	GenericPixelShader* m_PS;
	GenericVertexShader* m_VS;
	DirectionalLight* m_light;
};

