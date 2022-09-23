#pragma once
#include <string>
#include <math.h>
#define PI 3.1415926535f

inline float radian(float angle)
{
	return PI / 180.f * angle;
}

template<typename T>
struct Vector2
{
public:
	Vector2(){}
	Vector2(T x, T y) :x(x), y(y){}
	inline T length() const
	{
		return sqrtf(x * x + y * y);
	}
	inline void normalize()
	{
		T len = length();
		if (len <= 0.f)
		{
			return;
		}
		x /= len;
		y /= len;
	}
	inline T dot(const Vector2<T>& vec)const
	{
		return x * vec.x + y * vec.y;
	}

	inline T cross(const Vector2<T>& vec)const
	{
		return x * vec.y - y * vec.x;
	}
	inline Vector2<T> operator+(const Vector2<T>& vec)const
	{
		Vector2<T> res;
		res.x = x + vec.x;
		res.y = y + vec.y;
		return res;
	}
	inline Vector2<T> operator-(const Vector2<T>& vec)const
	{
		Vector2<T> res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		return res;
	}
	inline Vector2<T> operator*(T t)const
	{
		Vector2<T> res;
		res.x = x * t;
		res.y = y * t;
		return res;
	}
	inline Vector2<T> operator+=(const Vector2<T>& vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}
	inline Vector2<T> operator-=(const Vector2<T>& vec)
	{
		x -= vec.x;
		y -= vec.y;
		return *this;
	}
	inline Vector2<T> operator*=(T t)
	{
		x *= t;
		y *= t;
		return *this;
	}
public:
	T x{ T() };
	T y{ T() };
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;


template<typename T>
struct Vector3
{
public:
	Vector3() {}
	Vector3(T* v) : x(v[0]), y(v[1]), z(v[2]) {}
	Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
public:
	inline T length()const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	inline void normalize()
	{
		T len = length();
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
		res.x = x + vec.x;
		res.y = y + vec.y;
		res.z = z + vec.z;
		return res;
	}
	inline Vector3<T> operator-(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = x - vec.x;
		res.y = y - vec.y;
		res.z = z - vec.z;
		return res;
	}

	inline Vector3<T> operator*(T w)const
	{
		Vector3<T> res;
		res.x = x * w;
		res.y = y * w;
		res.z = z * w;
		return res;
	}

	inline Vector3<T> operator*(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = x * vec.x;
		res.y = y * vec.y;
		res.z = z * vec.z;
		return res;
	}

	inline Vector3<T> operator/(T w)const
	{
		Vector3<T> res;
		res.x = x / w;
		res.y = y / w;
		res.z = z / w;
		return res;
	}
	inline Vector3<T> operator+=(const Vector3<T>& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	inline Vector3<T> operator-=(const Vector3<T>& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	inline Vector3<T> operator*=(const Vector3<T>& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}
	inline Vector3<T> operator*=(T w)
	{
		x *= w;
		y *= w;
		z *= w;
		return *this;
	}
	inline Vector3<T> operator/=(T w)
	{
		x /= w;
		y /= w;
		z /= w;
		return *this;
	}
	T dot(const Vector3<T>& vec)const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}
	inline Vector3<T> cross(const Vector3<T>& vec)const
	{
		Vector3<T> res;
		res.x = y * vec.z - z * vec.y;
		res.y = z * vec.x - x * vec.z;
		res.z = x * vec.y - y * vec.x;
		return res;
	}
	inline Vector3<T> reflect(const Vector3<T>& normal)const
	{
		Vector3<T> res;
		//R = I - 2.0 * dot(N, I) * N;
		T d = dot(normal);
		res.x = x - 2 * d * normal.x;
		res.y = y - 2 * d * normal.y;
		res.z = z - 2 * d * normal.z;
		return res;
	}
public:
	T x{ T() };
	T y{ T() };
	T z{ T() };
};

using Vector3i = Vector3<int>;
using Vector3f = Vector3<float>;

template<typename T>
struct Vector4
{
public:
	Vector4() {}
	Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
public:
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
	inline Vector4<T> operator+(T v)const
	{
		Vector4<T> res;
		res.x = x + v;
		res.y = y + v;
		res.z = z + v;
		res.w = w + v;

		return res;
	}
	inline Vector4<T> operator*(T v)const
	{
		Vector4<T> res;
		res.x = x * v;
		res.y = y * v;
		res.z = z * v;
		res.w = w * v;

		return res;
	}
	inline Vector4<T> operator/(T v)const
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
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}
	inline Vector4<T> operator-=(const Vector4<T>& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}
	inline Vector4<T> operator*=(T t)
	{
		x *= t;
		y *= t;
		z *= t;
		w *= t;
		return *this;
	}

	inline Vector4<T> operator/=(T t)
	{
		x /= t;
		y /= t;
		z /= t;
		w /= t;
		return *this;
	}
	inline T dot(const Vector4<T>& vec4) const
	{
		return x * vec4.x + y * vec4.y + z * vec4.z + w * vec4.w;
	}
public:
	T x{ T() };
	T y{ T() };
	T z{ T() };
	T w{ T() };
};

using Vector4i = Vector4<int>;
using Vector4f = Vector4<float>;

struct Matrix3
{
public:
	Matrix3()
	{
		identity();
	}

	inline void zero()
	{
		for (int i = 0; i < 3; ++i)
		{
			memset(m[i], 0, sizeof(float) * 3);
		}
	}

	inline void identity()
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
		identity();
	}

	inline void zero()
	{
		for (int i = 0; i < 4; ++i)
		{
			memset(m[i], 0, sizeof(float) * 4);
		}
	}
	inline void identity()
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

	static inline Matrix4 lookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
	{
		Matrix4 matrix;
		Vector3f dir = target - position;
		dir.normalize();

		Vector3f t = up;
		Vector3f post = up.cross(dir);
		post.normalize();
		t = dir.cross(post);

		matrix.m[0][0] = post.x;
		matrix.m[0][1] = post.y;
		matrix.m[0][2] = post.z;
		matrix.m[0][3] = -post.dot(position);

		matrix.m[1][0] = t.x;
		matrix.m[1][1] = t.y;
		matrix.m[1][2] = t.z;
		matrix.m[1][3] = -t.dot(position);

		matrix.m[2][0] = dir.x;
		matrix.m[2][1] = dir.y;
		matrix.m[2][2] = dir.z;
		matrix.m[2][3] = -dir.dot(position);

		return matrix;
	}

	static inline Matrix4 perspectiveProjection(float w, float h, float fov, float n, float f)
	{
		Matrix4 mat;
		float tanVar = tanf(radian(fov / 2));
		float ratio = w / h;
		mat.m[0][0] = 1.f / (ratio * tanVar);
		mat.m[1][1] = 1.f / tanVar;
		mat.m[2][2] = f / (f - n);
		mat.m[2][3] = f * n / (n - f);
		mat.m[3][2] = 1.f;
		mat.m[3][3] = 0.f;
		return mat;
	}

	static inline Matrix4 orthogonalProjection(float w, float h, float n, float f)
	{
		Matrix4 mat;
		mat.m[0][0] = 2.f / w;
		mat.m[1][1] = 2.f / h;
		mat.m[2][2] = 1.f / (f - n);
		mat.m[2][3] = n / (n - f);
		return mat;
	}

public:
	float m[4][4];
};



template <typename T>
inline T clamp(T min, T max, T value)
{
	if (value < min)
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}
	return value;
}


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
template<typename T>
inline T findMax(T v1, T v2, T v3)
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

template<typename T>
inline T findMin(T v1, T v2, T v3)
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

inline Matrix3 makeTBN(const Vector3f& t, const Vector3f& b, const Vector3f& n)
{
	Matrix3 mat;
	int size = sizeof(t);
	memcpy(mat.m[0], &t, size);
	memcpy(mat.m[1], &b, size);
	memcpy(mat.m[2], &n, size);
	return mat;
}

inline Vector3f bgr2Vector(unsigned int value)
{
	Vector3f color;
	color.x = ((float)((value << 8)  >> 24)) / 255.f;
	color.y = ((float)((value << 16) >> 24)) / 255.f;
	color.z = ((float)((value << 24) >> 24)) / 255.f;
	return color;
}

inline Vector3f rgb2Vector(unsigned int value)
{
	Vector3f color;
	color.x = ((float)((value << 24) >> 24)) / 255.f;
	color.y = ((float)((value << 16) >> 24)) / 255.f;
	color.z = ((float)((value << 8)  >> 24)) / 255.f;
	return color;
}

inline unsigned int colorValue(const Vector3f& color)
{
	int red   = static_cast<int>(color.x * 255.f);
	int green = static_cast<int>(color.y * 255.f);
	int blue  = static_cast<int>(color.z * 255.f);
	return (red << 16) + (green << 8) + blue;

}



