#include "Renderer.h"
#include <QtWidgets/QApplication>
#include <qtimer.h>
#include <Windows.h>
#include <qtimer.h>
#include "SkyBox.h"
#include "Scene.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Renderer w;
    w.show();
	std::vector<std::string> files;
	/*
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_negX.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_posX.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_posY.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_negY.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_negZ.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/sunset_posZ.bmp");
	*/
	files.push_back("D:/Project/Renderer/skybox/skybox/left.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/right.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/top.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/bottom.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/back.bmp");
	files.push_back("D:/Project/Renderer/skybox/skybox/front.bmp");
	Scene scene;
	scene.sky = std::make_unique<SkyBox>();
	scene.sky->vertices.resize(36);
	scene.sky->indices.resize(12);
	scene.sky->vertexNormals.resize(36);
	scene.sky->vertices[0] = { -0.5, 0.5, -0.5 };
	scene.sky->vertices[1] = { -0.5, -0.5, -0.5 };
	scene.sky->vertices[2] = { 0.5, -0.5, -0.5 };
	scene.sky->vertices[3] = { -0.5, 0.5, -0.5 };
	scene.sky->vertices[4] = { 0.5, -0.5, -0.5 };
	scene.sky->vertices[5] = { 0.5, 0.5, -0.5 };
	
	scene.sky->vertices[6] = { -0.5, 0.5, 0.5 };
	scene.sky->vertices[7] = { -0.5, -0.5, 0.5 };
	scene.sky->vertices[8] = { -0.5, -0.5, -0.5 };
	scene.sky->vertices[9] = { -0.5, 0.5, 0.5 };
	scene.sky->vertices[10] = { -0.5, -0.5, -0.5 };
	scene.sky->vertices[11] = { -0.5, 0.5, -0.5 };
	

	scene.sky->vertices[12] = { 0.5, 0.5, 0.5 };
	scene.sky->vertices[13] = { 0.5, -0.5, 0.5 };
	scene.sky->vertices[14] = { -0.5, -0.5, 0.5 };
	scene.sky->vertices[15] = { 0.5, 0.5, 0.5 };
	scene.sky->vertices[16] = { -0.5, -0.5, 0.5 };
	scene.sky->vertices[17] = { -0.5, 0.5, 0.5 };
	

	scene.sky->vertices[18] = { 0.5, 0.5, -0.5 };
	scene.sky->vertices[19] = { 0.5, -0.5, -0.5 };
	scene.sky->vertices[20] = { 0.5, -0.5, 0.5 };
	scene.sky->vertices[21] = { 0.5, 0.5, -0.5 };
	scene.sky->vertices[22] = { 0.5, -0.5, 0.5 };
	scene.sky->vertices[23] = { 0.5, 0.5, 0.5 };
	

	scene.sky->vertices[24] = { -0.5, 0.5, 0.5 };
	scene.sky->vertices[25] = { -0.5, 0.5, -0.5 };
	scene.sky->vertices[26] = { 0.5, 0.5, -0.5 };
	scene.sky->vertices[27] = { -0.5, 0.5, 0.5 };
	scene.sky->vertices[28] = { 0.5, 0.5, -0.5 };
	scene.sky->vertices[29] = { 0.5, 0.5, 0.5 };
	
	scene.sky->vertices[30] = { 0.5, -0.5, 0.5 };
	scene.sky->vertices[31] = { 0.5, -0.5, -0.5 };
	scene.sky->vertices[32] = { -0.5, -0.5, -0.5 };
	scene.sky->vertices[33] = { 0.5, -0.5, 0.5 };
	scene.sky->vertices[34] = { -0.5, -0.5, -0.5 };
	scene.sky->vertices[35] = { -0.5, -0.5, 0.5 };

	for (int i = 0; i < 36; ++i)
	{
		scene.sky->vertices[i] *= 2;
	}
	

	for (int i = 0; i < 12; ++i)
	{
		scene.sky->indices[i].u = i * 3 + 0;
		scene.sky->indices[i].v = i * 3 + 1;
		scene.sky->indices[i].w = i * 3 + 2;
	}
	for (int i = 0; i < 12; ++i)
	{
		Vector3f p1 = scene.sky->vertices[i * 3 + 0];
		Vector3f p2 = scene.sky->vertices[i * 3 + 1];
		Vector3f p3 = scene.sky->vertices[i * 3 + 2];

		Vector3f v1{ p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
		Vector3f v2{ p3.x - p2.x, p3.y - p2.y, p3.z - p2.z };
		Vector3f normal = v1.cross(v2);
		normal.normalize();
		normal *= -1;
		scene.sky->vertexNormals[i * 3 + 0] = normal;
		scene.sky->vertexNormals[i * 3 + 1] = normal;
		scene.sky->vertexNormals[i * 3 + 2] = normal;
	}

	Mesh mesh;
	mesh.reflect = true;
	mesh.indices = scene.sky->indices;
	mesh.vertices.resize(scene.sky->vertices.size());
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		mesh.vertices[i].pos = scene.sky->vertices[i];
		mesh.vertices[i].normal = scene.sky->vertexNormals[i];
	}
	scene.reflectMeshes.push_back(mesh);

	Mesh dynamicMesh;
	dynamicMesh.reflect = false;
	dynamicMesh.indices = scene.sky->indices;
	dynamicMesh.vertices.resize(scene.sky->vertices.size());
	for (int i = 0; i < mesh.vertices.size(); ++i)
	{
		dynamicMesh.vertices[i].pos = scene.sky->vertices[i] * 0.2f;
		dynamicMesh.vertices[i].normal = scene.sky->vertexNormals[i];
	}
	scene.meshes.push_back(dynamicMesh);

	scene.envCubeMap = std::make_unique<DynamicCubeMap>();

	scene.nearPlane = 1.f;
	scene.farPlane = 1000.f;
	scene.sky->texture = std::make_unique<CubeMap>(files);
	scene.camera.radius = 10.f;
	scene.camera.target = { 0.f, 0.f, 0.f };
	scene.camera.yaw = 90.f;
	scene.camera.pitch = 0.f;
	scene.camera.up = { 0.f, 1.f, 0.f };
	scene.camera.pos = { 0.f, 0.f, -10.f };
	scene.camera.useSphereMode = true;
	w.setScene(&scene);
	w.m_count = 0;
	//w.drawTriangle();
	QTimer timer;
	timer.setInterval(30);
	QObject::connect(&timer, &QTimer::timeout, [&w]()
		{
			w.m_count += 2;
			if (w.m_count > 360)
			{
				w.m_count = 0;
			}
			w.render();
			QApplication::processEvents();
		});
	timer.start();
	//w.render();
    return a.exec();

}
