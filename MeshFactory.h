#pragma once
#include "Mesh.h"
class MeshFactory
{
public:
	static void createCube(Mesh& mesh, float length = 1.f, float width = 1.f, float height = 1.f)
	{
		
		std::vector<Vector3i> indices;
		std::vector<Vector3f> coordinates;
		std::vector<Vector3f> normals;
		normals.resize(36);
		coordinates.resize(36);
		indices.resize(12);
		coordinates[0] = { -0.5, 0.5, -0.5 };
		coordinates[2] = { -0.5, -0.5, -0.5 };
		coordinates[1] = { 0.5, -0.5, -0.5 };
		coordinates[3] = { -0.5, 0.5, -0.5 };
		coordinates[5] = { 0.5, -0.5, -0.5 };
		coordinates[4] = { 0.5, 0.5, -0.5 };

		coordinates[6] = { -0.5, 0.5, 0.5 };
		coordinates[8] = { -0.5, -0.5, 0.5 };
		coordinates[7] = { -0.5, -0.5, -0.5 };
		coordinates[9] = { -0.5, 0.5, 0.5 };
		coordinates[11] = { -0.5, -0.5, -0.5 };
		coordinates[10] = { -0.5, 0.5, -0.5 };


		coordinates[12] = { 0.5, 0.5, 0.5 };
		coordinates[14] = { 0.5, -0.5, 0.5 };
		coordinates[13] = { -0.5, -0.5, 0.5 };
		coordinates[15] = { 0.5, 0.5, 0.5 };
		coordinates[17] = { -0.5, -0.5, 0.5 };
		coordinates[16] = { -0.5, 0.5, 0.5 };


		coordinates[18] = { 0.5, 0.5, -0.5 };
		coordinates[20] = { 0.5, -0.5, -0.5 };
		coordinates[19] = { 0.5, -0.5, 0.5 };
		coordinates[21] = { 0.5, 0.5, -0.5 };
		coordinates[23] = { 0.5, -0.5, 0.5 };
		coordinates[22] = { 0.5, 0.5, 0.5 };


		coordinates[24] = { -0.5, 0.5, 0.5 };
		coordinates[26] = { -0.5, 0.5, -0.5 };
		coordinates[25] = { 0.5, 0.5, -0.5 };
		coordinates[27] = { -0.5, 0.5, 0.5 };
		coordinates[29] = { 0.5, 0.5, -0.5 };
		coordinates[28] = { 0.5, 0.5, 0.5 };

		coordinates[30] = { 0.5, -0.5, 0.5 };
		coordinates[32] = { 0.5, -0.5, -0.5 };
		coordinates[31] = { -0.5, -0.5, -0.5 };
		coordinates[33] = { 0.5, -0.5, 0.5 };
		coordinates[35] = { -0.5, -0.5, -0.5 };
		coordinates[34] = { -0.5, -0.5, 0.5 };

		for (int i = 0; i < 36; ++i)
		{
			coordinates[i].x *= length;
			coordinates[i].y *= width;
			coordinates[i].z *= height;
		}

		for (int i = 0; i < 12; ++i)
		{
			indices[i].x = i * 3 + 0;
			indices[i].y = i * 3 + 1;
			indices[i].z = i * 3 + 2;
		}
		for (int i = 0; i < 12; ++i)
		{
			Vector3f p1 = coordinates[i * 3 + 0];
			Vector3f p2 = coordinates[i * 3 + 1];
			Vector3f p3 = coordinates[i * 3 + 2];

			Vector3f v1{ p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
			Vector3f v2{ p3.x - p2.x, p3.y - p2.y, p3.z - p2.z };
			Vector3f normal = v1.cross(v2);
			normal.normalize();
			normals[i * 3 + 0] = normal;
			normals[i * 3 + 1] = normal;
			normals[i * 3 + 2] = normal;
		}
		for (int i = 0; i < 36; ++i)
		{
			Vertex v;
			v.pos = coordinates[i];
			v.normal = normals[i];
			mesh.vertices.push_back(v);
		}
		mesh.indices = indices;

	}

	static void createSphere(Mesh& mesh, float radius, int level, int slice)
	{
		int vertexCount = 2 + (level - 1) * (slice + 1);
		int indexCount = 6 * (level - 1) * slice;

		const float pi = 3.141592654f;

		float phi = 0.f, theta = 0.f;
		float perPhi = pi / level;
		float perTheta = pi * 2.f / slice;
		Vertex firstVertex{ {0.f, radius, 0.f},{0.f, 1.f, 0.f}, {0.f, 0.f} };
		mesh.vertices.push_back(firstVertex);
		for (int i = 1; i < level; ++i)
		{
			phi = perPhi * i;
			for (int j = 0; j <= slice; ++j)
			{
				Vector3f vec;
				theta = perTheta * j;
				vec.x = radius * sinf(phi) * cosf(theta);
				vec.y = radius * cosf(phi);
				vec.z = radius * sinf(phi) * sinf(theta);
				Vector3f normal = vec;
				normal.normalize();
				mesh.vertices.push_back({ vec, normal, {theta / (pi * 2.f), phi / pi} });
			}
		}
		Vertex lastVertex = { {0.f, -radius, 0.f},{0.f, -1.f, 0.f}, {0.f, 1.f} };
		mesh.vertices.push_back(lastVertex);
		if (level > 1)
		{
			for (int j = 1; j <= slice; ++j)
			{
				mesh.indices.push_back({ 0, j % (slice + 1) + 1, j });
			}
		}
		for (int i = 1; i < level - 1; ++i)
		{
			for (int j = 1; j <= slice; ++j)
			{
				Vector3i index1;
				Vector3i index2;
				index1.x = (i - 1) * (slice + 1) + j;
				index1.y = (i - 1) * (slice + 1) + j % (slice + 1) + 1;
				index1.z = i * (slice + 1) + j % (slice + 1) + 1;

				index2.x = i * (slice + 1) + j % (slice + 1) + 1;
				index2.y = i * (slice + 1) + j;
				index2.z = (i - 1) * (slice + 1) + j;
				
				mesh.indices.push_back(index1);
				mesh.indices.push_back(index2);
			}
		}
		if (level > 1)
		{
			for (int j = 1; j <= slice; ++j)
			{
				Vector3i index;
				index.x = (level - 2) * (slice + 1) + j;
				index.y = (level - 2) * (slice + 1) + j % (slice + 1) + 1;
				index.z = (level - 1) * (slice + 1) + 1;
				mesh.indices.push_back(index);
			}
		}
	}
};

