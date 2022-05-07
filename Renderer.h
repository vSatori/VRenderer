#pragma once

#include <QtWidgets/QWidget>

#include "MathUtility.h"
#include "Camera.h"
#include "Mesh.h"
#include "ShaderInfo.h"
class Scene;
class Renderer : public QWidget
{
    Q_OBJECT

public:
    Renderer(QWidget *parent = Q_NULLPTR);
	~Renderer();
public:
	void render();
	void drawTexture2D(const QString& file);
	void setScene(Scene* scene);
	void drawTriangle();
protected:
	virtual void paintEvent(QPaintEvent* e)override;
	virtual void mousePressEvent(QMouseEvent* e)override;
	virtual void mouseMoveEvent(QMouseEvent* e)override;
	virtual void mouseReleaseEvent(QMouseEvent* e)override;
	virtual void wheelEvent(QWheelEvent *event)override;
private:
	Matrix4 perspectiveMat(float near, float far);
	Matrix4 modelMat();
	void drawFragment(const VertexOut& v1, const VertexOut& v2, const VertexOut& v3);
	void drawSkyFragment(const Vector3f&v1, const Vector2f& p1, float z1,  const Vector3f& v2, const Vector2f& p2, float z2,
		const Vector3f& v3, const Vector2f& p3, float z3, CubeMap* texture);
	void clear();
	bool checkCull(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	std::vector<Vector4f> sidesClip(const Vector4f& v1, const Vector4f& v2, const Vector4f& v3);
	Vector4f clipLine(const Vector4f& v1, const Vector4f& v2, const Vector4f& side, int index);
	Vector4f clipLineW(const Vector4f& v1, const Vector4f& v2);
	bool inside(const Vector4f& v, const Vector4f& line);
	bool inside(const Vector4f& v, int index);
	void generateCubeMap();
	

private:
	unsigned int* m_frameData;
	unsigned int* m_curFrameData;
	float* m_zbuffer;
	Scene* m_scene;
	ShaderInfo m_shaderInfo;
public:
	static float s_minX;
	static float s_maxX;
	static float s_minY;
	static float s_maxY;
	int m_count;
private:
	int m_lastX;
	int m_lastY;
	float m_fov;
	bool m_interact; 
	std::vector<int> m_pixels;
	std::vector<Vector4f> m_sides;
	std::vector<Vector3f> m_currentvs;
	std::vector<Vector3f> m_inputvs;
	std::vector<Vector3f> m_currentnvs;
	int m_index1;
	int m_index2;
	bool m_reflect;
};
