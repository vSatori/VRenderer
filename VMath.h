#pragma once
#include <string.h>
#include <math.h>
template <typename T>
struct Vector2
{
	inline float corss(const Vector2<T>& vec)const
	{
		return x * vec.y - y * vec.x;
	}
	inline Vector2<T> operator+(const Vector2<T>& vec)const
	{
		Vector2f res;
		res.x = x + vec.x;
		res.y = y + vec.y;
		return res;
	}
	inline Vector2<T> operator-(const Vector2<T>& vec)const
	{
		Vector2f res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		return res;
	}
	inline Vector2<T> operator*(float t)const
	{
		Vector2f res;
		res.x = x * t;
		res.y = y * t;
		return res;
	}
	inline Vector2<T> operator+=(const Vector2<T>& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		return *this;
	}
	inline Vector2<T> operator-=(const Vector2<T>& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		return *this;
	}
	inline Vector2<T> operator*=(float t)
	{
		this->x *= t;
		this->y *= t;
		return *this;
	}
	T x;
	T y;
};

template <typename T>
struct Vector3
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
	inline Vector3<T> operator+(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = this->x + vec.x;
		res.y = this->y + vec.y;
		res.z = this->z + vec.z;
		return res;
	}
	inline Vector3<T> operator-(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = this->x - vec.x;
		res.y = this->y - vec.y;
		res.z = this->z - vec.z;
		return res;
	}

	inline Vector3<T> operator*(float w)const
	{
		Vector3<T> res;
		res.x = this->x * w;
		res.y = this->y * w;
		res.z = this->z * w;
		return res;
	}

	inline Vector3<T> operator*(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = this->x * vec.x;
		res.y = this->y * vec.y;
		res.z = this->z * vec.z;
		return res;
	}

	inline Vector3<T> operator/(float w)const
	{
		Vector3<T> res;
		res.x = this->x / w;
		res.y = this->y / w;
		res.z = this->z / w;
		return res;
	}
	inline Vector3<T> operator+=(const Vector3<T>& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		return *this;
	}
	inline Vector3<T> operator-=(const Vector3<T>& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		return *this;
	}
	inline Vector3<T> operator*=(const Vector3<T>& vec)
	{
		this->x *= vec.x;
		this->y *= vec.y;
		this->z *= vec.z;
		return *this;
	}
	inline Vector3<T> operator*=(float w)
	{
		this->x *= w;
		this->y *= w;
		this->z *= w;
		return *this;
	}
	float dot(const Vector3<T>& vec)const
	{
		return this->x * vec.x + this->y * vec.y + this->z * vec.z;
	}
	inline Vector3<T> cross(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = this->y * vec.z - this->z * vec.y;
		res.y = this->z * vec.x - this->x * vec.z;
		res.z = this->x * vec.y - this->y * vec.x;
		return res;
	}
	inline Vector3<T> reflect(const Vector3<T>& normal)const
	{
		Vector3<T> res;
		//R = I - 2.0 * dot(N, I) * N;
		float d = this->dot(normal);
		res.x = this->x - 2 * d * normal.x;
		res.y = this->y - 2 * d * normal.y;
		res.z = this->z - 2 * d * normal.z;
		return res;
	}
public:
	T x;
	T y;
	T z;
};

template <typename T>
struct Vector4
{
	inline Vector4<T> operator=(const Vector3<T>& vec3)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
		w = 1.f;
		return *this;
	}
	inline Vector4<T> operator+(const Vector4<T>& vec4)const
	{
		Vector4<T> res;
		res.x = x + vec4.x;
		res.y = y + vec4.y;
		res.z = z + vec4.z;
		res.w = w + vec4.w;

		return res;
	}

	inline Vector4<T> operator-(const Vector4<T>& vec4)const
	{
		Vector4<T> res;
		res.x = x - vec4.x;
		res.y = y - vec4.y;
		res.z = z - vec4.z;
		res.w = w - vec4.w;
			
		return res;
	}
	inline Vector4<T> operator+(float v)const
	{
		Vector4<T> res;
		res.x = x + v;
		res.y = y + v;
		res.z = z + v;
		res.w = w + v;

		return res;
	}
	inline Vector4<T> operator*(float v)const
	{
		Vector4<T> res;
		res.x = x * v;
		res.y = y * v;
		res.z = z * v;
		res.w = w * v;

		return res;
	}
	inline Vector4<T> operator/(float v)const
	{
		Vector4<T> res;
		res.x = x / v;
		res.y = y / v;
		res.z = z / v;
		res.w = w / v;

		return res;
	}
	inline Vector4<T> operator+=(const Vector4<T>& vec)
	{
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
		this->w += vec.w;
		return *this;
	}
	inline Vector4<T> operator-=(const Vector4<T>& vec)
	{
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
		this->w -= vec.w;
		return *this;
	}
	inline Vector4<T> operator*=(float t)
	{
		this->x *= t;
		this->y *= t;
		this->z *= t;
		this->w *= t;
		return *this;
	}

	inline Vector4<T> operator/=(float t)
	{
		x /= t;
		y /= t;
		z /= t;
		w /= t;
		return *this;
	}
	inline float dot(const Vector4<T>& vec4) const
	{
		return x * vec4.x + y * vec4.y + z * vec4.z + w * vec4.w;
	}
	T x;
	T y;
	T z;
	T w;
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;
using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;

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

template<typename T>
inline Vector4<T> vector3To4(const Vector3<T>& vec3)
{
	return Vector4<T>{vec3.x, vec3.y, vec3.z, 1.f};
}
template<typename T>
inline Vector3<T> Vector4To3(const Vector4<T>& vec4)
{
	return Vector3<T>{vec4.x, vec4.y, vec4, z};
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
