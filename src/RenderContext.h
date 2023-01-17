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

#define FW RenderContext::cxt_frameWidth
#define FH RenderContext::cxt_frameHeight

class RenderContext
{
public:
	static CullMode cxt_cullMode;
	static FillMode cxt_fillMode;
	static DepthMode cxt_depthMode;
	static AlphaMode cxt_alphaMode;
	static float cxt_transparency;
	static bool cxt_discardFragment;
	static bool cxt_clockwiseOrder;
	static unsigned int* cxt_renderTarget;
	static bool* cxt_pixelMask;
	static float* cxt_zbuffers;
	static Vector3f* cxt_pixelColors;
	static Vector3f cxt_eyePos;
	static float cxt_nearPlane;
	static float cxt_farPlane;
	static VertexShader* cxt_VS;
	static PixelShader* cxt_PS;
	static unsigned int cxt_currentPixelIndex;
	static unsigned int cxt_currentSampleIndex;
	static int cxt_sampleCount;
	static const float cxt_clipW;
	static const int cxt_frameWidth = 800;
	static const int cxt_frameHeight = 600;
	static const MSAALevel cxt_msaaLevel = MSAALevel::MSAA4X;

	
	
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
	

	static std::vector<Fragment> sutherlandHodgemanClipping(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3);
	static Fragment clipPlane(const Fragment& fm1, const Fragment& fm2, Side side);
	static Fragment clipWPlane(const Fragment& fm1, const Fragment& fm2);
	static bool inside(const Vector4f& pos, Side side);
};



