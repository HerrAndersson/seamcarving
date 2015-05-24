#include "stdafx.h"
#include "ToScreen.h"
#include "Application.h"

//#include "Console.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>

HINSTANCE				g_hInst					= nullptr;  
HWND					g_hWnd					= nullptr;
ToScreen*				Screen					= nullptr;
Application*            App						= nullptr;

HRESULT                 InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT					Render();
void					Update(float deltaTime);

//Create Direct3D device and swap chain
HRESULT Init()
{
	//RedirectIOToConsole();
	Screen = new ToScreen();
	Screen->Init(g_hWnd);

	return S_OK;
}

HRESULT Render()
{
	Screen->Render();

	return S_OK;
}

void CreateApp()
{
	App = new Application(Screen);
}

// Entry point to the program. Initializes everything and goes into a message processing loop.
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( Init() ) )
		return 0;

	CreateApp();

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//render
			bool result = App->Update(dt);
			if (!result)
				return WM_QUIT;
			Render();

			prevTimeStamp = currTimeStamp;
		}
	}

	SAFE_DELETE(Screen);
	SAFE_DELETE(App);

	return (int) msg.wParam;
}

// Register class and create window
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = nullptr;
	wcex.lpszClassName  = L"SeamCarving";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if (!(g_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"SeamCarving", L"SeamCarving", WS_OVERLAPPEDWINDOW | CW_USEDEFAULT | CW_USEDEFAULT, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr)))
		return E_FAIL;

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}

// Called every time the application receives a message
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}