﻿#include "precompiled.h"

#include <win32/irradiance.h>

#include <sys/sys_base.h>

#include <dx11/dx11_system.h>
#include <fnd/fnd_universe.h>

#include <gx/gx_initializer.h>
#include <gx/gx_render_context.h>


#include <win32/application.h>
#include <win32/window.h>

#include <iostream>
#include <fstream>
#include <math/math_functions.h>

#define MAX_LOADSTRING 100

// Global Variables:
static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
static ATOM				MyRegisterClass(HINSTANCE hInstance);
static HWND				InitInstance(HINSTANCE, int);
static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

using namespace wnd;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        std::cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart) / 1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart) / PCFreq;
}

extern std::shared_ptr<gx::scene> universe_bootstrap(  dx11::system_context context, std::shared_ptr<fnd::universe> universe );

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    UNUSED_PARAMETER(hInstance);
	UNUSED_PARAMETER(nCmdShow);

	UNUSED_PARAMETER(hPrevInstance);
	UNUSED_PARAMETER(lpCmdLine);

	
    // Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IRRADIANCE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hwnd = InitInstance (hInstance, nCmdShow);

	if (hwnd == 0)
	{
		return FALSE;
	}

	RECT r;
	::GetClientRect(hwnd, &r);

	//1. Initialize static graphic data (singletons)
	gx::initializer			initializer;

	//2. Create a default view port
	gx::view_port			view_port( r.left, r.top, r.right - r.left, r.bottom - r.top );
    //3. Create direct3d11 structures
	dx11::system_context	context = dx11::create_system_context(hwnd);
	//4. Create render contexts
	gx::render_context		render_context(context, 3, view_port);
	//5. Create application with a window and worlds with data
	application				application;

    std::shared_ptr<fnd::universe> universe = std::make_shared<fnd::universe>();
    std::shared_ptr<gx::scene> scene = universe_bootstrap( context, universe );

    application.set_universe(universe);
    application.set_scene(scene);

	window* wnd = new window( application, context.m_swap_chain, &render_context );
	application.new_window(wnd);
	wnd->set_scene(scene);
	::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG>(wnd) );

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IRRADIANCE));
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				application.update();
				application.render();
			}
		}
		else
		{
				application.update();
				application.render();
		}
	}

	return (int) msg.wParam;
}

static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbWndExtra		= 0;
	wcex.cbClsExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IRRADIANCE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IRRADIANCE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

static HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return 0;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	window* wnd = reinterpret_cast<window*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		if (wnd !=0 )
		{
			wnd->render();
			ValidateRect( hWnd, NULL );
		}
		break;

	case WM_SIZE:
		{
			if (wnd !=0 )
			{
				wnd->resize_window(LOWORD(lParam), HIWORD(lParam));
				wnd->render();
			}
		}
		break;

	case WM_DESTROY:
		if (wnd)
		{
			wnd->destroy();
		}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Message handler for about box.
static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

