#include "RenderContext.h"
#include "omp.h"
/*
#ifdef _OPENMP
omp_lock_t lock;
omp_init_lock(&lock);
#endif // _omp
*/

static Vector2f g_samples0X[]{ {0.f, 0.f} };
static Vector2f g_samples2X[]{ {-0.25f, -0.25f},{0.25f, 0.25f} };
static Vector2f g_samples4X[]{ {-0.125f, -0.375f}, {0.375f, -0.125f}, {-0.375f, 0.125f}, {0.125f, 0.375f} };
static Vector2f g_samples8X[]{ {-0.375f, -0.375f}, {0.125f, 0.375f}, {-0.125f, 0.125f}, {0.375f, 0.125f},
							   {-0.375f, -0.125f}, {0.125f, -0.125f},{0.125f, -0.375f}, {0.375f, -0.375f} };
static Vector2f* g_currentSamples = g_samples4X;

static unsigned int g_whiteValue = (255 << 16) + (255 << 8) + 255;
static unsigned int g_redValue = (255 << 16);

static float g_sampleDepths[8];
static char g_sampleMasks[8];

const float RenderContext::cxt_clipW = 0.00001f;
CullMode RenderContext::cxt_cullMode = CullMode::CULLBACKFACE;
FillMode RenderContext::cxt_fillMode = FillMode::SOLID;
DepthMode RenderContext::cxt_depthMode = DepthMode::LESS;
AlphaMode RenderContext::cxt_alphaMode = AlphaMode::ALPHADISABLE;
float RenderContext::cxt_transparency = 1.f;
bool RenderContext::cxt_discardFragment = true;
bool RenderContext::cxt_clockwiseOrder = true;
unsigned int* RenderContext::cxt_renderTarget = nullptr;
bool* RenderContext::cxt_pixelMask = nullptr;
float* RenderContext::cxt_zbuffers = nullptr;
Vector3f* RenderContext::cxt_pixelColors = nullptr;

Vector3f RenderContext::cxt_eyePos = { 0.f, 0.f, 0.f };
float RenderContext::cxt_nearPlane = 0.f;
float RenderContext::cxt_farPlane = 100.f;
VertexShader* RenderContext::cxt_VS = nullptr;
PixelShader* RenderContext::cxt_PS = nullptr;
unsigned int RenderContext::cxt_currentPixelIndex = 0;
unsigned int RenderContext::cxt_currentSampleIndex = 0;
int RenderContext::cxt_sampleCount = 4;



void RenderContext::init()
{
	switch (RenderContext::cxt_msaaLevel)
	{
	case MSAALevel::MSAA0X:
	{
		RenderContext::cxt_sampleCount = 1;
		g_currentSamples = g_samples0X;
		break;
	}
	case MSAALevel::MSAA2X:
	{
		RenderContext::cxt_sampleCount = 2;
		g_currentSamples = g_samples2X;
		break;
	}
	case MSAALevel::MSAA4X:
	{
		RenderContext::cxt_sampleCount = 4;
		g_currentSamples = g_samples4X;
		break;
	}
	case MSAALevel::MSAA8X:
	{
		RenderContext::cxt_sampleCount = 8;
		g_currentSamples = g_samples8X;
		break;
	}
	default:
		break;
	}

	cxt_renderTarget = new unsigned int[cxt_frameWidth * cxt_frameHeight];
	cxt_zbuffers = new float[cxt_frameWidth * cxt_frameHeight * cxt_sampleCount];
	cxt_pixelColors = new Vector3f[cxt_frameWidth * cxt_frameHeight * cxt_sampleCount];
	cxt_pixelMask = new bool[cxt_frameWidth * cxt_frameHeight];
}

void RenderContext::finalize()
{
	if (cxt_renderTarget)
	{
		delete[] cxt_renderTarget;
	}
	if (cxt_pixelColors)
	{
		delete[] cxt_pixelColors;
	}
	if (cxt_zbuffers)
	{
		delete[] cxt_zbuffers;
	}
	if (cxt_pixelMask)
	{
		delete[] cxt_pixelMask;
	}
}

void RenderContext::clear()
{
	int count = cxt_frameWidth * cxt_frameHeight;
	
	for (int i = 0; i < count; ++i)
	{
		cxt_renderTarget[i] = g_whiteValue;
	}
	
	memset(cxt_pixelMask, 0, sizeof(bool) * count);
	count *= cxt_sampleCount;
	for (int i = 0; i < count; ++i)
	{
		cxt_zbuffers[i] = 1.f;
	}
	
	int floatSize = count * 3;
	float* p = (float*)&cxt_pixelColors[0];
	for (int i = 0; i < floatSize; ++i)
	{
		p[i] = 1.f;
	}
	

}

bool RenderContext::Cull(const Vector3f& vertexPos, const Vector3f& faceNormal)
{
	switch (cxt_cullMode)
	{
		case CullMode::CULLNONE:
		{
			return true;
		}
		case CullMode::CULLFRONTFACE:
		{
			bool b = (faceNormal.dot(cxt_eyePos - vertexPos) >= 0);
			if (cxt_clockwiseOrder)
			{
				return !b;
			}
			return b;
			//return ~((faceNormal.dot(eyePos - vertexPos) <= 0) ^ clockwise);
		}
		case CullMode::CULLBACKFACE:
		{
			bool b = (faceNormal.dot(cxt_eyePos - vertexPos) >= 0);
			if (cxt_clockwiseOrder)
			{
				return b;
			}
			return !b;
			//return ~((faceNormal.dot(eyePos - vertexPos) >= 0) ^ clockwise));
		}
		default:
		{
			return false;
		}
	}
}

bool RenderContext::depthTest(float z, int index)
{
	switch (cxt_depthMode)
	{
		case DepthMode::NEVER:
		{
			return false;
		}
		case DepthMode::LESS:
		{
			return z < cxt_zbuffers[index];
		}
		case DepthMode::EQUAL:
		{
			return z == cxt_zbuffers[index];
		}
		case DepthMode::LESSEQUAL:
		{
			return z <= cxt_zbuffers[index];
		}
		case DepthMode::ALWAYS:
		{
			return true;
		}
		default:
			return false;
	}
	return false;
}

void RenderContext::draw(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3)
{
	switch (cxt_fillMode)
	{
		case FillMode::SOLID:
		{
			drawFragment(fm1, fm2, fm3);
			break;
		}
		case FillMode::WIREFRAME:
		{
			Vector2f p1{ ((fm1.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm1.posH.y) * 0.5f * cxt_frameHeight) };
			Vector2f p2{ ((fm2.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm2.posH.y) * 0.5f * cxt_frameHeight) };
			Vector2f p3{ ((fm3.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm3.posH.y) * 0.5f * cxt_frameHeight) };

			int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
			int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
			int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

			int h = cxt_frameHeight - 1;
			int w = cxt_frameWidth  - 1;
			x1 = clamp(0, w, x1);
			x2 = clamp(0, w, x2);
			x3 = clamp(0, w, x3);
			y1 = clamp(0, h, y1);
			y2 = clamp(0, h, y2);
			y3 = clamp(0, h, y3);
			drawLine(x1, y1, x2, y2);
			drawLine(x2, y2, x3, y3);
			drawLine(x3, y3, x1, y1);
			break;
		}
		default:
			break;
	}
}

bool RenderContext::clippingTest(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3)
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
	
	float w = cxt_clipW > cxt_nearPlane ? cxt_clipW : cxt_nearPlane;
	if (v1.w < w || v2.w < w || v3.w < w)
	{
		return true;
	}
	
	return false;
}
#include <iostream>
void RenderContext::drawFragment(const Fragment & fm1, const Fragment & fm2, const Fragment & fm3)
{
	Vector2f p1{ ((fm1.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm1.posH.y) * 0.5f * cxt_frameHeight) };
	Vector2f p2{ ((fm2.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm2.posH.y) * 0.5f * cxt_frameHeight) };
	Vector2f p3{ ((fm3.posH.x + 1.f) * 0.5f * cxt_frameWidth),  ((1.f - fm3.posH.y) * 0.5f * cxt_frameHeight) };

	int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
	int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
	int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), cxt_frameWidth  - 1);
	int maxy = std::min(findMax(y1, y2, y3), cxt_frameHeight - 1);

	Fragment frag;
	
	float* pfm = (float*)&frag;
	const float* pfm1 = (const float*)&fm1;
	const float* pfm2 = (const float*)&fm2;
	const float* pfm3 = (const float*)&fm3;

	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			bool coveraged = false;
			Vector3f sampleColor;
			int baseIndex = (cxt_frameWidth * y + x) * cxt_sampleCount;
			memset(g_sampleMasks, 0, cxt_sampleCount);
			for (int index = 0; index < cxt_sampleCount; ++index)
			{
				Vector2f p{ static_cast<float>(x), static_cast<float>(y) };
				p += g_currentSamples[index];

				float areaA = ((p - p2).cross(p3 - p2));
				float areaB = ((p - p3).cross(p1 - p3));
				float areaC = ((p - p1).cross(p2 - p1));

				if (cxt_clockwiseOrder)
				{
					bool okA = areaA < 0.000001f;
					bool okB = areaB < 0.000001f;
					bool okC = areaC < 0.000001f;

					if (!(okA && okB && okC))
					{
						continue;
					}
				}
				else
				{
					bool okD = areaA > -0.000001f;
					bool okE = areaB > -0.000001f;
					bool okF = areaC > -0.000001f;

					if (!(okD && okE && okF))
					{
						continue;
					}
				}
				float area = areaA + areaB + areaC;
				float a = areaA / area;
				float b = areaB / area;
				float c = areaC / area;

				float z = (fm1.posH.z * a + fm2.posH.z * b + fm3.posH.z * c);
				g_sampleDepths[index] = z;
				cxt_currentPixelIndex = (cxt_frameWidth * y + x) * cxt_sampleCount + index;
				if (!depthTest(z, cxt_currentPixelIndex))
				{
					continue;
				}
				if (!cxt_discardFragment)
				{
					continue;
				}
				
				if (!coveraged)
				{
					cxt_currentSampleIndex = index;
					a /= fm1.posH.w;
					b /= fm2.posH.w;
					c /= fm3.posH.w;
					float rw = 1.f / (a + b + c);
					
					for (int i = 0; i < Fragment::floatSize; ++i)
					{
						pfm[i] = (pfm1[i] * a + pfm2[i] * b + pfm3[i] * c) * rw;
					}
					sampleColor = cxt_PS->execute(frag);
					
					float* pc = (float*)&sampleColor;
					for (int i = 0; i < 3; ++i)
					{
						if (pc[i] > 1.f)
						{
							pc[i] = 1.f;
						}
					}
					coveraged = true;
				}
				g_sampleMasks[index] = 1;
				cxt_pixelMask[cxt_currentPixelIndex / cxt_sampleCount] = true;
			}
			if (cxt_alphaMode == AlphaMode::ALPHATOCOVERAGE && cxt_msaaLevel >= MSAALevel::MSAA4X)
			{
				int num = cxt_sampleCount - cxt_sampleCount * cxt_transparency;
				for (int i = 0; i < num; ++i)
				{
					g_sampleMasks[i] = 0;
				}
			}		
			if (coveraged)
			{
				for (int i = 0; i < cxt_sampleCount; ++i)
				{
					int index = baseIndex + i;
					if (g_sampleMasks[i])
					{
						cxt_zbuffers[index] = g_sampleDepths[i];
						cxt_pixelColors[index] = sampleColor;
					}
				}
			}
		}
	}
}
void RenderContext::resolve()
{
	int num = cxt_frameWidth * cxt_frameHeight * cxt_sampleCount;
	for (int i = 0; i < num; i += cxt_sampleCount)
	{
		if (!cxt_pixelMask[i / cxt_sampleCount])
		{
			continue;
		}
		Vector3f color;
		for (int j = 0; j < cxt_sampleCount; ++j)
		{
			color += cxt_pixelColors[i + j];
		}
		cxt_renderTarget[i / cxt_sampleCount] = colorValue(color / cxt_sampleCount);
	}
}
void RenderContext::drawLine(int x1, int y1, int x2, int y2)
{
	bool steep = false;
	if (std::abs(x1 - x2) < std::abs(y1 - y2))
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
		steep = true;
	}

	if (x1 > x2) 
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	int dx = x2 - x1;
	int dy = y2 - y1;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y1;
	int index = 0;
	for (int x = x1; x <= x2; x++)
	{
		if (steep)
		{
			cxt_renderTarget[y + cxt_frameWidth * x] = g_redValue;
		}
		else
		{
			cxt_renderTarget[x + cxt_frameWidth * y] = g_redValue;
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y2 > y1 ? 1 : -1);
			error2 -= dx * 2;
		}
		++index;
	}
}



std::vector<Fragment> RenderContext::sutherlandHodgemanClipping(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3)
{
	std::vector<Fragment> output;
	std::vector<Fragment> inputW{ fm1, fm2, fm3 };
	int wsize = inputW.size();
	for (int i = 0; i < wsize; ++i)
	{
		auto& current = inputW[i];
		auto& last = inputW[(i + wsize - 1) % wsize];

		if (inside(current.posH, Side::W))
		{
			if (!inside(last.posH, Side::W))
			{
				output.push_back(clipWPlane(current, last));

			}
			output.push_back(current);
		}
		else if (inside(last.posH, Side::W))
		{
			output.push_back(clipWPlane(current, last));
		}
	}
	for (int i = 0; i < 6; ++i)
	{
		std::vector<Fragment> input(output);
		output.clear();
		int size = input.size();
		for (int j = 0; j < size; ++j)
		{
			auto& current = input[j];
			auto& last = input[(j + size - 1) % size];
			Side side = static_cast<Side>(i);
			if (inside(current.posH, side))
			{
				if (!inside(last.posH, side))
				{
					output.push_back(clipPlane(last, current, side));
				}
				output.push_back(current);
			}
			else if (inside(last.posH, side))
			{
				output.push_back(clipPlane(last, current, side));
			}
		}
	}
	return output;
}

Fragment RenderContext::clipPlane(const Fragment& fm1, const Fragment& fm2, Side side)
{
	float k1, k2;
	switch (side)
	{
	case Side::POSX:
	case Side::NEGX:
	{
		k1 = fm1.posH.x;
		k2 = fm2.posH.x;
		break;
	}
	case Side::POSY:
	case Side::NEGY:
	{
		k1 = fm1.posH.y;
		k2 = fm2.posH.y;
		break;
	}
	case Side::POSZ:
	case Side::NEGZ:
	{
		k1 = fm1.posH.z;
		k2 = fm2.posH.z;
		break;
	}
	case Side::W:
	{
		k1 = fm1.posH.w;
		k2 = fm2.posH.w;
		break;
	}
	}
	if (static_cast<int>(side) % 2)
	{
		k1 *= -1;
		k2 *= -1;
	}
	float t = (fm1.posH.w - k1) / ((fm1.posH.w - k1) - (fm2.posH.w - k2));
	/*
	float c1 = v1.dot(side);
	float c2 = v2.dot(side);
	float weight = c2 / (c2 - c1);
	*/
	return Fragment::lerp(fm1, fm2, t);
}

Fragment RenderContext::clipWPlane(const Fragment& fm1, const Fragment& fm2)
{
	float w = cxt_clipW > cxt_nearPlane ? cxt_clipW : cxt_nearPlane;
	float t = (fm1.posH.w - w) / (fm1.posH.w - fm2.posH.w);
	return Fragment::lerp(fm1, fm2, t);
}

bool RenderContext::inside(const Vector4f& pos, Side side)
{
	bool in = true;
	switch (side)
	{
	case Side::POSX:
	{
		in = pos.x <= pos.w;
		break;
	}
	case Side::NEGX:
	{
		in = pos.x >= -pos.w;
		break;
	}
	case Side::POSY:
	{
		in = pos.y <= pos.w;
		break;
	}
	case Side::NEGY:
	{
		in = pos.y >= -pos.w;
		break;
	}
	case Side::POSZ:
	{
		in = pos.z <= pos.w;
		break;
	}
	case Side::NEGZ:
	{
		in = pos.z >= -pos.w;
		break;
	}
	case Side::W:
	{
		float w = cxt_clipW > cxt_nearPlane ? cxt_clipW : cxt_nearPlane;
		in = pos.w >= w;
		break;
	}
	}
	return in;
}