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
	static const float clipW;
	static unsigned int currentPixelIndex;
	static int count;
	static int allCount;
public:
	static void init();
	static void finalize();
	static void clear();
	static bool Cull(const Vector3f& vertexPos, const Vector3f& faceNormal);
	static bool checkClipping(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	static void drawFragment(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3);
	
	static std::vector<VertexOut> polygonClipping(const VertexOut& vo1, const VertexOut& vo2, const VertexOut& vo3);
	static VertexOut clipPlane(const VertexOut& vo1, const VertexOut& vo2, int side);
	static VertexOut clipWPlane(const VertexOut& vo1, const VertexOut& vo2);
	static bool inside(const Vector4f& pos, int side);
	/*
	static void drawFragmentByScanLine(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	static void drawTopTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	static void drawBottomTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	*/
};



