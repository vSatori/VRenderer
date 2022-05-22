#include "Scene.h"
#include "RenderContext.h"
#include "MeshFactory.h"
#include <qdebug.h>
#include <Windows.h>
#include "Pmx.h"
#define MAXVRTEXSIZE 500000;
//static VertexOut vertexBuffer[500000];

static void perspectiveDivide(Vector4f& posH)
{
	posH.x /= posH.w;
	posH.y /= posH.w;
	posH.z /= posH.w;
}


void Scene::drawMesh(const Mesh & mesh)
{
	//auto vsBegin = GetTickCount64();
	int vertexSize = mesh.vertices.size();
	VertexOut* vertexBuff = new VertexOut[vertexSize];
	for (int vIndex = 0; vIndex < vertexSize; ++vIndex)
	{
		vertexBuff[vIndex] = RenderContext::vs->execute(mesh.vertices[vIndex]);
	}
	//qDebug() << "vertex shader: " << GetTickCount64() - vsBegin;
	//auto fsBegin = GetTickCount64();
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
				v.posH.x /= v.posH.w;
				v.posH.y /= v.posH.w;
				v.posH.z /= v.posH.w;
			}
			int size = vos.size() - 3 + 1;
			for (int j = 0; j < size; ++j)
			{
				RenderContext::drawFragment(vos[0], vos[j + 1], vos[j + 2]);
			}
		}
		else
		{
			perspectiveDivide(vo1.posH);
			perspectiveDivide(vo2.posH);
			perspectiveDivide(vo3.posH);
			RenderContext::drawFragment(vo1, vo2, vo3);
		}
		
	}
	//qDebug() << GetTickCount64() - fsBegin;
	delete[] vertexBuff;
}

void Scene::drawMesh(const Mesh & mesh, const std::vector<VertexOut>& vertexBuff)
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
				v.posH.x /= v.posH.w;
				v.posH.y /= v.posH.w;
				v.posH.z /= v.posH.w;
			}
			int size = vos.size() - 3 + 1;
			for (int j = 0; j < size; ++j)
			{
				RenderContext::drawFragment(vos[0], vos[j + 1], vos[j + 2]);
			}
		}
		else
		{
			perspectiveDivide(vo1.posH);
			perspectiveDivide(vo2.posH);
			perspectiveDivide(vo3.posH);
			RenderContext::drawFragment(vo1, vo2, vo3);
		}

	}
}









DynamicEnviromentMappingScene::DynamicEnviromentMappingScene()
{
	m_skyVS = new SkyVertexShader;
	m_skyPS = new SkyPixelShader;
	m_skyVS->function = makeSkyVSFunction(m_skyVS);
	m_skyPS->function = makeSkyPSFunction(m_skyPS);
	m_sphereVS = new GenericVertexShader;
	m_spherePS = new GenericPixelShader;
	m_sphereVS->function = makeGenericVSFunction(m_sphereVS);
	m_spherePS->function = makeGenericPSFunction(m_spherePS);
	MeshFactory::createCube(m_sky, 1.f, 1.f, 1.f);
	std::vector<std::string> files;
	files.push_back("D:/D3D12/models/skybox/skybox/left.bmp");
	files.push_back("D:/D3D12/models/skybox/skybox/right.bmp");
	files.push_back("D:/D3D12/models/skybox/skybox/top.bmp");
	files.push_back("D:/D3D12/models/skybox/skybox/bottom.bmp");
	files.push_back("D:/D3D12/models/skybox/skybox/back.bmp");
	files.push_back("D:/D3D12/models/skybox/skybox/front.bmp");
	m_sky.cubeMap = std::make_unique<CubeMap>(files);
	m_skyPS->cubeMap = m_sky.cubeMap.get();
	MeshFactory::createCube(m_reflectSphere, 2.f, 2.f, 2.f);
	m_movingSpheres.resize(4);
	for (int i = 0; i < 4; ++i)
	{
		MeshFactory::createSphere(m_movingSpheres[i], 0.5f, 30, 30);
	}
	m_spherePS->texture = m_movingSpheres[0].texture.get();
	m_sphereColors.resize(4);
	m_sphereColors[0] = { 1.f, 0.f, 0.f };
	m_sphereColors[1] = { 1.f, 1.f, 0.f };
	m_sphereColors[2] = { 0.f, 1.f, 0.f };
	m_sphereColors[3] = { 0.f, 0.f, 1.f };

	DirectionalLight* left = new DirectionalLight;
	left->function = makeComputeDirectLightFunction(left);
	left->ambient = { 0.1f, 0.1f, 0.1f };
	left->ambientFactor = 1.f;
	left->diffuse = { 0.5f, 0.5f, 0.5f };
	left->diffuseFactor = 1.f;
	left->specular = { 0.1f, 0.1f, 0.1f };
	left->specularFactor = 5.f;
	left->direction = { 0.f, -1.f, 1.f };
	left->pos = { 0.f, 100.f, -100.f };
	Vector3f poses[] = { {10.f, 0.f, 0.f},{0.f, 0.f, -10.f},{0.f, 0.f, 10.f} };
	Vector3f dirs[] = { {-1.f, 0.f, 0.f},{0.f, 0.f, 1.f}, {0.f, 0.f, -1.f} };
	m_light = left;
	m_spherePS->light = nullptr;
	m_envCubeMap = std::make_unique<DynamicCubeMap>();
	m_spherePS->envCubeMap = m_envCubeMap.get();

	nearPlane = 1.f;
	farPlane = 1000.f;
	RenderContext::nearPlane = nearPlane;
	RenderContext::farPlane = farPlane;
	fov = 90.f;

	camera.useSphereMode = true;
	camera.radius = 10.f;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.pos = { 0.f, 0.f, -5.f };
	camera.target = { 0.f, 0.f, 0.f };

	
}
DynamicEnviromentMappingScene::~DynamicEnviromentMappingScene()
{
	delete m_skyPS;
	delete m_skyVS;
	delete m_sphereVS;
	delete m_spherePS;
	delete m_light;
}
#include <qimage.h>
#include <qapplication.h>
void DynamicEnviromentMappingScene::render()
{
	m_frameFactor += 2;
	if (m_frameFactor >= 360.f)
	{
		m_frameFactor = 0.f;
	}

	render(false);
	qDebug() << RenderContext::allCount << RenderContext::count;
	return;

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
		//render(false);
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
	Matrix4 projection = getProjectionMatrix();

	m_sphereVS->view = camera.matrix;
	m_sphereVS->projection = projection;
	m_spherePS->eyePos = camera.pos;

	RenderContext::vs = m_sphereVS;
	RenderContext::ps = m_spherePS;

	if (drawRelect)
	{
		m_sphereVS->world.init();
		m_spherePS->reflect = true;
		//drawMesh(m_reflectSphere);
	}
	m_spherePS->reflect = false;
	for (int index = 0; index < m_movingSpheres.size(); ++index)
	{
		const auto& movingSphere = m_movingSpheres[index];
		world.init();
		world.m[0][3] = cosf(radian(m_frameFactor + index * 90.f)) * 6.f;
		world.m[2][3] = sinf(radian(m_frameFactor + index * 90.f)) * 6.f;
		m_sphereVS->world = world;
		m_spherePS->color = m_sphereColors[index];
		
		//m_spherePS->blend = true;
		//m_spherePS->alpha = 0.5f;
		//RenderContext::cullMode = CullMode::CULLNONE;
		
		//drawMesh(movingSphere);		
	}
	
	RenderContext::cullMode = CullMode::CULLNONE;
	m_skyVS->view = camera.matrix;
	m_skyVS->projection = projection;
	RenderContext::vs = m_skyVS;
	RenderContext::ps = m_skyPS;
	RenderContext::posArea = true;
	drawMesh(m_sky);
	RenderContext::posArea = false;
}


Scene::Scene()
{
}

Scene::~Scene()
{
}

Matrix4 Scene::getProjectionMatrix()
{
	Matrix4 mat;
	float tanVar = tan((fov / 2) * (3.14159 / 180.f));
	float ratio = (float)RenderContext::width / (float)RenderContext::height;
	mat.m[0][0] = 1.f / (ratio * tanVar);
	mat.m[1][1] = 1.f / tanVar;
	mat.m[2][2] = farPlane / (farPlane - nearPlane);
	mat.m[2][3] = nearPlane * farPlane / (nearPlane - farPlane);
	mat.m[3][2] = 1;
	mat.m[3][3] = 0;
	return mat;
}

Matrix4 Scene::getOrthogonalMatrix(float w, float h, float n, float f)
{
	Matrix4 mat;
	mat.m[0][0] = 2.f / w;
	mat.m[1][1] = 2.f / h;
	mat.m[2][2] = 1.f / (f - n);
	mat.m[2][3] = n / (n - f);
	return mat;
}

ShadowMappingScene::ShadowMappingScene()
{
	m_sphereVS = new GenericVertexShader;
	m_spherePS = new GenericPixelShader;
	m_sphereVS->function = makeGenericVSFunction(m_sphereVS);
	m_spherePS->function = makeGenericPSFunction(m_spherePS);
	m_shadowVS = new ShadowMapVertexShader;
	m_shadowVS->function = makeShadowMapVSFunction(m_shadowVS);
	MeshFactory::createSphere(m_sphere, 1.f, 30, 30);
	MeshFactory::createCube(m_ground, 5.f, 1.f, 5.f);
	DirectionalLight* left = new DirectionalLight;
	left->function = makeComputeDirectLightFunction(left);
	left->ambient = { 0.1f, 0.1f, 0.1f };
	left->ambientFactor = 1.f;
	left->diffuse = { 0.5f, 0.5f, 0.5f };
	left->diffuseFactor = 1.f;
	left->specular = { 0.1f, 0.1f, 0.1f };
	left->specularFactor = 5.f;
	left->direction = { 0.f, -1.f, 1.f };
	left->pos = { 0.f, 100.f, -100.f };
	m_depthTexture = new DepthTexture;
	m_light = left;
	m_spherePS->light = m_light;
	m_spherePS->depthTexture = m_depthTexture;

	nearPlane = 1.f;
	farPlane = 1000.f;
	RenderContext::nearPlane = nearPlane;
	RenderContext::farPlane = farPlane;
	fov = 90.f;

	camera.useSphereMode = true;
	camera.radius = 10.f;
	camera.pos = { 0.f, 0.f, -5.f };
	camera.target = { 0.f, 0.f, 0.f };
}

ShadowMappingScene::~ShadowMappingScene()
{
}

void ShadowMappingScene::render()
{
	m_frameFactor += 2;
	if (m_frameFactor > 360)
	{
		m_frameFactor = 0;
	}

	float x = cosf(radian(m_frameFactor)) * 10.f;
	float y = 10.f;
	float z = sinf(radian(m_frameFactor)) * 10.f;
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
	Matrix4 world;
	Matrix4 projection;
	Vector3f center = { 0.f, 0.f, 0.f };
	projection = getShadowProjectionMatrix(camera.matrix);
	m_shadowVS->view = camera.matrix;
	m_shadowVS->projection = projection;
	RenderContext::vs = m_shadowVS;
	world.m[1][3] = -1.7f;
	m_shadowVS->world = world;
	drawMesh(m_ground);
	m_shadowVS->world.init();
	drawMesh(m_sphere);
}

void ShadowMappingScene::renderScene()
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	Matrix4 world;
	Matrix4 projection = getProjectionMatrix();
	m_sphereVS->view = camera.matrix;
	m_sphereVS->projection = projection;
	m_spherePS->eyePos = camera.pos;
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	tempCamera.pos = m_light->pos;
	tempCamera.update();
	m_sphereVS->matLitView = tempCamera.matrix;
	m_sphereVS->shadowProjection = getShadowProjectionMatrix(tempCamera.matrix);
	RenderContext::vs = m_sphereVS;
	RenderContext::ps = m_spherePS;
	world.m[1][3] = -1.7f;
	m_sphereVS->world = world;
	m_spherePS->color = Vector3f{ 0.9f, 0.9f, 0.9f };
	drawMesh(m_ground);
	m_sphereVS->world.init();
	m_spherePS->color = Vector3f{ 1.f, 0.f, 0.f };
	drawMesh(m_sphere);
}

Matrix4 ShadowMappingScene::getShadowProjectionMatrix(const Matrix4& matView)
{
	Vector3f center{0.f, 0.f, 0.f};
	Vector4f centerView = matView * vector3To4(center);
	float n = centerView.z - 3;
	float f = centerView.z + 3;
	return getOrthogonalMatrix(20.f, 16.f, n, f);
}

#include <codecvt>
#include <Windows.h>
std::string wstring2string(std::wstring wstr)
{
	std::string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

PmxModelScene::PmxModelScene()
{
	const char *filename = "D:/D3D12/models/bachong/八重神子.pmx";
	std::wstring prepath = L"D:\\D3D12\\models\\bachong\\";
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
	m_keqing.resize(model.material_count);
	pmx::PmxMaterial* materials = model.materials.get();
	std::wstring* texturePaths = model.textures.get();
	int currentIndx = 0;
	for (int i = 0; i < model.material_count; ++i)
	{
		auto& material = materials[i];
		int faceSize = material.index_count / 3;
		m_keqing[i].indices.resize(faceSize);
		m_keqing[i].vertices = vertices;
		for (int j = 0; j < faceSize; ++j)
		{
			m_keqing[i].indices[j] = indices[currentIndx + j];
		}
		currentIndx += faceSize;

		std::wstring path = prepath + texturePaths[material.diffuse_texture_index];
		m_keqing[i].texture = std::make_unique<Texture>(wstring2string(path).c_str());

	}
	qDebug() << model.vertex_count << model.index_count / 3;

	nearPlane = 0.1f;
	farPlane = 1000.f;
	RenderContext::nearPlane = nearPlane;
	RenderContext::farPlane = farPlane;
	camera.useSphereMode = true;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.radius = 20;
	camera.target = { 0.f, 16.f, 0.f };
	fov = 60.f;

	m_light = new DirectionalLight;
	m_light->function = makeComputeDirectLightFunction(m_light);
	m_light->ambient = { 0.1f, 0.1f, 0.1f };
	m_light->ambientFactor = 1.f;
	m_light->diffuse = { 0.5f, 0.5f, 0.5f };
	m_light->diffuseFactor = 1.f;
	m_light->specular = { 0.1f, 0.1f, 0.1f };
	m_light->specularFactor = 5.f;
	m_light->direction = { 0.f, -1.f, 1.f };
	m_light->pos = { 0.f, 100.f, -100.f };

	m_VS = new GenericVertexShader;
	m_VS->function = makeGenericVSFunction(m_VS);
	m_PS = new GenericPixelShader;
	m_PS->function = makeGenericPSFunction(m_PS);
	m_PS->color = { 0.f, 0.7f, 1.f };
	m_PS->light = nullptr;
	
	RenderContext::vs = m_VS;
	RenderContext::ps = m_PS;

	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::fillMode = FillMode::SOLID;
}

void PmxModelScene::render()
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	m_VS->projection = getProjectionMatrix();
	m_VS->view = camera.matrix;
	int vertexSize = m_keqing[0].vertices.size();
	std::vector<VertexOut> buff;
	buff.resize(vertexSize);
	for (int vIndex = 0; vIndex < vertexSize; ++vIndex)
	{
		buff[vIndex] = RenderContext::vs->execute(m_keqing[0].vertices[vIndex]);
	}
	for (int i = 0; i < m_keqing.size(); ++i)
	{
		m_PS->texture = m_keqing[i].texture.get();
		drawMesh(m_keqing[i], buff);
	}
	
}
