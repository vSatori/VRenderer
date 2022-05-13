#include "Scene.h"
#include "RenderContext.h"
#include "MeshFactory.h"
#include <qdebug.h>
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
	MeshFactory::createCube(m_sky, 1.f, 1.f, 1.f);
	std::vector<std::string> files;
	files.push_back("D:/Project/Renderer/skybox/skybox/left.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/right.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/top.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/bottom.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/back.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/front.bmp");
	m_sky.cubeMap = std::make_unique<CubeMap>(files);
	m_skyPS->cubeMap = m_sky.cubeMap.get();
	MeshFactory::createSphere(m_reflectSphere, 2.f, 20, 20);
	MeshFactory::createCube(m_ground, 5.f, 1.f, 5.f);
	m_movingSpheres.resize(4);
	for (int i = 0; i < 4; ++i)
	{
		MeshFactory::createSphere(m_movingSpheres[i], 1.f, 30, 30);
		m_movingSpheres[i].texture = std::make_unique<Texture>("D:/Project/Renderer/skybox/skybox/left.bmp");
	}
	m_spherePS->texture = m_movingSpheres[0].texture.get();
	m_sphereColors.resize(4);
	m_sphereColors[0] = { 1.f, 0.f, 0.f };
	m_sphereColors[1] = { 1.f, 1.f, 0.f };
	m_sphereColors[2] = { 0.f, 1.f, 0.f };
	m_sphereColors[3] = { 0.f, 0.f, 1.f };

	m_lights.resize(1);
	DirectionalLight* left = new DirectionalLight;
	left->function = makeComputeDirectLightFunction(left);
	left->ambient = { 0.1f, 0.1f, 0.1f };
	left->ambientFactor = 1.f;
	left->diffuse = { 0.5f, 0.5f, 0.5f };
	left->diffuseFactor = 1.f;
	left->specular = { 0.1f, 0.1f, 0.1f };
	left->specularFactor = 5.f;
	left->direction = { 0.f, -1.f, 1.f };
	left->pos = { 0.f, 20.f, -20.f };
	Vector3f poses[] = { {10.f, 0.f, 0.f},{0.f, 0.f, -10.f},{0.f, 0.f, 10.f} };
	Vector3f dirs[] = { {-1.f, 0.f, 0.f},{0.f, 0.f, 1.f}, {0.f, 0.f, -1.f} };
	/*
	for (int i = 1; i < 1; ++i)
	{
		DirectionalLight* light = new DirectionalLight;
		memcpy(light, left, sizeof(DirectionalLight) - sizeof(light->function));
		light->direction = dirs[i - 1];
		light->pos = poses[i - 1];
		light->function = makeComputeDirectLightFunction(light);
		m_lights[i] = light;
	}
	*/
	m_depthTextures.resize(4);
	for (auto& tex : m_depthTextures)
	{
		tex = new DepthTexture;
	}

	m_lights[0] = left;
	m_spherePS->lights = m_lights;
	m_spherePS->depthTextures = m_depthTextures;
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
	for (Light* lit : m_lights)
	{
		delete lit;
	}
	for (DepthTexture* tex : m_depthTextures)
	{
		delete tex;
	}
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

	useOrthogonal = true;
	int index = 0;
	for (auto light : m_lights)
	{
		camera.target = { 0.f, 0.f, 0.f };
		camera.pos = light->pos;
		camera.useSphereMode = false;
		RenderContext::drawColor = false;
		render(false);
		m_depthTextures[index]->setRowData(RenderContext::zbuffer, RenderContext::width, RenderContext::height);
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
		++index;
	}
	useOrthogonal = false;
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
	Matrix4 projection;
	if (useOrthogonal)
	{
		projection = getProjectionMatrix();
	}
	else
	{
		projection = getProjectionMatrix();
	}
	
	m_sphereVS->view = camera.viewMat;
	m_sphereVS->projection = projection;
	m_spherePS->eyePos = camera.pos;
	Camera tempCamera = camera;
	tempCamera.useSphereMode = false;
	tempCamera.target = { 0.f, 0.f, 0.f };
	int size = m_lights.size();
	m_sphereVS->matLitViews.resize(size);
	for (int i = 0; i < size; ++i)
	{
		tempCamera.pos = m_lights[i]->pos;
		tempCamera.update();
		m_sphereVS->matLitViews[i] = tempCamera.viewMat;
	}
	
	
	//..
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
	mat.m[2][2] = (farPlane + nearPlane) / (farPlane - nearPlane);
	mat.m[2][3] = nearPlane * farPlane / (nearPlane - farPlane);
	mat.m[3][2] = 1;
	mat.m[3][3] = 0;
	return mat;
}

Matrix4 Scene::getOrthogonalMatrix()
{
	Matrix4 mat;

	mat.m[2][2] = 2.f / (farPlane - nearPlane);
	mat.m[2][3] = (nearPlane + farPlane) / (nearPlane - farPlane);
	mat.m[3][3] = 1;
	return mat;
}

