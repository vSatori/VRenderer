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
	SOLID,WIREFRAME,SOLIDWITHWIREFRAME,NONE
};

enum class DepthMode
{
	NEVER,LESS,EQUAL,LESSEQUAL,ALWAYS
};

enum class BlendMode
{

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
	static bool alphaBlending;
	static bool drawColor;
	static bool clockwise;
	static Vector3f* pixels;
	static unsigned int* renderTarget;
	static float* zbuffer;
	static bool* pixelMask;
	static const int width = 800;
	static const int height = 600;
	static Vector3f eyePos;
	static float nearPlane;
	static float farPlane;
	static VertexShader* vs;
	static PixelShader* ps;
	static const float clipW;
	static unsigned int currentPixelIndex;
public:
	static void init();
	static void finalize();
	static void clear();
	static bool Cull(const Vector3f& vertexPos, const Vector3f& faceNormal);
	static bool depthTest(float z, int index);
	static void draw(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static bool checkClipping(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	static void drawFragment(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static void resolve();
	static void drawLine(int x1, int y1, int x2, int y2);
	
	static std::vector<Fragment> polygonClipping(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static Fragment clipPlane(const Fragment& fm1, const Fragment& fm2, Side side);
	static Fragment clipWPlane(const Fragment& fm1, const Fragment& fm2);
	static bool inside(const Vector4f& pos, Side side);
	/*
	static void drawFragmentByScanLine(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	static void drawTopTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	static void drawBottomTriangle(VertexOut& vo1, VertexOut& vo2, VertexOut& vo3);
	*/
};



