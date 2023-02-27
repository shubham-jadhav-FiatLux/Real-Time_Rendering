// Header Files
#include <windows.h>
#include "D3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// D3D11 Related Header Files
#include <dxgi.h>
#include <D3D11.h>

// D3D Related Libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D11.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
static BOOL gbFullscreen = FALSE;

// Entry-Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	HRESULT initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	
	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	//int iRetVal = 0;
	HRESULT hr = S_OK;

	// Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exiting ..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Successfully Opened.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Initialization Of WNDCLASSEX Structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// Create The Window
	hwnd = CreateWindow(szAppName,
		TEXT("Shubham Balaso Jadhav"),
		WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) / 2 - WIN_WIDTH / 2),
		(GetSystemMetrics(SM_CYSCREEN) / 2 - WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// initialize
	hr = initialize();
	if (FAILED(hr))
	{
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "initialize() failed.\n");
			fclose(gpFile);
			gpFile = NULL;
			DestroyWindow(hwnd);
			hwnd = NULL;
	}
	else
	{
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "initialize() Succeeded.\n");
			fclose(gpFile);
			gpFile = NULL;
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	// Foregrounding And Focusing The Window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				// Render The Scene
				display();

				// Update The Scene
				update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);
}

// Callback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullscreen(void);
	void resize(int, int);

	// Code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		break;				// As this is Retend mode graphics there is WM_PAINT to paint		

	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;

		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			DestroyWindow(hwnd);
			break;

		default:
			break;
		}
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), LOWORD(lParam));
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void ToggleFullscreen(void)
{
	// Variable Declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// Code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
}

HRESULT initialize(void)
{
	// Function Declarations
	HRESULT PrintD3DInfo(void);

	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	hr = PrintD3DInfo();

	return hr;
}

HRESULT PrintD3DInfo(void)
{
	// Variable Declarations
	HRESULT hr = S_OK;
	IDXGIFactory* pIDXGIFactory = NULL;
	IDXGIAdapter* pIDXGIAdapter = NULL;
	DXGI_ADAPTER_DESC dxgiAdapterDescripter;
	char str[255];

	// Code
	// Create DXGI Factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);
	if (FAILED(hr))
	{
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "CreateDXGIFactory() failed.\n");
			fclose(gpFile);
			gpFile = NULL;
			return hr;
	}
	else
	{
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "CreateDXGIFactory() Succeeded.\n");
			fclose(gpFile);
			gpFile = NULL;
	}

	// Get IDXGI Adapter
	if (pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		// Get description of bound adapter
		ZeroMemory((void*)&dxgiAdapterDescripter, sizeof(DXGI_ADAPTER_DESC));
		pIDXGIAdapter->GetDesc(&dxgiAdapterDescripter);

		WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDescripter.Description, 255, str, 255, NULL, NULL);
		
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "Graphic Card Name = %s.\n", str);
			fprintf(gpFile, "Graphic Card Memory(VRAM) = %d GB.\n", (int)ceil(dxgiAdapterDescripter.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0));
			fclose(gpFile);
			gpFile = NULL;
	}
	else
	{
			fopen_s(&gpFile, "Log.txt", "a+");
			fprintf(gpFile, "IDXGIAdapter::EnumAdapters failed.\n");
			fclose(gpFile);
			gpFile = NULL;
	}

	if (pIDXGIAdapter)
	{
		pIDXGIAdapter->Release();
		pIDXGIAdapter = NULL;
	}

	if (pIDXGIFactory)
	{
		pIDXGIFactory->Release();
		pIDXGIFactory = NULL;
	}

	return hr;
}

void resize(int width, int height)
{
	// Code
	if (height == 0)
		height = 1;					// To avoid 'Divide By 0' in future
}

void display(void)
{
	// Code

}

void update(void)
{
	// Code

}

void uninitialize(void)
{
	// Function Declarations
	void ToggleFullscreen(void);

	// Code
	if (gbFullscreen)
	{
		ToggleFullscreen();
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
