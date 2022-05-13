#pragma once
#include <string.h>
#include <math.h>

struct Vector2f
{
	float corss(const Vector2f& vec)const
	{
		return x * vec.y - y * vec.x;
	}
	Vector2f operator+(const Vector2f& vec)const
	{
		Vector2f res;
		res.x = x + vec.x;
		res.y = y + vec.y;
		return res;
	}
	Vector2f operator-(const Vector2f& vec)const
	{
		Vector2f res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		return res;
	}
	Vector2f operator*(float t)const
	{
		Vector2f res;
		res.x = x * t;
		res.y = y * t;
		return res;
	}
	Vector2f operator+=(const Vector2f& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		return *this;
	}
	Vector2f operator-=(const Vector2f& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		return *this;
	}
	Vector2f operator*=(float t)
	{
		this->x *= t;
		this->y *= t;
		return *this;
	}
	float x;
	float y;
};
struct Vector3f
{

	void init()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}
	float length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	void normalize()
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
	Vector3f operator+(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = this->x + vec.x;
		res.y = this->y + vec.y;
		res.z = this->z + vec.z;
		return res;
	}
	Vector3f operator-(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = this->x - vec.x;
		res.y = this->y - vec.y;
		res.z = this->z - vec.z;
		return res;
	}

	Vector3f operator*(float w)const
	{
		Vector3f res;
		res.x = this->x * w;
		res.y = this->y * w;
		res.z = this->z * w;
		return res;
	}

	Vector3f operator*(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = this->x * vec.x;
		res.y = this->y * vec.y;
		res.z = this->z * vec.z;
		return res;
	}

	Vector3f operator/(float w)const
	{
		Vector3f res;
		res.x = this->x / w;
		res.y = this->y / w;
		res.z = this->z / w;
		return res;
	}
	Vector3f operator+=(const Vector3f& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		return *this;
	}
	Vector3f operator-=(const Vector3f& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		return *this;
	}
	Vector3f operator*=(const Vector3f& vec)
	{
		this->x *= vec.x;
		this->y *= vec.y;
		this->z *= vec.z;
		return *this;
	}
	Vector3f operator*=(float w)
	{
		this->x *= w;
		this->y *= w;
		this->z *= w;
		return *this;
	}
	float dot(const Vector3f& vec)const
	{
		return this->x * vec.x + this->y * vec.y + this->z * vec.z;
	}
	Vector3f cross(const Vector3f& vec)const
	{
		Vector3f res;
		res.x = this->y * vec.z - this->z * vec.y;
		res.y = this->z * vec.x - this->x * vec.z;
		res.z = this->x * vec.y - this->y * vec.x;
		return res;
	}
	Vector3f reflect(const Vector3f& normal)const
	{
		Vector3f res;
		//R = I - 2.0 * dot(N, I) * N;
		float d = this->dot(normal);
		res.x = this->x - 2 * d * normal.x;
		res.y = this->y - 2 * d * normal.y;
		res.z = this->z - 2 * d * normal.z;
		return res;
	}
public:
	float x;
	float y;
	float z;
};


struct Vector4f
{
	Vector4f()
	{

	}
	Vector4f(float x, float y, float z, float w) : 
		x(x), y(y),z(z),w(w)
	{

	}
	Vector4f(const Vector3f& vec3) :
		x(vec3.x),
		y(vec3.y),
		z(vec3.z),
		w(1.f) {}
	Vector4f operator=(const Vector3f& vec3)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
		w = 1.f;
		return *this;
	}
	Vector4f operator+(const Vector4f& vec4)const
	{
		Vector4f res;
		res.x = x + vec4.x;
		res.y = y + vec4.y;
		res.z = z + vec4.z;
		res.w = w + vec4.w;

		return res;
	}

	Vector4f operator-(const Vector4f& vec4)const
	{
		Vector4f res;
		res.x = x - vec4.x;
		res.y = y - vec4.y;
		res.z = z - vec4.z;
		res.w = w - vec4.w;
			
		return res;
	}
	Vector4f operator+(float v)const
	{
		Vector4f res;
		res.x = x + v;
		res.y = y + v;
		res.z = z + v;
		res.w = w + v;

		return res;
	}
	Vector4f operator*(float v)const
	{
		Vector4f res;
		res.x = x * v;
		res.y = y * v;
		res.z = z * v;
		res.w = w * v;

		return res;
	}
	Vector4f operator/(float v)const
	{
		Vector4f res;
		res.x = x / v;
		res.y = y / v;
		res.z = z / v;
		res.w = w / v;

		return res;
	}
	Vector4f operator+=(const Vector4f& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		this->w += vec.w;
		return *this;
	}
	Vector4f operator-=(const Vector4f& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		this->w -= vec.w;
		return *this;
	}
	Vector4f operator*=(float t)
	{
		this->x *= t;
		this->y *= t;
		this->z *= t;
		this->w *= t;
		return *this;
	}

	Vector4f operator/=(float t)
	{
		x /= t;
		y /= t;
		z /= t;
		w /= t;
		return *this;
	}
	float dot(const Vector4f& vec4) const
	{
		return x * vec4.x + y * vec4.y + z * vec4.z + w * vec4.w;
	}
	float x;
	float y;
	float z;
	float w;
};

struct Vector3i
{
	int u;
	int v;
	int w;
};

struct Matrix4
{
public:
	Matrix4()
	{
		init();
	}
	void init()
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
	Matrix4 operator*(const Matrix4& mat)const
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

	Matrix4 operator=(const Matrix4& mat)
	{
		for (int i = 0; i < 4; ++i)
		{
			memcpy(m[i], mat.m[i], sizeof(float) * 4);
		}
		return *this;
	}
	
	Vector4f operator*(const Vector4f& vec)const
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
