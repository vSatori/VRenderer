
#include <Windows.h>
#include "Scene.h"
#include "RenderContext.h"
#include "MeshFactory.h"
#include <Windows.h>
#include "Transform.h"
#include "Pmx.h"
#define MAXVRTEXSIZE 500000;
//static VertexOut vertexBuffer[500000];

static void perspectiveDivide(Vector4f& posH)
{
	posH.x /= posH.w;
	posH.y /= posH.w;
	posH.z /= posH.w;
}



Scene::Scene() : nearPlane(0.1f), farPlane(10.f), fov(90.f), m_frameFactor(0.f)
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
	m_reflectPS = new ReflectPixelShader;
	m_reflectPS->function = makeReflectPixelShader(m_reflectPS);

	MeshFactory::createCube(m_sky, 1.f, 1.f, 1.f);
	std::vector<std::string> files;
	files.push_back("D:/Project/Renderer/skybox/skybox/left.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/right.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/top.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/bottom.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/back.jpg");
	files.push_back("D:/Project/Renderer/skybox/skybox/front.jpg");
	m_sky.cubeMap = std::make_unique<CubeMap>(files);
	m_skyPS->cubeMap = m_sky.cubeMap.get();


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
	

	m_envCubeMap = std::make_unique<DynamicCubeMap>();
	m_reflectPS->envCubeMap = m_envCubeMap.get();

	nearPlane = 1.f;
	farPlane = 1000.f;
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
	delete m_reflectPS;
}

void DynamicEnviromentMappingScene::render()
{
	m_frameFactor += 2;
	if (m_frameFactor >= 360.f)
	{
		m_frameFactor = 0.f;
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
	Matrix4 projection = getProjectionMatrix();

	m_sphereVS->view = camera.matrix;
	m_sphereVS->projection = projection;
	m_sphereVS->vp = projection * camera.matrix;

	RenderContext::vs = m_sphereVS;
	if (drawRelect)
	{
		m_sphereVS->world.init();
		RenderContext::ps = m_reflectPS;
		drawMesh(m_reflectSphere);
	}
	RenderContext::ps = m_spherePS;
	for (int index = 0; index < m_movingSpheres.size(); ++index)
	{
		const auto& movingSphere = m_movingSpheres[index];
		float x = cosf(radian(m_frameFactor + index * 90.f)) * 6.f;
		float z = sinf(radian(m_frameFactor + index * 90.f)) * 6.f;
		m_sphereVS->world = Transform::translate(x, 0.f, z);
		m_spherePS->material = movingSphere.material;
		drawMesh(movingSphere);		
	}
	RenderContext::cullMode = CullMode::CULLNONE;
	RenderContext::posArea = true;
	RenderContext::vs = m_skyVS;
	RenderContext::ps = m_skyPS;
	m_skyVS->view = camera.matrix;
	m_skyVS->projection = projection;
	drawMesh(m_sky);
	RenderContext::posArea = false;
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

	m_sphere.material.ambient = { 1.f, 1.f, 1.f };
	m_sphere.material.diffuse = { 1.f, 0.f, 0.f };
	m_sphere.material.specular = { 0.5f, 0.5f, 0.5f };
	m_sphere.material.shininess = 32;

	m_ground.material.ambient = { 1.f, 1.f, 1.f };
	m_ground.material.diffuse = { 1.f, 1.f, 1.f };
	m_ground.material.specular = { 0.5f, 0.5f, 0.5f };
	m_ground.material.shininess = 32;

	DirectionalLight* left = new DirectionalLight;
	left->function = makeComputeDirectLightFunction(left);
	left->ambient = { 0.1f, 0.1f, 0.1f };
	left->diffuse = { 0.5f, 0.5f, 0.5f };
	left->specular = { 1.f, 1.f, 1.f };
	left->direction = { 0.f, -1.f, 1.f };
	left->pos = { 0.f, 100.f, -100.f };
	m_light = left;
	m_spherePS->light = m_light;

	m_depthTexture = new DepthTexture;
	m_spherePS->depthTexture = m_depthTexture;

	nearPlane = 1.f;
	farPlane = 1000.f;
	fov = 90.f;

	camera.useSphereMode = true;
	camera.radius = 10.f;
	camera.pos = { 0.f, 0.f, -5.f };
	camera.target = { 0.f, 0.f, 0.f };
}

ShadowMappingScene::~ShadowMappingScene()
{
}
#include <QtGui/qimage.h>
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
	/*
	unsigned int* data = new unsigned int[1280 * 720];
	for (int col = 0; col < RenderContext::height; ++col)
	{
		for (int row = 0; row < RenderContext::width; ++row)
		{
			int index = col * 1280 + row;
			float z = RenderContext::zbuffer[index];
			if (z > 1.f)
			{
				z = 0.95f;
			}
			int r = z * 255.f;
			int g = z * 255.f;
			int b = z * 255.f;
			if (col < 1 && row < 10)
			{
				qDebug() << r << g << b;
			}
			data[col * 1280 + row] = r << 16 + g << 8 + b;
		}
	}
	QImage image((uchar*)data, 1280, 720, QImage::Format::Format_RGB32);
	image.save("D:/depth.jpg");
	*/

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
	projection = getShadowProjectionMatrix(camera.matrix);
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
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;

	RenderContext::vs = m_sphereVS;
	RenderContext::ps = m_spherePS;

	Matrix4 projection = getProjectionMatrix();
	m_sphereVS->view = camera.matrix;
	m_sphereVS->projection = projection;
	m_sphereVS->vp = projection * camera.matrix;
	
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	tempCamera.pos = m_light->pos;
	tempCamera.update();
	m_sphereVS->shadow = getShadowProjectionMatrix(tempCamera.matrix) * tempCamera.matrix;
	
	m_sphereVS->world = Transform::translate(0.f, -1.7f, 0.f);
	m_sphereVS->world3 = Matrix4To3(m_sphereVS->world);
	//m_spherePS->color = Vector3f{ 0.9f, 0.9f, 0.9f };
	m_spherePS->material = m_ground.material;
	drawMesh(m_ground);

	m_sphereVS->world.init();
	m_sphereVS->world3 = Matrix4To3(m_sphereVS->world);
	//m_spherePS->color = Vector3f{ 1.f, 0.f, 0.f };
	m_spherePS->material = m_sphere.material;
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


std::string wstring2string(std::wstring wstr)
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

PmxModelScene::PmxModelScene()
{
	const char *filename = "D:/Project/other/models/bachong/∞À÷ÿ…Ò◊”.pmx";
	std::wstring prepath = L"D:\\Project\\other\\models\\bachong\\";
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
		m_keqing[i].indices.resize(faceSize);
		m_keqing[i].vertices = vertices;
		for (int j = 0; j < faceSize; ++j)
		{
			m_keqing[i].indices[j] = indices[currentIndx + j];
		}
		currentIndx += faceSize;
		m_keqing[i].texture = textures[material.diffuse_texture_index];
	}


	nearPlane = 0.1f;
	farPlane = 1000.f;

	camera.useSphereMode = true;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.radius = 20;
	camera.target = { 0.f, 16.f, 0.f };
	fov = 60.f;

	m_VS = new GenericVertexShader;
	m_VS->function = makeGenericVSFunction(m_VS);
	m_PS = new GenericPixelShader;
	m_PS->function = makeGenericPSFunction(m_PS);
	m_PS->color = { 0.f, 0.7f, 1.f };
	m_PS->light = nullptr;
	
	
}

void PmxModelScene::render()
{
	m_frameFactor += 2.f;
	if (m_frameFactor > 360.f)
	{
		m_frameFactor = 0.f;
	}
	RenderContext::clear();
	RenderContext::vs = m_VS;
	RenderContext::ps = m_PS;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::fillMode = FillMode::SOLID;
	camera.update();
	RenderContext::eyePos = camera.pos;

	m_VS->projection = getProjectionMatrix();
	m_VS->view = camera.matrix;
	m_VS->vp = m_VS->projection * m_VS->view;

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
		drawMesh(m_keqing[i], (VertexOut*)&buff[0]);
	}
	
}
#define MESH_RESOLUTION 64


// Mesh resolution
int N = MESH_RESOLUTION;
int M = MESH_RESOLUTION;
float L_x = 1000;
float L_z = 1000;


float A = 3e-7f;
// Wind speed
float V = 30;
// Wind direction
Vector2f omega(1, 1);
/*
OceanWaveScene::OceanWaveScene(): m_time(0.f), m_maxHeight(0.f), m_minHeight(0.f)
{
	m_wave = new Wave(N, M, L_x, L_z, omega, V, A, 1);
	int indexSize = (N - 1) * (M - 1) * 6;
	m_waveModel.vertices.resize(N * M);
	m_waveModel.indices.resize(indexSize);
	auto& indices = m_waveModel.indices;
	int index = 0;
	for (int j = 0; j < N - 1; ++j)
	{
		for (int i = 0; i < M - 1; ++i)
		{
			indices[++index] = { i + j * N, i + (j + 1) * N, (i + 1) + j * N };
			indices[++index] = { (i + 1) + j * N , i + (j + 1) * N, (i + 1) + (j + 1) * N };
		}
	}
	Material& material = m_waveModel.material;
	material.ambient = { 1.f, 1.f, 1.f };
	material.diffuse = { 1.f, 0.2f, 0.1f };
	material.specular = { 0.5f, 0.5f, 0.5f };
	material.shininess = 32.f;


	nearPlane = 0.1f;
	farPlane = 1000.f;

	camera.useSphereMode = true;
	camera.pitch = 45.f;
	camera.yaw = -90.f;
	camera.radius = 100;
	camera.target = { 0.f, 0.f, 0.f };
	fov = 60.f;


	m_light = new DirectionalLight;
	m_light->function = makeComputeDirectLightFunction(m_light);
	m_light->ambient = { 0.1f, 0.1f, 0.1f };
	m_light->diffuse = { 1.f, 1.f, 1.f };
	m_light->specular = { 1.f, 0.9f, 0.7f };
	m_light->direction = { 0.f, -1.f, 1.f };
	m_light->direction.normalize();
	m_light->pos = { 0.f, 50.f, 100.f };

	m_VS = new GenericVertexShader;
	m_VS->function = makeGenericVSFunction(m_VS);
	m_PS = new OceanWavePixelShader;
	m_PS->function = makeOceanWavePSFunction(m_PS);
	m_PS2 = new GenericPixelShader;
	m_PS2->function = makeGenericPSFunction(m_PS2);
	m_PS2->color = { 0.f, 0.7f, 1.f };
	m_PS->light = m_light;;
	m_PS2->light = m_light;
	RenderContext::vs = m_VS;
	RenderContext::ps = m_PS;
	//RenderContext::posArea = true;

	RenderContext::cullMode = CullMode::CULLBACKFACE;
	RenderContext::fillMode = FillMode::SOLID;
}

OceanWaveScene::~OceanWaveScene()
{
	delete m_wave;
	delete m_VS;
	delete m_PS;
	delete m_PS2;
}

void OceanWaveScene::render()
{
	m_time += 0.2;
	generateWave();
	RenderContext::clear();
	camera.update();
	
	RenderContext::eyePos = camera.pos;
	m_VS->world = Transform::scale(0.1f, 0.1f, 0.1f);
	m_VS->world3 = Matrix4To3(m_VS->world);
	m_VS->projection = getProjectionMatrix();
	m_VS->view = camera.matrix;
	m_VS->vp = m_VS->projection * m_VS->view;
	drawMesh(m_waveModel);
}
#include <QtCore/qdebug.h>
#undef max
void OceanWaveScene::generateWave()
{
	int vertexSize = N * M;
	m_wave->buildField(m_time);

	auto& vertices = m_waveModel.vertices;
	m_maxHeight = std::numeric_limits<float>::lowest();
	m_minHeight = std::numeric_limits<float>::max();

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < M; ++j)
		{
			int index = j * N + i;
			auto& vertex = vertices[index];
			vertex.pos = m_wave->heightField[index];		
			vertex.normal = m_wave->normalField[index];
			float height = m_wave->heightField[index].y;
			if (height > m_maxHeight)
			{
				m_maxHeight = height;
			}
			else if (height < m_minHeight)
			{
				m_minHeight = height;
			}
		}
	}
	m_PS->maxHeight = m_maxHeight * 0.1f;
	m_PS->minHeight = m_minHeight * 0.1f;
}
*/