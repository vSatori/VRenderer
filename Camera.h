#pragma once
#include "MathUtil.h"
struct Camera
{
public:
	inline void update()
	{
		if (useSphereMode)
		{
			float rp = radian(pitch);
			float ry = radian(yaw);
			pos.x = radius * cosf(rp) * cosf(ry) + target.x;
			pos.y = radius * sinf(rp) + target.y;
			pos.z = radius * cosf(rp) * sinf(ry) + target.z;
		}
		Vector3f up{0.f, 1.f, 0.f};
		matrix = Matrix4::lookAt(pos, target, up);
	}
public:
	Vector3f pos;
	Vector3f target;
	Vector3f up;
	float radius;
	Matrix4 matrix;
	float yaw;
	float pitch;
	bool useSphereMode;

};

