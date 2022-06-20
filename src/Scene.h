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
	void drawMesh(const Mesh& mesh, Fragment* vertexBuff);
public:
	Camera camera;
	float nearPlane;
	float farPlane;
	float fov;
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
	float m_angle;

	SkyBox m_sky;
	Mesh m_reflectSphere;
	std::vector<Mesh> m_movingSpheres;
	CubeMap* m_envCubeMap;
	
	ReflectPixelShader*  m_reflectPS;
	GenericVertexShader* m_commonVS;
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
	float m_angle;

	Mesh m_sphere;
	Mesh m_ground;
	
	GenericVertexShader*   m_commonVS;
	GenericPixelShader*    m_commonPS;
	ShadowMapVertexShader* m_shadowVS;

	DirectionalLight*      m_light;
	DepthTexture*          m_depthTexture;
	Matrix4                m_lightWorld;
	Vector3f               m_lightPosition;
};

class PmxModelScene : public Scene
{
public:
	PmxModelScene(const std::string& modelpath);
	~PmxModelScene();
public:
	virtual void render()override;
public:
	bool onlyDrawPmxModel;
private:
	float m_angle;

	std::vector<Mesh> m_model;
	Mesh m_bigBox;
	Mesh m_lightBox;

	GenericVertexShader* m_VS;
	GenericPixelShader*  m_PS;
	
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

	GenericVertexShader*  m_VS;
	OceanWavePixelShader* m_PS;

	DirectionalLight* m_light;
};
