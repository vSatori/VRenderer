#pragma once
#include <Windows.h>
class Scene;

#define renderer RenderView::instance()
class RenderView
{
public:
	static RenderView* instance()
	{
		static RenderView view;
		return &view;
	}
private:
	RenderView();
	~RenderView();
public:
	void setScene(Scene* scene);
	bool init(HINSTANCE ins);
	int run();
	void renderScene();
	LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	Scene* m_currentScene{nullptr};
	bool m_interact{false};
	int m_lastX{0};
	int m_lastY{0};
	HWND m_wnd{nullptr};
	HDC m_wndc{nullptr};
	HDC m_fdc{nullptr};
	HBITMAP m_bitmap{nullptr};
	unsigned char* m_frameBuffer{nullptr};
};