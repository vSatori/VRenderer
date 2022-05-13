#pragma once

#include <QWidget>
class Scene;

class RenderView : public QWidget
{
public:
	RenderView(QWidget *parent = Q_NULLPTR);
	~RenderView();
public:
	void renderScene();
	void setScene(Scene* scene);
protected:
	virtual void paintEvent(QPaintEvent* e)override;
	virtual void mousePressEvent(QMouseEvent* e)override;
	virtual void mouseMoveEvent(QMouseEvent* e)override;
	virtual void mouseReleaseEvent(QMouseEvent* e)override;
	virtual void wheelEvent(QWheelEvent *event)override;
public:
	unsigned int fps;
private:
	Scene* m_currentScene;
	bool m_interact;
	int m_lastX;
	int m_lastY;
};
