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
protected:
	virtual void paintEvent(QPaintEvent* e)override;
	virtual void mousePressEvent(QMouseEvent* e)override;
	virtual void mouseMoveEvent(QMouseEvent* e)override;
	virtual void mouseReleaseEvent(QMouseEvent* e)override;
	virtual void wheelEvent(QWheelEvent *event)override;
private:
	Matrix4 perspectiveMat(float near, float far);
	void drawFragment(const VertexOut& v1, const VertexOut& v2, const VertexOut& v3);
	void drawSkyFragment(const Vector3f&v1, const Vector2f& p1, const Vector3f& v2, const Vector2f& p2, const Vector3f& v3, const Vector2f& p3, CubeTexture* texture);
	void clear();
private:
	unsigned int* m_frameData;
	float* m_zbuffer;
	Scene* m_scene;
	ShaderInfo m_shaderInfo;
private:
	int m_lastX;
	int m_lastY;
	float m_fov;
	bool m_interact; 

};
