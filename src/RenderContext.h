#pragma once
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
	SOLID,WIREFRAME,NONE
};

enum class DepthMode
{
	NEVER,LESS,EQUAL,LESSEQUAL,ALWAYS
};

enum class AlphaMode
{
	ALPHADISABLE,ALPHABLENDING,ALPHATOCOVERAGE
};

enum class MSAALevel
{
	MSAA0X,MSAA2X,MSAA4X,MSAA8X
};

enum class Side
{
	POSX,NEGX,POSY,NEGY,POSZ,NEGZ,W
};


class RenderContext
{
public:
	static CullMode cullMode;
	static FillMode fillMode;
	static DepthMode depthMode;
	static AlphaMode alphaMode;
	static float transparency;
	static bool alphaBlending;
	static bool drawColor;
	static bool clockwise;
	static unsigned int* renderTarget;
	static bool* pixelMask;
	static char* sampleMasks;
	static float* zbuffers;
	static Vector3f* pixelColors;
	static Vector3f eyePos;
	static float nearPlane;
	static float farPlane;
	static VertexShader* vs;
	static PixelShader* ps;
	static unsigned int currentPixelIndex;
	static unsigned int currentSampleIndex;
	static int sampleCount;
	static const float clipW;
	static const int width = 800;
	static const int height = 600;
	static const MSAALevel msaaLevel = MSAALevel::MSAA4X;
	
public:
	static void init();
	static void finalize();
	static void clear();
	static bool Cull(const Vector3f& vertexPos, const Vector3f& faceNormal);
	static bool depthTest(float z, int index);
	static void draw(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static bool clippingTest(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	static void drawFragment(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static void drawLine(int x1, int y1, int x2, int y2);
	static void resolve();
	
	
	static std::vector<Fragment> polygonClipping(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static Fragment clipPlane(const Fragment& fm1, const Fragment& fm2, Side side);
	static Fragment clipWPlane(const Fragment& fm1, const Fragment& fm2);
	static bool inside(const Vector4f& pos, Side side);
};



