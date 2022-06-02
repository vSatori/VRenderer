
#include <QtWidgets/QApplication>
#include <QtCore/qtimer.h>
#include <Windows.h>
#include "Scene.h"
#include "RenderView.h"
#include "RayTracingScene.h"
#include <QTcore/qdebug.h>
#include <iostream>
#include <xmmintrin.h>
#include <iostream>

#include <random>

static float rand_double()
{
	std::random_device rd;
	std::uniform_real_distribution<double> dist(0.0, 1.0);
	std::mt19937 gen(rd());
	float r = dist(gen);
	return r;
}


std::vector<double> ds;

static bool isExit = false;
int main(int argc, char *argv[])
{



	QApplication a(argc, argv);
	RenderView view;
	RayTracingScene scene;
	view.setScene(&scene);
	QTimer timer;
	timer.setInterval(500);
	timer.setSingleShot(true);
	unsigned int fps = 0;
	unsigned long long diff = 0;
	QObject::connect(&timer, &QTimer::timeout, [&view, &diff, &fps]()
		{
			while (!isExit)
			{
				if (diff >= 1000)
				{
					diff = 0;
					view.fps = fps;
					fps = 0;
				}
				auto s = GetTickCount64();
				view.renderScene();
				QApplication::processEvents();
				diff += GetTickCount64() - s;
				++fps;
			}

		});
	view.show();
	view.renderScene();
	//timer.start();
    a.exec();
	isExit = true;
	return 0;
}
