
#include <Windows.h>
#include "Scene.h"
#include "RenderContext.h"
#include "MeshFactory.h"
#include "Transform.h"
#include "MikuMikuFormats/Pmx.h"


static void perspectiveDivide(Vector4f& posH)
{
	posH.x /= posH.w;
	posH.y /= posH.w;
	posH.z /= posH.w;
}



Scene::Scene() : nearPlane(0.1f), farPlane(10.f), fov(90.f)
{
}

Scene::~Scene()
{
}




void Scene::drawMesh(const Mesh & mesh)
{
	int vertexSize = mesh.vertices.size();
	VertexOut* vertexBuff = new VertexOut[vertexSize];
	for (int vIndex = 0; vIndex < vertexSize; ++vIndex)
	{
		vertexBuff[vIndex] = RenderContext::vs->execute(mesh.vertices[vIndex]);
	}
	drawMesh(mesh, vertexBuff);
	delete[] vertexBuff;
}

void Scene::drawMesh(const Mesh& mesh, VertexOut* vertexBuff)
{
	auto vsBegin = GetTickCount64();
	int faceSize = mesh.indices.size();
	for (int faceIndex = 0; faceIndex < faceSize; ++faceIndex)
	{
		const Vector3i& face = mesh.indices[faceIndex];
		VertexOut vo1 = vertexBuff[face.x];
		VertexOut vo2 = vertexBuff[face.y];
		VertexOut vo3 = vertexBuff[face.z];
		Vector3f faceNormal = (vo2.posW - vo1.posW).cross(vo3.posW - vo2.posW);
		if (!RenderContext::Cull(vo1.posW, faceNormal))
		{
			continue;
		}
		if (RenderContext::checkClipping(vo1.posH, vo2.posH, vo3.posH))
		{
			std::vector<VertexOut> vos = RenderContext::polygonClipping(vo1, vo2, vo3);
			for (auto& v : vos)
			{
				perspectiveDivide(v.posH);
			}
			int size = vos.size() - 3 + 1;
			for (int j = 0; j < size; ++j)
			{
				RenderContext::draw(vos[0], vos[j + 1], vos[j + 2]);
			}
		}
		else
		{
			perspectiveDivide(vo1.posH);
			perspectiveDivide(vo2.posH);
			perspectiveDivide(vo3.posH);
			RenderContext::draw(vo1, vo2, vo3);
		}
	}
}


DynamicEnviromentMappingScene::DynamicEnviromentMappingScene() : 
	m_angle(0.f),
	m_envCubeMap(nullptr),
	m_reflectPS (nullptr),
	m_commonVS  (nullptr),
	m_spherePS  (nullptr),
	m_skyVS     (nullptr),
	m_skyPS     (nullptr)

{
	MeshFactory::createCube(m_sky, 1.f, 1.f, 1.f);
	std::vector<std::string> files;
	files.push_back("D:/Project/Renderer/skybox/skybox/left.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/right.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/top.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/bottom.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/back.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/front.jpg");
	m_sky.cubeMap = std::make_unique<StaticCubeMap>(files);

	std::vector<Vector3f> colors;
	colors.resize(4);
	colors[0] = { 1.f, 0.f, 0.f };
	colors[1] = { 1.f, 1.f, 0.f };
	colors[2] = { 0.f, 1.f, 0.f };
	colors[3] = { 0.f, 0.f, 1.f };
	MeshFactory::createCube(m_reflectSphere, 2.f, 2.f, 2.f);
	m_movingSpheres.resize(4);
	for (int i = 0; i < 4; ++i)
	{
		Mesh& sphere = m_movingSpheres[i];
		MeshFactory::createSphere(sphere, 0.5f, 30, 30);
		m_movingSpheres[i].material.diffuse = colors[i];
	}
	
	m_envCubeMap = new DynamicCubeMap;
	
	m_reflectPS = new ReflectPixelShader;
	m_commonVS  = new GenericVertexShader;
	m_spherePS  = new GenericPixelShader;
	m_skyVS     = new SkyVertexShader;
	m_skyPS     = new SkyPixelShader;

	m_reflectPS->envCubeMap = m_envCubeMap;
	m_skyPS->cubeMap = m_sky.cubeMap.get();

	camera.useSphereMode = true;
	camera.radius = 10.f;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.pos = { 0.f, 0.f, -5.f };
	camera.target = { 0.f, 0.f, 0.f };

	nearPlane = 1.f;
	farPlane = 1000.f;
	fov = 90.f;
}
DynamicEnviromentMappingScene::~DynamicEnviromentMappingScene()
{
	delete m_reflectPS;
	delete m_commonVS;
	delete m_spherePS;
	delete m_skyPS;
	delete m_skyVS;
}
	

void DynamicEnviromentMappingScene::render()
{
	m_angle += 2;
	if (m_angle >= 360.f)
	{
		m_angle = 0.f;
	}

	unsigned int* frameCache = RenderContext::renderTarget;
	Camera cameraCache = camera;
	float fovCache = fov;

	camera.useSphereMode = false;
	camera.pos = { 0.f, 0.f, 0.f };
	Vector3f targets[6] =
	{
		{-1.f, 0.f, 0.f},
		{1.f, 0.f, 0.f},
		{0.f, 1.f, 0.01f},
		{0.f, -1.f, 0.01f},
		{0.f, 0.f, -1.f},
		{0.f, 0.f, 1.f},
	};
	for (int i = 0; i < 6; ++i)
	{
		unsigned int* buff = new unsigned int[RenderContext::width * RenderContext::height];
		RenderContext::renderTarget = buff;
		camera.target = targets[i];
		render(false);
		m_envCubeMap->setRowData(static_cast<DynamicCubeMap::Direction>(i), buff, RenderContext::width, RenderContext::height);
		delete[] buff;
	}
	
	RenderContext::renderTarget = frameCache;
	camera = cameraCache;
	fov = fovCache;
	
	render(true);
}

void DynamicEnviromentMappingScene::render(bool drawRelect)
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	Matrix4 world;
	Matrix4 projection = Matrix4::perspectiveProjection(RenderContext::width, RenderContext::height, fov, nearPlane, farPlane);

	m_commonVS->view = camera.matrix;
	m_commonVS->projection = projection;
	m_commonVS->vp = projection * camera.matrix;

	RenderContext::vs = m_commonVS;
	if (drawRelect)
	{
		m_commonVS->world.init();
		RenderContext::ps = m_reflectPS;
		drawMesh(m_reflectSphere);
	}
	RenderContext::ps = m_spherePS;
	for (int index = 0; index < m_movingSpheres.size(); ++index)
	{
		const auto& movingSphere = m_movingSpheres[index];
		float x = cosf(radian(m_angle + index * 90.f)) * 6.f;
		float z = sinf(radian(m_angle + index * 90.f)) * 6.f;
		m_commonVS->world = Transform::translate(x, 0.f, z);
		m_spherePS->material = movingSphere.material;
		drawMesh(movingSphere);		
	}
	RenderContext::cullMode = CullMode::CULLNONE;
	RenderContext::clockwise = true;
	RenderContext::vs = m_skyVS;
	RenderContext::ps = m_skyPS;
	m_skyVS->view = camera.matrix;
	m_skyVS->projection = projection;
	drawMesh(m_sky);
	RenderContext::clockwise = false;
}

ShadowMappingScene::ShadowMappingScene() : 
	m_angle(0.f),
	m_commonVS    (nullptr),
	m_commonPS    (nullptr),
	m_shadowVS    (nullptr),
	m_light       (nullptr),
	m_depthTexture(nullptr)
{
	
	MeshFactory::createSphere(m_sphere, 1.f, 30, 30);
	MeshFactory::createCube(m_ground, 5.f, 1.f, 5.f);

	m_sphere.material.ambient   =  { 1.f, 0.f, 0.f };
	m_sphere.material.diffuse   = { 1.f, 0.f, 0.f };
	m_sphere.material.specular  = { 0.5f, 0.5f, 0.5f };
	m_sphere.material.shininess = 32.f;

	m_ground.material.ambient   = { 1.f, 1.f, 1.f };
	m_ground.material.diffuse   = { 1.f, 1.f, 1.f };
	m_ground.material.specular  = { 0.5f, 0.5f, 0.5f };
	m_ground.material.shininess = 32.f;

	DirectionalLight* light = new DirectionalLight;
	light->ambient   = { 0.2f, 0.2f, 0.2f };
	light->diffuse   = { 0.5f, 0.5f, 0.5f };
	light->specular  = { 1.f, 1.f, 1.f };
	light->direction = { 0.f, -1.f, 1.f };
	light->pos       = { 0.f, 100.f, -100.f };
	m_light = light;
	
	m_depthTexture = new DepthTexture;
	
	m_commonVS = new GenericVertexShader;
	m_commonPS = new GenericPixelShader;
	m_shadowVS = new ShadowMapVertexShader;

	m_commonPS->light = m_light;
	m_commonPS->depthTexture = m_depthTexture;

	camera.useSphereMode = true;
	camera.radius = 10.f;
	camera.pos = { 0.f, 0.f, -5.f };
	camera.target = { 0.f, 0.f, 0.f };

	nearPlane = 1.f;
	farPlane = 1000.f;
	fov = 90.f;
}

ShadowMappingScene::~ShadowMappingScene()
{
	delete m_commonVS;
	delete m_commonPS;
	delete m_shadowVS;
	delete m_light;
	delete m_depthTexture;
}

void ShadowMappingScene::render()
{
	m_angle += 1;
	if (m_angle > 360)
	{
		m_angle = 0;
	}

	float x = cosf(radian(m_angle)) * 10.f;
	float y = 10.f;
	float z = sinf(radian(m_angle)) * 10.f;
	m_light->pos = { x, y, z };
	m_light->direction = { -x, -y, -z };
	m_light->direction.normalize();

	Camera cameraCache = camera;
	float fovCache = fov;

	camera.target = { 0.f, 0.f, 0.f };
	camera.pos = m_light->pos;
	camera.useSphereMode = false;

	RenderContext::drawColor = false;
	renderShadow();
	m_depthTexture->setRowData(RenderContext::zbuffer, RenderContext::width, RenderContext::height);
	RenderContext::drawColor = true;

	camera = cameraCache;
	fov = fovCache;

	renderScene();
}

void ShadowMappingScene::renderShadow()
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::vs = m_shadowVS;

	Matrix4 projection;
	projection = shadowProjection(camera.matrix);
	m_shadowVS->view = camera.matrix;
	m_shadowVS->projection = projection;
	m_shadowVS->vp = projection * camera.matrix;
	
	m_shadowVS->world = Transform::translate(0.f, -1.7f, 0.f);
	drawMesh(m_ground);
	m_shadowVS->world.init();
	drawMesh(m_sphere);

	
}

void ShadowMappingScene::renderScene()
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos   = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::fillMode = FillMode::SOLID;

	RenderContext::vs = m_commonVS;
	RenderContext::ps = m_commonPS;

	Matrix4 projection = Matrix4::perspectiveProjection(RenderContext::width, RenderContext::height, fov, nearPlane, farPlane);
	m_commonVS->view = camera.matrix;
	m_commonVS->projection = projection;
	m_commonVS->vp = projection * camera.matrix;
	
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	tempCamera.pos = m_light->pos;
	tempCamera.update();
	m_commonVS->shadow = shadowProjection(tempCamera.matrix) * tempCamera.matrix;
	
	m_commonVS->world = Transform::translate(0.f, -1.7f, 0.f);
	m_commonVS->world3 = Matrix4To3(m_commonVS->world);
	m_commonPS->material = m_ground.material;
	drawMesh(m_ground);

	m_commonVS->world.init();
	m_commonVS->world3 = Matrix4To3(m_commonVS->world);
	m_commonPS->material = m_sphere.material;
	drawMesh(m_sphere);
}

Matrix4 ShadowMappingScene::shadowProjection(const Matrix4& matView)
{
	Vector3f center{0.f, 0.f, 0.f};
	Vector4f centerView = matView * vector3To4(center);
	float n = centerView.z - 3;
	float f = centerView.z + 3;
	return Matrix4::orthogonalProjection(20.f, 16.f, n, f);
}


static std::string wstring2string(std::wstring wstr)
{
	std::string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

static std::wstring string2wstring(std::string sToMatch)
{
	int iWLen = MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), 0, 0);
	wchar_t* lpwsz = new wchar_t[iWLen + 1];
	MultiByteToWideChar(CP_ACP, 0, sToMatch.c_str(), sToMatch.size(), lpwsz, iWLen);
	lpwsz[iWLen] = L'\0';
	std::wstring wsToMatch(lpwsz);
	delete[]lpwsz;
	return wsToMatch;
}

PmxModelScene::PmxModelScene(const std::string& modelpath) : 
	m_angle(0.f),
	m_VS   (nullptr),
	m_PS   (nullptr),
	m_light(nullptr)
{
	const char* filename = modelpath.c_str();
	int count = modelpath.find_last_of('/');
	std::wstring prepath = string2wstring(modelpath.substr(0, count));
	prepath.push_back('/');
	pmx::PmxModel model;
	std::ifstream stream = std::ifstream(filename, std::ios_base::binary);
	model.Read(&stream);
	stream.close();
	pmx::PmxVertex* pmxvertices = model.vertices.get();
	std::vector<Vertex> vertices;
	vertices.resize(model.vertex_count);
	for (int i = 0; i < model.vertex_count; ++i)
	{
		pmx::PmxVertex& pv = pmxvertices[i];
		Vertex& vertex = vertices[i];
		memcpy(&vertex.pos, pv.positon, sizeof(float) * 3);
		memcpy(&vertex.normal, pv.normal, sizeof(float) * 3);
		memcpy(&vertex.tex, pv.uv, sizeof(float) * 2);
	}
	std::vector<Vector3i> indices;
	indices.resize(model.index_count / 3);
	int* pmxindices = model.indices.get();
	for (int i = 0; i < model.index_count / 3; ++i)
	{
		Vector3i& index = indices[i];
		index.x = pmxindices[i * 3 + 0];
		index.y = pmxindices[i * 3 + 1];
		index.z = pmxindices[i * 3 + 2];
	}
	m_model.resize(model.material_count);
	pmx::PmxMaterial* materials = model.materials.get();
	std::wstring* texturePaths = model.textures.get();
	std::vector<std::shared_ptr<Texture>> textures;
	textures.resize(model.texture_count);
	for (int i = 0; i < model.texture_count; ++i)
	{
		std::wstring path = prepath + texturePaths[i];
		textures[i] = std::make_shared<Texture>(wstring2string(path).c_str());
	}
	int currentIndx = 0;
	for (int i = 0; i < model.material_count; ++i)
	{
		auto& material = materials[i];
		int faceSize = material.index_count / 3;
		m_model[i].indices.resize(faceSize);
		m_model[i].vertices = vertices;
		for (int j = 0; j < faceSize; ++j)
		{
			m_model[i].indices[j] = indices[currentIndx + j];
		}
		currentIndx += faceSize;
		m_model[i].texture = textures[material.diffuse_texture_index];
	}

	MeshFactory::createCube(m_bigBox, 40.f, 40.f, 40.f);
	MeshFactory::flipMesh(m_bigBox);
	m_bigBox.material.ambient = { 1.f, 1.f, 0.f };
	m_bigBox.material.diffuse = { 1.f, 1.f, 0.f };
	m_bigBox.material.specular = { 0.5f, 0.5f, 0.5f };
	m_bigBox.material.shininess = 32.f;

	MeshFactory::createCube(m_lightBox, 0.2f, 0.2f, 0.2f);

	nearPlane = 1.f;
	farPlane = 1000.f;

	camera.useSphereMode = true;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.radius = 20;
	camera.target = { 0.f, 10.f, 0.f };
	fov = 75.f;

	PointLight* lit = new PointLight;
	lit->ambient = { 0.2f, 0.2f, 0.2f };
	lit->constant = 0.1f;
	lit->linear = 0.1f;
	lit->quadratic = 0.0016f;
	m_light = lit;

	m_VS = new GenericVertexShader;
	m_PS = new GenericPixelShader;
	m_PS->color = { 0.f, 0.7f, 1.f };

	onlyDrawPmxModel = false;
	m_angle = 0.f;
}

PmxModelScene::~PmxModelScene()
{
	delete m_VS;
	delete m_PS;
	delete m_light;
}

void PmxModelScene::render()
{
	m_angle += 1;
	if (m_angle > 360)
	{
		m_angle = 0;
	}
	if (m_angle > 180)
	{
		m_light->diffuse = { 1.0f, 0.0f, 0.0f };
		m_light->specular = { 1.0f, 1.f, 1.0f };
		m_lightBox.material.diffuse = { 1.f, 0.f, 0.f };

	}
	else
	{
		m_light->diffuse = { 0.0f, 0.9f, 0.2f };
		m_light->specular = { 0.0f, 1.f, 0.0f };
		m_lightBox.material.diffuse = { 0.f, 0.9f, 0.2f };
	}

	float x = cosf(radian(m_angle)) * 8.f;
	float y = 16.f;
	float z = sinf(radian(m_angle)) * 8.f;
	m_light->pos = { x, y, z };

	RenderContext::clear();
	RenderContext::vs = m_VS;
	RenderContext::ps = m_PS;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::fillMode = FillMode::SOLID;
	camera.update();
	RenderContext::eyePos = camera.pos;

	m_VS->projection = Matrix4::perspectiveProjection(RenderContext::width, RenderContext::height, fov, nearPlane, farPlane);
	m_VS->view = camera.matrix;
	m_VS->vp = m_VS->projection * m_VS->view;
	m_PS->light = onlyDrawPmxModel ? nullptr : m_light;

	int vertexSize = m_model[0].vertices.size();
	std::vector<VertexOut> buff;
	buff.resize(vertexSize);
	for (int vIndex = 0; vIndex < vertexSize; ++vIndex)
	{
		buff[vIndex] = RenderContext::vs->execute(m_model[0].vertices[vIndex]);
	}
	
	for (int i = 0; i < m_model.size(); ++i)
	{
		m_PS->texture = m_model[i].texture.get();
		drawMesh(m_model[i], (VertexOut*)&buff[0]);
	}
	if (onlyDrawPmxModel)
	{
		return;
	}
	m_VS->world = Transform::translate(0.f, 20.f, 0.f);
	m_PS->texture = nullptr;
	m_PS->material = m_bigBox.material;
	drawMesh(m_bigBox);
	m_VS->world.init();
	m_PS->texture = nullptr;
	m_PS->material = m_bigBox.material;

	m_VS->world = Transform::translate(x, y, z);
	m_PS->texture = nullptr;
	m_PS->material = m_lightBox.material;
	m_PS->light = nullptr;
	drawMesh(m_lightBox);
	m_VS->world.init();
	m_PS->texture = nullptr;
	m_PS->material = m_lightBox.material;
	m_PS->light = m_light;
}



OceanWaveScene::OceanWaveScene() : 
	m_wave(nullptr),
	m_time      (0.f),
	m_maxHeight(0.f),
	m_minHeight(0.f),
	m_VS       (nullptr),
	m_PS       (nullptr),
	m_light    (nullptr)
{
	Vector2f wind{ 1.f, 1.f };
	wind.normalize();
	m_wave = new OceanWave(64, 64, 3e-7f, 1000.f, wind, 20.f);

	m_light = new DirectionalLight;
	m_light->ambient = { 0.1f, 0.1f, 0.1f };
	m_light->diffuse = { 1.f, 1.f, 1.f };
	m_light->specular = { 1.f, 0.9f, 0.7f };
	m_light->direction = { 0.f, -1.f, 1.f };
	m_light->direction.normalize();
	m_light->pos = { 0.f, 50.f, 100.f };

	m_VS = new GenericVertexShader;
	m_PS = new OceanWavePixelShader;

	m_PS->light = m_light;;

	camera.useSphereMode = true;
	camera.pitch = 45.f;
	camera.yaw = -90.f;
	camera.radius = 100;
	camera.target = { 0.f, 0.f, 0.f };

	nearPlane = 0.1f;
	farPlane = 1000.f;
	fov = 60.f;
}

OceanWaveScene::~OceanWaveScene()
{
	delete m_wave;
	delete m_VS;
	delete m_PS;
}

void OceanWaveScene::render()
{
	m_time += 0.1;
	generateWave();
	RenderContext::clear();
	camera.update();

	RenderContext::vs = m_VS;
	RenderContext::ps = m_PS;
	RenderContext::eyePos = camera.pos;

	m_VS->world = Transform::scale(0.1f, 0.1f, 0.1f);
	m_VS->world3 = Matrix4To3(m_VS->world);
	m_VS->projection = Matrix4::perspectiveProjection(RenderContext::width, RenderContext::height, fov, nearPlane, farPlane);
	m_VS->view = camera.matrix;
	m_VS->vp = m_VS->projection * m_VS->view;

	drawMesh(m_wave->wave);
}

void OceanWaveScene::generateWave()
{
	m_wave->update(m_time);
	m_PS->maxHeight = m_wave->maxHeight * 0.1f;
	m_PS->minHeight = m_wave->minHeight * 0.1f;
}
