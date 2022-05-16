
#include <QtWidgets/QApplication>
#include <qtimer.h>
#include <Windows.h>
#include <qtimer.h>
#include "Scene.h"
#include "RenderView.h"
#include <qdebug.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	RenderView view;
	PmxModelScene scene;
	view.setScene(&scene);
	QTimer timer;
	timer.setInterval(30);
	unsigned int fps = 0;
	QObject::connect(&timer, &QTimer::timeout, [&view]()
	{
		unsigned int m = 1000;
		unsigned int s = GetTickCount64();
		view.renderScene();
		unsigned int diff = GetTickCount64() - s;
		diff += 1;
		view.fps = m / (unsigned int)(diff);
		QApplication::processEvents();
	});
	view.show();
	view.renderScene();
	timer.start();
    return a.exec();
}
