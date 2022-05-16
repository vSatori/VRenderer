#include "Scene.h"
#include "RenderContext.h"
#include "MeshFactory.h"
#include <qdebug.h>
#include "Pmx.h"
void Scene::drawMesh(const Mesh & mesh)
{
	int vertexSize = mesh.vertices.size();
	VertexOut* vertexBuffer = new VertexOut[vertexSize];
	for (int vIndex = 0; vIndex < vertexSize; ++vIndex)
	{
		vertexBuffer[vIndex] = RenderContext::vs->execute(mesh.vertices[vIndex]);
	}
	int faceSize = mesh.indices.size();
	for (int faceIndex = 0; faceIndex < faceSize; ++faceIndex)
	{
		const Vector3i& face = mesh.indices[faceIndex];
		
		VertexOut vo1 = vertexBuffer[face.u];
		VertexOut vo2 = vertexBuffer[face.v];
		VertexOut vo3 = vertexBuffer[face.w];
		Vector3f faceNormal = (vo2.vin.pos - vo1.vin.pos).cross(vo3.vin.pos - vo2.vin.pos);
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
			float w1 = vo1.posH.w;
			float w2 = vo2.posH.w;
			float w3 = vo3.posH.w;
			vo1.posH /= vo1.posH.w;
			vo2.posH /= vo2.posH.w;
			vo3.posH /= vo3.posH.w;
			vo1.posH.w = w1;
			vo2.posH.w = w2;
			vo3.posH.w = w3;
			
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
	m_shadowVS = new ShadowMapVertexShader;
	m_shadowVS->function = makeShadowMapVSFunction(m_shadowVS);
	MeshFactory::createCube(m_sky, 1.f, 1.f, 1.f);
	std::vector<std::string> files;
	files.push_back("D:/Project/Renderer/skybox/skybox/left.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/right.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/top.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/bottom.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/back.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/front.bmp");
	m_sky.cubeMap = std::make_unique<CubeMap>(files);
	//m_skyPS->cubeMap = m_sky.cubeMap.get();
	MeshFactory::createSphere(m_reflectSphere, 2.f, 20, 20);
	MeshFactory::createCube(m_ground, 5.f, 1.f, 5.f);
	m_movingSpheres.resize(4);
	for (int i = 0; i < 4; ++i)
	{
		MeshFactory::createSphere(m_movingSpheres[i], 1.f, 30, 30);
		//m_movingSpheres[i].texture = std::make_unique<Texture>("D:/Project/Renderer/skybox/skybox/left.bmp");
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
	m_depthTexture = new DepthTexture;
	m_light = left;
	m_spherePS->light = m_light;
	m_spherePS->depthTexture = m_depthTexture;
	m_envCubeMap = std::make_unique<DynamicCubeMap>();
	m_spherePS->envCubeMap = m_envCubeMap.get();

	nearPlane = 1.f;
	farPlane = 1000.f;
	RenderContext::near = nearPlane;
	RenderContext::far = farPlane;
	fov = 90.f;

	camera.useSphereMode = true;
	camera.radius = 10.f;
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
	delete m_depthTexture;
}
#include <qimage.h>
#include <qapplication.h>
void DynamicEnviromentMappingScene::render()
{
	frameFactor += 2;
	if (frameFactor >= 360.f)
	{
		frameFactor = 0.f;
	}
	
	unsigned int* frameCache = RenderContext::renderTarget;
	Camera cameraCache = camera;
	float fovCache = fov;
	camera.target = { 0.f, 0.f, 0.f };
	camera.pos = m_light->pos;
	camera.useSphereMode = false;
	RenderContext::drawColor = false;
	renderShadow();
	m_depthTexture->setRowData(RenderContext::zbuffer, RenderContext::width, RenderContext::height);
	/*
	unsigned int* buff = new unsigned int[RenderContext::width * RenderContext::height];
	for (int h = 0; h < RenderContext::height; ++h)
	{
		for (int w = 0; w < RenderContext::width; ++w)
		{
			int cur = h * RenderContext::width + w;
			float depth = RenderContext::zbuffer[cur];
			if (depth < 1.f)
			{
				depth *= 0.3f;
			}
			Vector3f color{ 1.f, 1.f, 1.f };
			color *= depth;
			float* p = (float*)&color;
			for (int i = 0; i < 3; ++i)
			{
				if (*p > 1.f)
				{
					*p = 1.f;
				}
			}
			int r = color.x * 255.f;
			int g = color.y * 255.f;
			int b = color.z * 255.f;
			buff[cur] = (r << 16) + (g << 8) + b;
		}
	}

	QImage image((unsigned char*)buff, RenderContext::width, RenderContext::height, QImage::Format::Format_RGB32);
	image.save("D:/depth.jpg");
	exit(0);
	*/

	RenderContext::drawColor = true;
	/*
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
		delete buff;
	}
	*/
	RenderContext::renderTarget = frameCache;
	camera = cameraCache;
	fov = fovCache;
	
	render(false);
}

void DynamicEnviromentMappingScene::render(bool drawRelect)
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	Matrix4 world;
	Matrix4 projection = getProjectionMatrix();
	m_sphereVS->view = camera.viewMat;
	m_sphereVS->projection = projection;
	m_spherePS->eyePos = camera.pos;
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	
	tempCamera.pos = m_light->pos;
	tempCamera.update();
	m_sphereVS->matLitView = tempCamera.viewMat;

	m_sphereVS->shadowProjection = m_mat;
	
	RenderContext::vs = m_sphereVS;
	RenderContext::ps = m_spherePS;
	world.m[1][3] = -1.7f;
	m_sphereVS->world = world;
	m_spherePS->color = Vector3f{ 0.9f, 0.9f, 0.9f };
	drawMesh(m_ground);

	
	if (drawRelect)
	{
		m_sphereVS->world.init();
		m_spherePS->reflect = true;
		drawMesh(m_reflectSphere);
	}
	m_spherePS->reflect = false;
	for (int index = 0; index < m_movingSpheres.size(); ++index)
	{
		const auto& movingSphere = m_movingSpheres[index];
		world.init();
		//world.m[0][3] = cosf(radian(frameFactor + index * 90.f)) * 6.f;
		//world.m[2][3] = sinf(radian(frameFactor + index * 90.f)) * 6.f;
		m_sphereVS->world = world;
		m_spherePS->color = m_sphereColors[index];
		
		//m_spherePS->blend = true;
		//m_spherePS->alpha = 0.5f;
		//RenderContext::cullMode = CullMode::CULLNONE;
		
		drawMesh(movingSphere);
		break;
		
	}
	
	RenderContext::cullMode = CullMode::CULLNONE;
	m_skyVS->view = camera.viewMat;
	m_skyVS->projection = projection;
	RenderContext::vs = m_skyVS;
	RenderContext::ps = m_skyPS; 
	//drawMesh(m_sky);
}

void DynamicEnviromentMappingScene::renderShadow()
{
	RenderContext::clear();
	camera.update();
	RenderContext::eyePos = camera.pos;
	RenderContext::cullMode = CullMode::CULLBACKFACE;
	Matrix4 world;
	Matrix4 projection;
	Vector3f center = { 0.f, 0.f, 0.f };
	Vector4f centerView = camera.viewMat * center;
	float n = centerView.z - 3;
	float f = centerView.z + 3;
	projection = getOrthogonalMatrix(10.f, 8.f, n, f);
	m_mat = projection;
	m_shadowVS->view = camera.viewMat;
	m_shadowVS->projection = projection;
	RenderContext::vs = m_shadowVS;
	world.m[1][3] = -1.7f;
	m_shadowVS->world = world;
	drawMesh(m_ground);
	for (int index = 0; index < m_movingSpheres.size(); ++index)
	{
		const auto& movingSphere = m_movingSpheres[index];
		world.init();
		m_sphereVS->world = world;
		m_spherePS->color = m_sphereColors[index];
		drawMesh(movingSphere);
		break;
	}
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
	RenderContext::near = nearPlane;
	RenderContext::far = farPlane;
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
	frameFactor += 2;
	if (frameFactor > 360)
	{
		frameFactor = 0;
	}
	//frameFactor = 45;
	float x = cosf(radian(frameFactor)) * 10.f;
	float y = 10.f;
	float z = sinf(radian(frameFactor)) * 10.f;
	//x = 0;
	//y = 100;
	//z = -100;
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
	projection = getShadowProjectionMatrix(camera.viewMat);
	m_shadowVS->view = camera.viewMat;
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
	m_sphereVS->view = camera.viewMat;
	m_sphereVS->projection = projection;
	m_spherePS->eyePos = camera.pos;
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	tempCamera.pos = m_light->pos;
	tempCamera.update();
	m_sphereVS->matLitView = tempCamera.viewMat;
	m_sphereVS->shadowProjection = getShadowProjectionMatrix(tempCamera.viewMat);
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
	Vector4f centerView = matView * center;
	float n = centerView.z - 3;
	float f = centerView.z + 3;
	return getOrthogonalMatrix(20.f, 16.f, n, f);
}

PmxModelScene::PmxModelScene()
{
	const char *filename = "D:/Project/other/models/bachong/∞À÷ÿ…Ò◊”.pmx";
	pmx::PmxModel model;
	std::ifstream stream = std::ifstream(filename, std::ios_base::binary);
	model.Read(&stream);
	stream.close();
	pmx::PmxVertex* vertices = model.vertices.get();
	m_keqing.vertices.resize(model.vertex_count);
	for (int i = 0; i < model.vertex_count; ++i)
	{
		pmx::PmxVertex& pv = vertices[i];
		Vertex& vertex = m_keqing.vertices[i];
		memcpy(&vertex.pos, pv.positon, sizeof(float) * 3);
		memcpy(&vertex.normal, pv.normal, sizeof(float) * 3);
		memcpy(&vertex.tex, pv.uv, sizeof(float) * 2);
	}
	m_keqing.indices.resize(model.index_count / 3);
	int* indices = model.indices.get();
	for (int i = 0; i < model.index_count / 3; ++i)
	{
		Vector3i& index = m_keqing.indices[i];
		index.u = indices[i * 3 + 0];
		index.v = indices[i * 3 + 1];
		index.w = indices[i * 3 + 2];
	}
	
	

	nearPlane = 0.1f;
	farPlane = 1000.f;
	RenderContext::near = nearPlane;
	RenderContext::far = farPlane;
	camera.useSphereMode = true;
	camera.pitch = 0.f;
	camera.yaw = -90.f;
	camera.radius = 20;
	camera.target = { 0.f, 10.f, 0.f };
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
	m_PS->light = m_light;
	
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
	m_VS->view = camera.viewMat;
	drawMesh(m_keqing);

	
}
