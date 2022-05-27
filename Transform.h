#pragma once
#include "MathUtil.h"
class Transform
{
public:
	static Matrix4 scale(float x, float y, float z)
	{
		Matrix4 mat;
		mat.m[0][0] = x;
		mat.m[1][1] = y;
		mat.m[2][2] = z;
		return mat;
	}

	static Matrix4 translate(float x, float y, float z)
	{
		Matrix4 mat;
		mat.m[0][3] = x;
		mat.m[1][3] = y;
		mat.m[2][3] = z;
		return mat;
	}

	static Matrix4 rotate(Vector3f axis, float angle)
	{
		Matrix4 mat;
		float x = axis.x;
		float y = axis.y;
		float z = axis.z;
		float r = radian(angle);
		float cosVar = cosf(r);
		float sinVar = sinf(r);

		mat.m[0][0] = (y * y + z * z) * cosVar + x * x;
		mat.m[0][1] = x * y * (1 - cosVar) - z * sinVar;
		mat.m[0][2] = x * z * (1 - cosVar) + y * sinVar;

		mat.m[1][0] = x * y * (1 - cosVar) + z * sinVar;
		mat.m[1][1] = y * y + (1 - y * y) * cosVar;
		mat.m[1][2] = y * z * (1 - cosVar) - x * sinVar;

		mat.m[2][0] = x * z * (1 - cosVar) - y * sinVar;
		mat.m[2][1] = y * z * (1 - cosVar) + x * sinVar;
		mat.m[2][2] = z * z + (1 - z * z) * cosVar;

		return mat;

	}
};