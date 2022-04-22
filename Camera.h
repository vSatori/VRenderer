#pragma once
#include "MathUtility.h"
struct Camera
{
public:
	inline void update()
	{
		if (useSphereMode)
		{
			pos.x = radius * cosf(pitch) * cosf(yaw) + target.x;
			pos.y = radius * sinf(pitch) + target.y;
			pos.z = radius * cosf(pitch) * sinf(yaw) + target.z;
		}

		Vector3f dir = pos - target;
		dir.normalize();
		Vector3f up{ 0.f, 1.f, 0.f};
		Vector3f post = up.cross(dir);
		post.normalize();
		up = dir.cross(post);

		viewMat.m[0][0] = post.x;
		viewMat.m[0][1] = post.y;
		viewMat.m[0][2] = post.z;
		viewMat.m[0][3] = -post.dot(pos);

		viewMat.m[1][0] = up.x;
		viewMat.m[1][1] = up.y;
		viewMat.m[1][2] = up.z;
		viewMat.m[1][3] = -up.dot(pos);

		viewMat.m[2][0] = dir.x;
		viewMat.m[2][1] = dir.y;
		viewMat.m[2][2] = dir.z;
		viewMat.m[2][3] = -dir.dot(pos);

		viewMat.m[3][0] = 0;
		viewMat.m[3][1] = 0;
		viewMat.m[3][2] = 0;
		viewMat.m[3][3] = 1;
		

	}
public:
	Vector3f pos;
	Vector3f target;
	float radius;
	Matrix4 viewMat;
	float yaw;
	float pitch;
	bool useSphereMode;

};

