#include "Renderer.h"
#include <qpainter.h>
#include "Texture.h"
#include "Scene.h"
#include "Light.h"
#include <qevent.h>
#include "SkyBox.h"

static const int clientWidth = 640;
static const int clientHeight = 480;

Renderer::Renderer(QWidget *parent)
    : QWidget(parent), m_lastX(0), m_lastY(0), m_fov(90.f), m_interact(false)
{
  
	setFixedWidth(clientWidth);
	setFixedHeight(clientHeight);
	m_frameData = new unsigned int[clientHeight * clientWidth];
	m_zbuffer = new float[clientHeight * clientWidth];
}

Renderer::~Renderer()
{
	delete[] m_frameData;
	delete[] m_zbuffer;
}
#include <qdebug.h>
void Renderer::render()
{
	if (!m_scene)
	{
		return;
	}
	clear();
	m_scene->camera.update();
	Matrix4 matView = m_scene->camera.viewMat;
	Matrix4 matpers = perspectiveMat(m_scene->nearPlane, m_scene->farPlane);
	Matrix4 mat = matView * matpers;
	for (int index = 0; index < m_scene->meshes.size(); ++index)
	{
		Mesh& mesh = m_scene->meshes[index];
		int faceSize = mesh.indices.size();
		for (int i = 0; i < faceSize; ++i)
		{
			Vertex& v1 = mesh.vertices[mesh.indices[i].u];
			Vector3f dir = m_scene->camera.pos - v1.pos;
			if (dir.dot(v1.normal) <= 0)
			{
				continue;
			}
			Vertex& v2 = mesh.vertices[mesh.indices[i].v];
			Vertex& v3 = mesh.vertices[mesh.indices[i].w];
			Vector4f t1(v1.pos);
			Vector4f t2(v2.pos);
			Vector4f t3(v3.pos);
			t1 = mat * t1;
		    t2 = mat * t2;
		    t3 = mat * t3;
			t1 /= t1.w;
			t2 /= t2.w;
			t3 /= t3.w;
			
			Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight};
			Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
			Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
			VertexOut vo1{ v1, p1, 1.f / t1.w };
			VertexOut vo2{ v2, p2, 1.f / t2.w };
			VertexOut vo3{ v3, p3, 1.f / t3.w };
			drawFragment(vo1, vo2, vo3);
		}
	}
	if (m_scene->sky)
	{
		SkyBox * sky = m_scene->sky.get();
		matView.m[0][3] = 0.f;
		matView.m[1][3] = 0.f;
		matView.m[2][3] = 0.f;

		mat = matView * matpers;


		int faceSize = sky->indices.size();

		for (int i = 0; i < faceSize; ++i)
		{
			int u = sky->indices[i].u;
			int v = sky->indices[i].v;
			int w = sky->indices[i].w;
			Vector3f& v1 = m_scene->sky->vertices[u];
			Vector3f dir = m_scene->camera.pos - v1;
			/*
			if (dir.dot(sky->vertexNormals[u]) <= 0)
			{
				continue;
			}
			*/
			Vector3f& v2 = m_scene->sky->vertices[v];
			Vector3f& v3 = m_scene->sky->vertices[w];
			Vector4f t1(v1);
			Vector4f t2(v2);
			Vector4f t3(v3);
			t1 = mat * t1;
			t2 = mat * t2;
			t3 = mat * t3;
			t1 /= t1.w;
			t2 /= t2.w;
			t3 /= t3.w;
			Vector2f p1{ (t1.x + 1.f) * 0.5f * clientWidth,  (1.f - t1.y) * 0.5f * clientHeight };
			Vector2f p2{ (t2.x + 1.f) * 0.5f * clientWidth,  (1.f - t2.y) * 0.5f * clientHeight };
			Vector2f p3{ (t3.x + 1.f) * 0.5f * clientWidth,  (1.f - t3.y) * 0.5f * clientHeight };
			Vector3f vv1{ t1.x, t1.y, t1.z };
			Vector3f vv2{ t2.x, t2.y, t2.z };
			Vector3f vv3{ t3.x, t3.y, t3.z };
			
			qDebug() << u << ":" << vv1.x << vv1.y << vv1.z;
			qDebug() << v << ":" << vv2.x << vv2.y << vv2.z;
			qDebug() << w << ":" << vv3.x << vv3.y << vv3.z;
			
			
			drawSkyFragment(vv1, p1, vv2, p2, vv3, p3, sky->texture.get());
		}


	}
	update();

}

void Renderer::drawTexture2D(const QString & file)
{
	/*
	Texture tex(file.toStdString().c_str());
	for (int col = 0; col < clientHeight; ++col)
	{
		for (int row = 0; row < clientWidth; ++row)
		{
			float x = (float)row / (float)clientWidth;
			float y = (float)col / (float)clientHeight;
			m_frameData[col * clientWidth + row] = tex.sample(x, y);
		}
	}
	update();
	*/
}

void Renderer::setScene(Scene * scene)
{
	m_scene = scene;
}



void Renderer::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	QImage image((uchar*)m_frameData, this->width(), this->height(), QImage::Format_RGB32);
	p.drawImage(0, 0, image);
}

void Renderer::mousePressEvent(QMouseEvent * e)
{
	static bool first = true;
	if (first)
	{
		first = false;
		m_lastX = e->pos().x();
		m_lastY = e->pos().y();
	}
	m_interact = true;
}

void Renderer::mouseMoveEvent(QMouseEvent * e)
{
	if (!m_scene)
	{
		return;
	}
	int x = e->pos().x();
	int y = e->pos().y();
	float dx = (0.25f * (x - m_lastX));
	float dy = (0.25f * (y - m_lastY));

    m_scene->camera.yaw -= dx;
	m_scene->camera.pitch += dy;
	if (m_scene->camera.pitch > 89.f)
	{
		m_scene->camera.pitch = 89.f;
	}
	if (m_scene->camera.pitch < -89.f)
	{
		m_scene->camera.pitch = -89.f;
	}
	m_lastX = x;
	m_lastY = y;
	render();
}

void Renderer::mouseReleaseEvent(QMouseEvent * e)
{
	m_interact = false;
}

void Renderer::wheelEvent(QWheelEvent * event)
{
	if (m_fov >= 1.0f && m_fov <= 90.0f)
	{
		m_fov -= event->delta() * 0.1;
	}

	if (m_fov <= 1.0f)
	{
		m_fov = 1.0f;
	}

	else if (m_fov >= 90.0f)
	{
		m_fov = 90.0f;
	}
}

Matrix4 Renderer::perspectiveMat(float near, float far)
{
	Matrix4 mat;
	float tanVar = tan((m_fov / 2) * (3.14159 / 180.f));
	float ratio = (float)width() / (float)height();
	mat.m[0][0] = 1.f / (ratio * tanVar);
	mat.m[1][1] = 1.f / tanVar;
	//mat[2][2] = near / (far - near);
	//mat[2][3] = near * far / (far - near);
	mat.m[2][2] = (far + near) / (near - far);
	mat.m[2][3] = -2 * near * far / (near - far);
	mat.m[3][2] = 1;
	mat.m[3][3] = 0;
	return mat;
}

void Renderer::drawFragment(const VertexOut & vertex1, const VertexOut & vertex2, const VertexOut & vertex3)
{
	int x1 = vertex1.point.x; int y1 = vertex1.point.y;
	int x2 = vertex2.point.x; int y2 = vertex2.point.y;
	int x3 = vertex3.point.x; int y3 = vertex3.point.y;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), clientWidth - 1);
	int maxy = std::min(findMax(y1, y2, y3), clientHeight - 1);

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


			int v1 = dx1 * dy2 - dx2 * dy1;
			int v2 = dx2 * dy3 - dx3 * dy2;
			int v3 = dx3 * dy1 - dx1 * dy3;

			if (v1 * v2 >= 0 && v1 * v3 >= 0 && v2 * v3 >= 0)
			{
				int temp = (-(x1 - x2) * (y3 - y2) + (y1 - y2) * (x3 - x2));
				float a = 0;
				if (temp != 0)
				{
					a = (float((-(x - x2) * (y3 - y2) + (y - y2) * (x3 - x2)))) / (float(temp));
				}
				float b = 0;
				temp = (-(x2 - x3) * (y1 - y3) + (y2 - y3) * (x1 - x3));
				if (temp != 0)
				{
					b = (float((-(x - x3) * (y1 - y3) + (y - y3) * (x1 - x3)))) / (float(temp));
				}
				float c = std::max((1.f - a - b), 0.f);
				

				float z = vertex1.z * a + vertex2.z * b + vertex3.z * c;
				int index = clientWidth * y + x;
				if (m_zbuffer[index] > z)
				{
					continue;
				}

				
				Vector3f pos = vertex1.vin.pos * a + vertex2.vin.pos * b + vertex1.vin.pos * c;
				if (pos.z < m_scene->nearPlane || pos.z > m_scene->farPlane)
				{
					continue;
				}
				Vector3f normal = vertex1.vin.normal * a + vertex2.vin.normal * b + vertex1.vin.normal * c;
				Vector3f color;
				if (m_shaderInfo.texture)
				{
					float u = vertex1.vin.tex.x * a + vertex2.vin.tex.x * b + vertex3.vin.tex.x * c;
					float v = vertex1.vin.tex.y * a + vertex2.vin.tex.y * b + vertex3.vin.tex.y * c;
					color = m_shaderInfo.texture->sample(u, v);
				}
				else
				{
					color = m_shaderInfo.color;
				}
				Vector3f litColor;
				for (const Light& light : m_scene->lights)
				{
					litColor = litColor + light.caculate(m_scene->camera.pos, pos, normal);
				}
				float* p = (float*)&litColor;
				for (int i = 0; i < 3; ++i)
				{
					if (*(p + i) > 1.f)
					{
						*(p + i) = 1.f;
					}
				}
				color = color * litColor;
				int red = color.x * 255;
				int green = color.y * 255;
				int blue = color.z * 255;
				unsigned int colorValue = (red << 16) + (green << 8) + blue;
				m_zbuffer[index] = z;
				m_frameData[index] = colorValue;
			}


		}
	}

}

void Renderer::drawSkyFragment(const Vector3f&v1, const Vector2f& p1, const Vector3f& v2, const Vector2f& p2, const Vector3f& v3, const Vector2f& p3, CubeTexture* texture)
{
	int x1 = p1.x; int y1 = p1.y;
	int x2 = p2.x; int y2 = p2.y;
	int x3 = p3.x; int y3 = p3.y;

	int minx = std::max(findMin(x1, x2, x3), 1);
	int miny = std::max(findMin(y1, y2, y3), 1);
	int maxx = std::min(findMax(x1, x2, x3), clientWidth - 1);
	int maxy = std::min(findMax(y1, y2, y3), clientHeight - 1);

	for (int y = miny; y <= maxy; ++y)
	{
		for (int x = minx; x <= maxx; ++x)
		{
			int index = clientWidth * y + x;
			if (m_zbuffer[index] > 0.f)
			{
				continue;
			}
			int dx1 = x1 - x;
			int dy1 = y1 - y;
			int dx2 = x2 - x;
			int dy2 = y2 - y;
			int dx3 = x3 - x;
			int dy3 = y3 - y;


			int c1 = dx1 * dy2 - dx2 * dy1;
			int c2 = dx2 * dy3 - dx3 * dy2;
			int c3 = dx3 * dy1 - dx1 * dy3;


			if (c1 * c2 >= 0 && c1 * c3 >= 0 && c2 * c3 >= 0)
			{
				int temp = (-(x1 - x2) * (y3 - y2) + (y1 - y2) * (x3 - x2));
				float a = 0;
				if (temp != 0)
				{
					a = (float((-(x - x2) * (y3 - y2) + (y - y2) * (x3 - x2)))) / (float(temp));
				}
				float b = 0;
				temp = (-(x2 - x3) * (y1 - y3) + (y2 - y3) * (x1 - x3));
				if (temp != 0)
				{
					b = (float((-(x - x3) * (y1 - y3) + (y - y3) * (x1 - x3)))) / (float(temp));
				}
				float c = std::max((1.f - a - b), 0.f);

				Vector3f dir = v1 * a + v2 * b + v3 * c;
				Vector3f color = texture->sample(dir);
				//color = { 1.f, 0.f, 0.f };
				float* p = (float*)&color;
				for (int i = 0; i < 3; ++i)
				{
					if (*(p + i) > 1.f)
					{
						*(p + i) = 1.f;
					}
				}
				int red = color.x * 255;
				int green = color.y * 255;
				int blue = color.z * 255;
				unsigned int colorValue = (red << 16) + (green << 8) + blue;
				m_frameData[index] = colorValue;

			}
		}
	}


				
}

void Renderer::clear()
{
	memset(m_zbuffer, 0, clientWidth * clientHeight * 4);
	memset(m_frameData, 0, clientWidth * clientHeight * 4);
}
