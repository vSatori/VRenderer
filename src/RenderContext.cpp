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

const float RenderContext::clipW = 0.00001f;
CullMode RenderContext::cullMode = CullMode::CULLBACKFACE;
FillMode RenderContext::fillMode = FillMode::SOLID;
DepthMode RenderContext::depthMode = DepthMode::LESS;
AlphaMode RenderContext::alphaMode = AlphaMode::ALPHADISABLE;
float RenderContext::transparency = 1.f;
bool RenderContext::alphaBlending = false;
bool RenderContext::drawColor = true;
bool RenderContext::clockwise = true;
unsigned int* RenderContext::renderTarget = nullptr;
bool* RenderContext::pixelMask = nullptr;
char* RenderContext::sampleMasks = nullptr;
float* RenderContext::zbuffers = nullptr;
Vector3f* RenderContext::pixelColors = nullptr;

Vector3f RenderContext::eyePos = { 0.f, 0.f, 0.f };
float RenderContext::nearPlane = 0.f;
float RenderContext::farPlane = 100.f;
VertexShader* RenderContext::vs = nullptr;
PixelShader* RenderContext::ps = nullptr;
unsigned int RenderContext::currentPixelIndex = 0;
unsigned int RenderContext::currentSampleIndex = 0;
int RenderContext::sampleCount = 4;



void RenderContext::init()
{
	switch (RenderContext::msaaLevel)
	{
	case MSAALevel::MSAA0X:
	{
		RenderContext::sampleCount = 1;
		g_currentSamples = g_samples0X;
		break;
	}
	case MSAALevel::MSAA2X:
	{
		RenderContext::sampleCount = 2;
		g_currentSamples = g_samples2X;
		break;
	}
	case MSAALevel::MSAA4X:
	{
		RenderContext::sampleCount = 4;
		g_currentSamples = g_samples4X;
		break;
	}
	case MSAALevel::MSAA8X:
	{
		RenderContext::sampleCount = 8;
		g_currentSamples = g_samples8X;
		break;
	}
	default:
		break;
	}

	renderTarget = new unsigned int[width * height];
	zbuffers = new float[width * height * sampleCount];
	pixelColors = new Vector3f[width * height * sampleCount];
	pixelMask = new bool[width * height];
	sampleMasks = new char[width * height * sampleCount];
}

void RenderContext::finalize()
{
	if (renderTarget)
	{
		delete[] renderTarget;
	}
	if (pixelColors)
	{
		delete[] pixelColors;
	}
	if (zbuffers)
	{
		delete[] zbuffers;
	}
	if (pixelMask)
	{
		delete[] pixelMask;
	}
	if (sampleMasks)
	{
		delete[] sampleMasks;
	}
}

void RenderContext::clear()
{
	int count = width * height;
	
	for (int i = 0; i < count; ++i)
	{
		renderTarget[i] = g_whiteValue;
	}
	
	memset(pixelMask, 0, sizeof(bool) * count);
	count *= sampleCount;
	for (int i = 0; i < count; ++i)
	{
		zbuffers[i] = 1.f;

	}
	memset(sampleMasks, 0, count);
	int floatSize = count * 3;
	float* p = (float*)&pixelColors[0];
	for (int i = 0; i < floatSize; ++i)
	{
		p[i] = 1.f;
	}

}

bool RenderContext::Cull(const Vector3f& vertexPos, const Vector3f& faceNormal)
{
	switch (cullMode)
	{
		case CullMode::CULLNONE:
		{
			return true;
		}
		case CullMode::CULLFRONTFACE:
		{
			bool b = (faceNormal.dot(eyePos - vertexPos) >= 0);
			if (clockwise)
			{
				return !b;
			}
			return b;
			//return ~((faceNormal.dot(eyePos - vertexPos) <= 0) ^ clockwise);
		}
		case CullMode::CULLBACKFACE:
		{
			bool b = (faceNormal.dot(eyePos - vertexPos) >= 0);
			if (clockwise)
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
	switch (depthMode)
	{
		case DepthMode::NEVER:
		{
			return false;
		}
		case DepthMode::LESS:
		{
			return z < zbuffers[index];
		}
		case DepthMode::EQUAL:
		{
			return z == zbuffers[index];
		}
		case DepthMode::LESSEQUAL:
		{
			return z <= zbuffers[index];
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
	switch (fillMode)
	{
		case FillMode::SOLID:
		{
			drawFragment(fm1, fm2, fm3);
			break;
		}
		case FillMode::WIREFRAME:
		{
			Vector2f p1{ ((fm1.posH.x + 1.f) * 0.5f * width),  ((1.f - fm1.posH.y) * 0.5f * height) };
			Vector2f p2{ ((fm2.posH.x + 1.f) * 0.5f * width),  ((1.f - fm2.posH.y) * 0.5f * height) };
			Vector2f p3{ ((fm3.posH.x + 1.f) * 0.5f * width),  ((1.f - fm3.posH.y) * 0.5f * height) };

			int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
			int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
			int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

			int h = height - 1;
			int w = width  - 1;
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
	
	float w = clipW > nearPlane ? clipW : nearPlane;
	if (v1.w < w || v2.w < w || v3.w < w)
	{
		return true;
	}
	
	return false;
}
#include <iostream>
void RenderContext::drawFragment(const Fragment & fm1, const Fragment & fm2, const Fragment & fm3)
{
	Vector2f p1{ ((fm1.posH.x + 1.f) * 0.5f * width),  ((1.f - fm1.posH.y) * 0.5f * height) };
	Vector2f p2{ ((fm2.posH.x + 1.f) * 0.5f * width),  ((1.f - fm2.posH.y) * 0.5f * height) };
	Vector2f p3{ ((fm3.posH.x + 1.f) * 0.5f * width),  ((1.f - fm3.posH.y) * 0.5f * height) };

	int x1 = static_cast<int>(p1.x + 0.5f); int y1 = static_cast<int>(p1.y + 0.5f);
	int x2 = static_cast<int>(p2.x + 0.5f); int y2 = static_cast<int>(p2.y + 0.5f);
	int x3 = static_cast<int>(p3.x + 0.5f); int y3 = static_cast<int>(p3.y + 0.5f);

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), width  - 1);
	int maxy = std::min(findMax(y1, y2, y3), height - 1);

	Fragment frag;
	
	float* pfm = (float*)&frag;
	const float* pfm1 = (const float*)&fm1;
	const float* pfm2 = (const float*)&fm2;
	const float* pfm3 = (const float*)&fm3;

	float* depths = new float[sampleCount];
	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			bool isDone = false;
			Vector3f sampleColor;
			
			for (int index = 0; index < sampleCount; ++index)
			{
				Vector2f p{ static_cast<float>(x), static_cast<float>(y) };
				p += g_currentSamples[index];

				float areaA = ((p - p2).cross(p3 - p2));
				float areaB = ((p - p3).cross(p1 - p3));
				float areaC = ((p - p1).cross(p2 - p1));

				if (clockwise)
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
				currentPixelIndex = (width * y + x) * sampleCount + index;
				if (!depthTest(z, currentPixelIndex) && sampleMasks[currentPixelIndex] == 1)
				{
					continue;
				}
				//zbuffers[currentPixelIndex] = z;
				depths[index] = z;
				if (!drawColor)
				{
					continue;
				}
				if (!isDone)
				{
					currentSampleIndex = index;
					a /= fm1.posH.w;
					b /= fm2.posH.w;
					c /= fm3.posH.w;
					float rw = 1.f / (a + b + c);
					
					for (int i = 0; i < Fragment::floatSize; ++i)
					{
						pfm[i] = (pfm1[i] * a + pfm2[i] * b + pfm3[i] * c) * rw;
					}
					sampleColor = ps->execute(frag);
					
					float* pc = (float*)&sampleColor;
					for (int i = 0; i < 3; ++i)
					{
						if (pc[i] > 1.f)
						{
							pc[i] = 1.f;
						}
					}
					isDone = true;
				}
				sampleMasks[currentPixelIndex] = 1;
				pixelMask[currentPixelIndex / sampleCount] = true;
				//pixelColors[currentPixelIndex] = sampleColor;
			}
			if (alphaMode == AlphaMode::ALPHATOCOVERAGE && msaaLevel >= MSAALevel::MSAA4X)
			{
				int num = sampleCount - sampleCount * transparency;
				for (int i = 0; i < num; ++i)
				{
					sampleMasks[(width * y + x) * sampleCount + i] = 0;
				}
			}
			if (isDone)
			{
				for (int i = 0; i < sampleCount; ++i)
				{
					if (sampleMasks[(width * y + x) * sampleCount + i])
					{
						zbuffers[(width * y + x) * sampleCount + i] = depths[i];
						pixelColors[(width * y + x) * sampleCount + i] = sampleColor;
					}
				}
			}
		}
	}
	delete[] depths;
}
void RenderContext::resolve()
{
	//float weight = 1.f / sampleCount;
	int num = width * height * sampleCount;
	for (int i = 0; i < num; i += sampleCount)
	{
		if (!pixelMask[i / sampleCount])
		{
			continue;
		}
		Vector3f color;
		for (int j = 0; j < sampleCount; ++j)
		{
			color += pixelColors[i + j];
		}
		
		renderTarget[i / sampleCount] = colorValue(color / sampleCount);
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
			renderTarget[y + width * x] = g_redValue;
		}
		else
		{
			renderTarget[x + width * y] = g_redValue;
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



std::vector<Fragment> RenderContext::polygonClipping(const Fragment& fm1, const Fragment& fm2, const Fragment& fm3)
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
	float w = clipW > nearPlane ? clipW : nearPlane;
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
		float w = clipW > nearPlane ? clipW : nearPlane;
		in = pos.w >= w;
		break;
	}
	}
	return in;
}