#include "RenderView.h"
#include <qpainter.h>
#include <qevent.h>
#include "Scene.h"
#include "RenderContext.h"

RenderView::RenderView(QWidget *parent)
	: QWidget(parent), 
	m_currentScene(nullptr), 
	m_interact(false), 
	m_lastX(0.f), 
	m_lastY(0.f)
{
	RenderContext::init();
	setFixedSize(RenderContext::width, RenderContext::height);
}

RenderView::~RenderView()
{
	RenderContext::finalize();
}

void RenderView::renderScene()
{
	if (!m_currentScene)
	{
		return;
	}
	m_currentScene->render();
	update();
}

void RenderView::setScene(Scene * scene)
{
	m_currentScene = scene;
}

void RenderView::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	QImage image((uchar*)RenderContext::renderTarget, RenderContext::width, RenderContext::height, QImage::Format_RGB32);
	QPen pen;
	pen.setColor(Qt::red);
	p.setPen(pen);
	p.drawImage(0, 0, image);
	p.drawText(10, 10, QString("fps:%1").arg(fps));
}

void RenderView::mousePressEvent(QMouseEvent * e)
{
	m_lastX = e->pos().x();
	m_lastY = e->pos().y();
	m_interact = true;
}

void RenderView::mouseMoveEvent(QMouseEvent * e)
{
	if (!m_currentScene || !m_currentScene->camera.useSphereMode)
	{
		return;
	}
	Camera& camera = m_currentScene->camera;
	int x = e->pos().x();
	int y = e->pos().y();
	float dx = (0.25f * (x - m_lastX));
	float dy = (0.25f * (y - m_lastY));

	camera.yaw -= dx;
    camera.pitch += dy;
	if (camera.pitch > 89.f)
	{
		camera.pitch = 89.f;
	}
	if (camera.pitch < -89.f)
	{
		camera.pitch = -89.f;
	}
	m_lastX = x;
	m_lastY = y;
}

void RenderView::mouseReleaseEvent(QMouseEvent * e)
{
	m_interact = false;
}

void RenderView::wheelEvent(QWheelEvent * event)
{
	if (!m_currentScene)
	{
		return;
	}
	float& fov = m_currentScene->fov;
	if (fov >= 1.0f && fov <= 90.0f)
	{
		fov -= event->delta() * 0.1;
		return;
	}

	if (fov <= 1.0f)
	{
		fov = 1.0f;
	}

	else if (fov >= 90.0f)
	{
		fov = 90.0f;
	}
}
