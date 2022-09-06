#include "RenderView.h"
#include "Scene.h"
#include "RenderContext.h"


LRESULT MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return renderer->ProcessMessage(hwnd, msg, wParam, lParam);
}


RenderView::RenderView()
{

}

RenderView::~RenderView()
{
	ReleaseDC(m_wnd, m_fdc);
	DeleteObject(m_bitmap);
	DeleteDC(m_fdc);
}
void RenderView::setScene(Scene* scene)
{
	m_currentScene = scene;
	RenderContext::cxt_nearPlane = scene->nearPlane;
	RenderContext::cxt_farPlane = scene->farPlane;
}

bool RenderView::init(HINSTANCE ins)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ins;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "VRenderWindow";

	if (!RegisterClass(&wc))
	{
		return false;
	};

	RECT R = { 0, 0, RenderContext::cxt_frameWidth, RenderContext::cxt_frameHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_wnd = CreateWindow("VRenderWindow", "VRenderer",
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, ins, 0);
	if (!m_wnd)
	{
		return false;
	}
	ShowWindow(m_wnd, SW_SHOW);
	UpdateWindow(m_wnd);

	m_wndc = GetDC(m_wnd);
	m_fdc = CreateCompatibleDC(m_wndc);
	if (!m_fdc)
	{
		return false;
	}

	BITMAPINFO info = { { sizeof(BITMAPINFOHEADER), RenderContext::cxt_frameWidth, -RenderContext::cxt_frameHeight, 1, 32, BI_RGB,
		RenderContext::cxt_frameWidth * RenderContext::cxt_frameHeight * 4, 0, 0, 0, 0 } };
	LPVOID ptr;
	m_bitmap = CreateDIBSection(m_fdc, &info, DIB_RGB_COLORS, &ptr, 0, 0);
	if (!m_bitmap)
	{
		return false;
	}
	SelectObject(m_fdc, m_bitmap);
	m_frameBuffer = (unsigned char*)ptr;
	return true;
}


int RenderView::run()
{
	MSG msg = { 0 };
	unsigned int dfps = 0;
	unsigned int fps = 0;
	unsigned long long diff = 0;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (diff >= 1000)
			{
				diff = 0;
				fps = dfps;
				dfps = 0;
			}
			auto s = GetTickCount64();
			renderScene();
			std::string txt = "fps: ";
			txt	+= std::to_string(fps);
			TextOut(m_wndc, 30, 30, txt.c_str(), txt.size());
			diff += GetTickCount64() - s;
			++dfps;
		}
	}

	return (int)msg.wParam;
}

void RenderView::renderScene()
{
	if (!m_currentScene)
	{
		return;
	}
	m_currentScene->render();
	memcpy(m_frameBuffer, RenderContext::cxt_renderTarget, RenderContext::cxt_frameWidth * RenderContext::cxt_frameHeight * 4);
	HDC dc = GetDC(m_wnd);
	BitBlt(dc, 0, 0, RenderContext::cxt_frameWidth, RenderContext::cxt_frameHeight, m_fdc, 0, 0, SRCCOPY);
	ReleaseDC(m_wnd, dc);
}

LRESULT RenderView::ProcessMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_MENUCHAR:
	{
		return MAKELRESULT(0, MNC_CLOSE);
	}
	case WM_LBUTTONDOWN:
	{
		m_lastX = LOWORD(lParam);
		m_lastY = HIWORD(lParam);
		m_interact = true;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		m_interact = false;
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (!m_interact)
		{
			return 0;
		}
		if (!m_currentScene || !m_currentScene->camera.useSphereMode)
		{
			return 0;
		}
		Camera& camera = m_currentScene->camera;
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
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
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		if (!m_currentScene)
		{
			return 0;
		}
		float& fov = m_currentScene->fov;
		if (fov >= 1.0f && fov <= 90.0f)
		{
			fov -= (float)GET_WHEEL_DELTA_WPARAM(wParam) * 0.05f;
			return 0;
		}

		if (fov <= 1.0f)
		{
			fov = 1.0f;
		}

		else if (fov >= 90.0f)
		{
			fov = 90.0f;
		}
		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}