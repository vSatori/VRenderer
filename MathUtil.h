#pragma once
#include <string.h>
#include <math.h>
#define PI 3.1415926535f
struct Vector2f
{
public:
	Vector2f(){}
	Vector2f(float x, float y) :x(x), y(y)
	{

	}
	inline float length() const
	{
		return sqrt(x * x + y * y);
	}
	inline void normalize()
	{
		float len = sqrt(x * x + y * y);
		x /= len;
		y /= len;
	}
	inline float dot(const Vector2f& vec)const
	{
		return x * vec.x + y * vec.y;
	}

	inline float corss(const Vector2f& vec)const
	{
		return x * vec.y - y * vec.x;
	}
	inline Vector2f operator+(const Vector2f& vec)const
	{
		Vector2f res;
		res.x = x + vec.x;
		res.y = y + vec.y;
		return res;
	}
	inline Vector2f operator-(const Vector2f& vec)const
	{
		Vector2f res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		return res;
	}
	inline Vector2f operator*(float t)const
	{
		Vector2f res;
		res.x = x * t;
		res.y = y * t;
		return res;
	}
	inline Vector2f operator+=(const Vector2f& vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}
	inline Vector2f operator-=(const Vector2f& vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}
	inline Vector2f operator*=(float t)
	{
		x *= t;
		y *= t;
		return *this;
	}
	float x = 0.f;
	float y = 0.f;
};

struct Vector3i
{
	int x;
	int y;
	int z;
};


struct Vector3f
{
	inline float length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	inline void normalize()
	{
		float len = length();
		if (len <= 0.f)
		{
			return;
		}
		x /= len;
		y /= len;
		z /= len;
	}
	inline Vector3f operator+(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = x + vec.x;
		res.y = y + vec.y;
		res.z = z + vec.z;
		return res;
	}
	inline Vector3f operator-(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		res.z = z - vec.z;
		return res;
	}

	inline Vector3f operator*(float w)const
	{
		Vector3f res;
		res.x = x * w;
		res.y = y * w;
		res.z = z * w;
		return res;
	}

	inline Vector3f operator*(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = x * vec.x;
		res.y = y * vec.y;
		res.z = z * vec.z;
		return res;
	}

	inline Vector3f operator/(float w)const
	{
		Vector3f res;
		res.x = x / w;
		res.y = y / w;
		res.z = z / w;
		return res;
	}
	inline Vector3f operator+=(const Vector3f& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	inline Vector3f operator-=(const Vector3f& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	inline Vector3f operator*=(const Vector3f& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}
	inline Vector3f operator*=(float w)
	{
		x *= w;
		y *= w;
		z *= w;
		return *this;
	}
	float dot(const Vector3f& vec)const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}
	inline Vector3f cross(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = y * vec.z - z * vec.y;
		res.y = z * vec.x - x * vec.z;
		res.z = x * vec.y - y * vec.x;
		return res;
	}
	inline Vector3f reflect(const Vector3f& normal)const
	{
		Vector3f res;
		//R = I - 2.0 * dot(N, I) * N;
		float d = dot(normal);
		res.x = x - 2 * d * normal.x;
		res.y = y - 2 * d * normal.y;
		res.z = z - 2 * d * normal.z;
		return res;
	}
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};


struct Vector4f
{
	inline Vector4f operator=(const Vector3f& vec3)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
		w = 1.f;
		return *this;
	}
	inline Vector4f operator+(const Vector4f& vec4)const
	{
		Vector4f res;
		res.x = x + vec4.x;
		res.y = y + vec4.y;
		res.z = z + vec4.z;
		res.w = w + vec4.w;

		return res;
	}

	inline Vector4f operator-(const Vector4f& vec4)const
	{
		Vector4f res;
		res.x = x - vec4.x;
		res.y = y - vec4.y;
		res.z = z - vec4.z;
		res.w = w - vec4.w;

		return res;
	}
	inline Vector4f operator+(float v)const
	{
		Vector4f res;
		res.x = x + v;
		res.y = y + v;
		res.z = z + v;
		res.w = w + v;

		return res;
	}
	inline Vector4f operator*(float v)const
	{
		Vector4f res;
		res.x = x * v;
		res.y = y * v;
		res.z = z * v;
		res.w = w * v;

		return res;
	}
	inline Vector4f operator/(float v)const
	{
		Vector4f res;
		res.x = x / v;
		res.y = y / v;
		res.z = z / v;
		res.w = w / v;

		return res;
	}
	inline Vector4f operator+=(const Vector4f& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}
	inline Vector4f operator-=(const Vector4f& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}
	inline Vector4f operator*=(float t)
	{
		x *= t;
		y *= t;
		z *= t;
		w *= t;
		return *this;
	}

	inline Vector4f operator/=(float t)
	{
		x /= t;
		y /= t;
		z /= t;
		w /= t;
		return *this;
	}
	inline float dot(const Vector4f& vec4) const
	{
		return x * vec4.x + y * vec4.y + z * vec4.z + w * vec4.w;
	}
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;
};


struct Matrix3
{
public:
	Matrix3()
	{
		init();
	}
	inline void init()
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (i == j)
				{
					m[i][j] = 1.f;
					continue;
				}
				m[i][j] = 0.f;
			}
		}
	}
	inline Matrix3 operator*(const Matrix3& mat)const
	{
		Matrix3 res;
		for (int row = 0; row < 3; ++row)
		{
			for (int col = 0; col < 3; ++col)

			{
				res.m[row][col] = m[row][0] * mat.m[0][col]
					+ m[row][1] * mat.m[1][col]
					+ m[row][2] * mat.m[2][col];
			}
		}
		return res;
	}

	inline Matrix3 operator=(const Matrix3& mat)
	{
		for (int i = 0; i < 3; ++i)
		{
			memcpy(m[i], mat.m[i], sizeof(float) * 3);
		}
		return *this;
	}

	inline Vector3f operator*(const Vector3f& vec)const
	{
		Vector3f res;
		float temp[3];
		for (int i = 0; i < 3; ++i)
		{
			temp[i] = m[i][0] * vec.x
				+ m[i][1] * vec.y
				+ m[i][2] * vec.z;
		}
		memcpy(&res, temp, sizeof(float) * 3);
		return res;
	}

public:
	float m[3][3];
};



struct Matrix4
{
public:
	Matrix4()
	{
		init();
	}
	inline void init()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (i == j)
				{
					m[i][j] = 1.f;
					continue;
				}
				m[i][j] = 0.f;
			}
		}
	}
	inline Matrix4 operator*(const Matrix4& mat)const
	{
		Matrix4 res;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)

			{
				res.m[row][col] = m[row][0] * mat.m[0][col]
					+ m[row][1] * mat.m[1][col]
					+ m[row][2] * mat.m[2][col]
					+ m[row][3] * mat.m[3][col];
			}
		}
		return res;
	}

	inline Matrix4 operator=(const Matrix4& mat)
	{
		for (int i = 0; i < 4; ++i)
		{
			memcpy(m[i], mat.m[i], sizeof(float) * 4);
		}
		return *this;
	}

	inline Vector4f operator*(const Vector4f& vec)const
	{
		Vector4f res;
		float temp[4];


		for (int i = 0; i < 4; ++i)
		{
			temp[i] = m[i][0] * vec.x
				+ m[i][1] * vec.y
				+ m[i][2] * vec.z
				+ m[i][3] * vec.w;
		}

		memcpy(&res, temp, sizeof(float) * 4);
		return res;
	}

public:
	float m[4][4];
};


inline Vector4f vector3To4(const Vector3f& vec3)
{
	return Vector4f{vec3.x, vec3.y, vec3.z, 1.f};
}

inline Vector3f Vector4To3(const Vector4f& vec4)
{
	return Vector3f{vec4.x, vec4.y, vec4.z};
}

inline Matrix3 Matrix4To3(const Matrix4& mat4)
{
	Matrix3 mat3;
	for (int i = 0; i < 3; ++i)
	{
		memcpy(mat3.m[i], mat4.m[i], sizeof(float) * 3);
	}
	return mat3;
}

inline int findMax(int v1, int v2, int v3)
{
	if (v1 >= v2)
	{
		if (v1 >= v3)
		{
			return v1;
		}
		return v3;
	}
	if (v2 >= v3)
	{
		return v2;
	}
	return v3;
}

inline int findMin(int v1, int v2, int v3)
{
	if (v1 <= v2)
	{
		if (v1 <= v3)
		{
			return v1;
		}
		return v3;
	}
	if (v2 <= v3)
	{
		return v2;
	}
	return v3;
}

inline float radian(float angle)
{
	return 3.1415926f / 180.f * angle;
}

inline Vector3f toVector(unsigned int value)
{
	Vector3f color;
	color.x = ((float)((value << 8) >> 24)) / 255.f;
	color.y = ((float)((value << 16) >> 24)) / 255.f;
	color.z = ((float)((value << 24) >> 24)) / 255.f;
	return color;
}

