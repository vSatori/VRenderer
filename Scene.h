#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "VertexShader.h"
#include "PixelShader.h"
#

class Scene
{
public:
	Scene();
	~Scene();
public:
	virtual void render() = 0;
protected:
	void drawMesh(const Mesh& mesh);
	void drawMesh(const Mesh& mesh, VertexOut* vertexBuff);
public:
	Camera camera;
	float nearPlane;
	float farPlane;
	float fov;
protected:
	float m_frameFactor;
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
	Mesh m_reflectSphere;
	std::vector<Mesh> m_movingSpheres;
	std::unique_ptr<DynamicCubeMap> m_envCubeMap;
	ReflectPixelShader*  m_reflectPS;
	GenericVertexShader* m_sphereVS;
	GenericPixelShader*  m_spherePS;
	SkyVertexShader*     m_skyVS;
	SkyPixelShader*      m_skyPS;
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
	Matrix4 shadowProjection(const Matrix4& matView);
private:
	Mesh m_sphere;
	Mesh m_ground;
	GenericVertexShader*   m_sphereVS;
	GenericPixelShader*    m_spherePS;
	ShadowMapVertexShader* m_shadowVS;
	DirectionalLight*      m_light;
	DepthTexture*          m_depthTexture;
	Matrix4                m_lightWorld;
	Vector3f               m_lightPosition;
};

class PmxModelScene : public Scene
{
public:
	PmxModelScene();
public:
	virtual void render()override;
public:
	bool onlyDrawPmxModel;
private:
	std::vector<Mesh> m_model;
	Mesh m_bigBox;
	Mesh m_lightBox;
	GenericPixelShader* m_PS;
	GenericVertexShader* m_VS; 
	PointLight* m_light;
};


#include "OceanWave.h"
class OceanWaveScene : public Scene
{
public:
	OceanWaveScene();
	~OceanWaveScene();
public:
	virtual void render()override;
private:
	void generateWave();
private:
	OceanWave* m_wave;
	float m_time;
	float m_maxHeight;
	float m_minHeight;
	GenericVertexShader* m_VS;
	OceanWavePixelShader* m_PS;
	GenericPixelShader* m_PS2;
	DirectionalLight* m_light;
};
