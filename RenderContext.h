#pragma once
#include "MathUtility.h"
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <algorithm>

enum class CullMode
{
	CULLFRONTFACE,CULLBACKFACE,CULLNONE
};

enum class FillMode
{
	SOLID,WIREFRAME,SOLIDWITHWIREFRAME
};

enum class DepthMode
{

};

class RenderContext
{
public:
	static CullMode cullMode;
	static FillMode fillMode;
	static DepthMode depthMode;
	static Vector3f eyePos;
	static unsigned int* renderTarget;
	static float* zbuffer;
	static int width;
	static int height;
	static float near;
	static float far;
	static VertexShader vs;
	static PixelShader ps;
	static const float clipW;
public:
	static bool Cull(const Vector3f& vertexPos, const Vector3f& faceNormal)
	{
		switch (cullMode)
		{
			case CullMode::CULLNONE:
			{
				return true;
			}
			case CullMode::CULLFRONTFACE:
			{
				return faceNormal.dot(eyePos - vertexPos) <= 0;
			}
			case CullMode::CULLBACKFACE:
			{
				return faceNormal.dot(eyePos - vertexPos) >= 0;
			}
			default:
			{
				return false;
			}
		}
	}

	static bool checkClipping(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
	{
		if (v1.w < near || v2.w < near || v3.w < near)
		{
			return true;
		}
		if (v1.x + v1.w < 0 || v2.x + v2.w < 0 || v3.x + v3.w < 0)
		{
			return true;
		}
		if (v1.x - v1.w > 0 || v2.x - v2.w > 0 || v3.x - v3.w > 0)
		{
			return true;
		}
		if (v1.y + v1.w < 0 || v2.y + v2.w < 0 || v3.y + v3.w < 0)
		{
			return true;
		}
		if (v1.y - v1.w > 0 || v2.y - v2.w > 0 || v3.y - v3.w > 0)
		{
			return true;
		}
		if (v1.z + v1.w < 0 || v2.z + v2.w < 0 || v3.y + v3.w < 0)
		{
			return true;
		}
		if (v1.z - v1.w > 0 || v2.z - v2.w > 0 || v3.y - v3.w > 0)
		{
			return true;
		}
		if (v1.w < clipW || v2.w < clipW || v3.w < clipW)
		{
			return true;
		}
		return false;
	}

	static void drawFragment(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3)
	{
		Vector2f p1{ (vo1.posH.x + 1.f) * 0.5f * width,  (1.f - vo1.posH.y) * 0.5f * height };
		Vector2f p2{ (vo2.posH.x + 1.f) * 0.5f * width,  (1.f - vo2.posH.y) * 0.5f * height };
		Vector2f p3{ (vo3.posH.x + 1.f) * 0.5f * width,  (1.f - vo3.posH.y) * 0.5f * height };

		int x1 = p1.x; int y1 = p1.y;
		int x2 = p2.x; int y2 = p2.y;
		int x3 = p3.x; int y3 = p3.y;

		float z1 = 1.f / vo1.posH.w;
		float z2 = 1.f / vo2.posH.w;
		float z3 = 1.f / vo3.posH.w;

		int minx = std::max(findMin(x1, x2, x3), 1);
		int miny = std::max(findMin(y1, y2, y3), 1);
		int maxx = std::min(findMax(x1, x2, x3), width - 1);
		int maxy = std::min(findMax(y1, y2, y3), height - 1);

		for (int y = miny; y <= maxy; ++y)
		{
			for (int x = minx; x <= maxx; ++x)
			{
				int dx1 = x1 - x;
				int dy1 = y1 - y;
				int dx2 = x2 - x;
				int dy2 = y2 - y;
				int dx3 = x3 - x;
				int dy3 = y3 - y;


				long long v1 = dx1 * dy2 - dx2 * dy1;
				long long v2 = dx2 * dy3 - dx3 * dy2;
				long long v3 = dx3 * dy1 - dx1 * dy3;

				if (v1 * v2 >= 0 && v1 * v3 >= 0 && v2 * v3 >= 0)
				{
					Vector2f p{ x, y };
					float areaA = (p - p2).corss(p3 - p2);
					float areaB = (p - p3).corss(p1 - p3);
					float areaC = (p - p1).corss(p2 - p1);
					float area = areaA + areaB + areaC;
					float a = areaA / area;
					float b = areaB / area;
					float c = areaC / area;


					float z = vo1.posH.w * a + vo2.posH.w * b + vo3.posH.w * c;
					int index = width * y + x;
					if (zbuffer[index] > z)
					{
						continue;
					}
					float hz = 1.f / ((a * z1) + (b * z2) + (c * z3));

				    VertexOut vout = (vo1 * (a * z1) + vo2 * (b * z2) + vo3 * (c * z3)) * (1.f / hz);
					Vector3f color = ps(vout);
					
					float* pc = (float*)&color;
					for (int i = 0; i < 3; ++i)
					{
						if (*(pc + i) > 1.f)
						{
							*(pc + i) = 1.f;
						}
					}
					int red = color.x * 255;
					int green = color.y * 255;
					int blue = color.z * 255;
					unsigned int colorValue = (red << 16) + (green << 8) + blue;
					zbuffer[index] = z ;
					renderTarget[index] = colorValue;
				}


			}
		}
	}
};

const float RenderContext::clipW = 0.00001f;

