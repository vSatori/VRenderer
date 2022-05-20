#pragma once
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <algorithm>

static unsigned int whiteValue = (255 << 16) + (255 << 8) + 255;
enum class CullMode
{
	CULLFRONTFACE,CULLBACKFACE,CULLNONE
};

enum class FillMode
{
	SOLID,WIREFRAME,SOLIDWITHWIREFRAME,NONE
};

enum class DepthMode
{

};

enum class BlendMode
{

};


typedef void(*FragmentLerpFunction)(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3, float a, float b, float c);

void genericLerpFunction(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3, float a, float b, float c);

void CubeMapLerpFunction(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3, float a, float b, float c);

void ModelLerpFunction(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3, float a, float b, float c);

void ModelLerpFunction(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, float t, float hz1, float hz2);

void ModelLerpFunction(VertexOut& vout, const VertexOut& vo1, const VertexOut& vo2, float t);

class RenderContext
{
public:
	static CullMode cullMode;
	static FillMode fillMode;
	//static DepthMode depthMode;
	static bool alphaBlending;
	static bool drawColor;
	static bool posArea;
	static unsigned int* renderTarget;
	static float* zbuffer;
	static const int width = 1280;
	static const int height = 720;
	static Vector3f eyePos;
	static float nearPlane;
	static float farPlane;
	static VertexShader* vs;
	static PixelShader* ps;
	static FragmentLerpFunction  lerpFunc;
	static const float clipW;
	static unsigned int currentPixelIndex;
public:
	static void init()
	{
		cullMode = CullMode::CULLBACKFACE;
		fillMode = FillMode::SOLID;
		renderTarget = new unsigned int[width * height];
		zbuffer = new float[width * height];
		vs = nullptr;
		ps = nullptr;
	}
	static void finalize()
	{
		if (renderTarget)
		{
			delete renderTarget;
		}
		if (zbuffer)
		{
			delete zbuffer;
		}

	}


	static void clear()
	{
		//memset(renderTarget, 0, width * height * 4);
		
		//memset(zbuffer, 1, width * height * 4);
		
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				renderTarget[i * width + j] = whiteValue;
			}
		}
		
		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				zbuffer[i * width + j] = 1.001f;
			}
		}
		
	}

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




	static void drawFragment(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3);

	static void drawFragmentByScanLine(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	

	static std::vector<VertexOut> polygonClipping(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3)
	{
		std::vector<VertexOut> output;
		std::vector<VertexOut> inputW{ vo1, vo2, vo3 };
		for (int i = 0; i < inputW.size(); ++i)
		{
			auto& current = inputW[i];
			auto& last = inputW[(i + inputW.size() - 1) % inputW.size()];

			if (inside(current.posH, 6))
			{
				if (!inside(last.posH, 6))
				{
					output.push_back(clipWPlane(current, last));

				}
				output.push_back(current);
			}
			else if (inside(last.posH, 6))
			{
				output.push_back(clipWPlane(current, last));
			}
		}
		for (int i = 0; i < 6; ++i)
		{
			std::vector<VertexOut> input(output);
			output.clear();
			int size = input.size();
			for (int j = 0; j < size; ++j)
			{
				auto& current = input[j];
				auto& last = input[(j + size - 1) % size];

				
				if (inside(current.posH, i))
				{
					if (!inside(last.posH, i))
					{
						output.push_back(clipPlane(last, current, i));
					}
					output.push_back(current);
				}
				else if (inside(last.posH, i))
				{
					output.push_back(clipPlane(last, current, i));
				}
			}
		}
		return output;
	}

	static VertexOut clipPlane(const VertexOut& vo1, const VertexOut& vo2, int side)
	{
		float k1, k2;
		switch (side)
		{
		case 0:
		case 1:
		{
			k1 = vo1.posH.x;
			k2 = vo2.posH.x;
			break;
		}
		case 2:
		case 3:
		{
			k1 = vo1.posH.y;
			k2 = vo2.posH.y;
			break;
		}
		case 4:
		case 5:
		{
			k1 = vo1.posH.z;
			k2 = vo2.posH.z;
			break;
		}
		case 6:
		{
			k1 = vo1.posH.w;
			k2 = vo2.posH.w;
			break;
		}
		}
		if (side == 0 || side == 3 || side == 4)
		{
			k1 *= -1;
			k2 *= -1;
		}
		float t = (vo1.posH.w - k1) / ((vo1.posH.w - k1) - (vo2.posH.w - k2));
		/*
		float c1 = v1.dot(side);
		float c2 = v2.dot(side);
		float weight = c2 / (c2 - c1);
		*/
		return vo1 + (vo2 - vo1) * t;
	}

	static VertexOut clipWPlane(const VertexOut& vo1, const VertexOut& vo2)
	{
		float t = (vo1.posH.w - clipW) / (vo1.posH.w - vo2.posH.w);
		return vo1 + (vo2 - vo1) * t;
	}

	static bool inside(const Vector4f& pos, int side)
	{
		bool in = true;
		switch (side)
		{
		case 0:
		{
			in = pos.x >= -pos.w;
			break;
		}
		case 1:
		{
			in = pos.x <= pos.w;
			break;
		}
		case 2:
		{
			in = pos.y <= pos.w;
			break;
		}
		case 3:
		{
			in = pos.y >= -pos.w;

			break;
		}
		case 4:
		{
			in = pos.z >= -pos.w;
			break;
		}
		case 5:
		{
			in = pos.z <= pos.w;
			break;
		}
		case 6:
		{
			in = pos.w >= clipW;

			break;
		}
		}
		return in;
	}

	static void drawTopTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	static void drawBottomTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
};



