// Header Files
#include <windows.h>
#include "D3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// D3D11 Related Header Files
#include <dxgi.h>
#include <D3D11.h>
#include <d3dcompiler.h>

#pragma warning(disable:4838)
#include "XNAMath_204/xnamath.h"

// D3D Related Libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
static BOOL gbFullscreen = FALSE;

float anglePyramid = 0.0f;
float angleCube = 0.0f;

// D3D11 Related Global Variables
IDXGISwapChain* gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;
float clearColor[4];

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_PositionBuffer_Pyramid = NULL;
ID3D11Buffer* gpID3D11Buffer_ColorBuffer_Pyramid = NULL;
ID3D11Buffer* gpID3D11Buffer_PositionBuffer_Cube = NULL;
ID3D11Buffer* gpID3D11Buffer_ColorBuffer_Cube = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX perspectiveProjectionMatrix;

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
	HRESULT resize(int, int);

	// Variable Declarations
	HRESULT hr = S_OK;

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
		if (gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam), HIWORD(lParam));

			if (FAILED(hr))
			{
					fopen_s(&gpFile, "Log.txt", "a+");
					fprintf(gpFile, "WndProc : resize() failed.\n");
					fclose(gpFile);
					gpFile = NULL;
					DestroyWindow(hwnd);
					hwnd = NULL;
					return hr;
			}
		}
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
	HRESULT resize(int, int);

	// Variable Declarations
	HRESULT hr = S_OK;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE};
	D3D_FEATURE_LEVEL d3dFeatureLevel_Required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_Aquired = D3D_FEATURE_LEVEL_10_0;
	UINT numDrivers = 0;
	UINT createDeviceFlags = 0;
	UINT numFeatureLevels = 1;

	// Code
	hr = PrintD3DInfo();
	
	// Initialize Swap Chain Descripter
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDescripter;
	ZeroMemory((void*)&dxgiSwapChainDescripter, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDescripter.BufferCount = 1;
	dxgiSwapChainDescripter.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDescripter.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDescripter.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDescripter.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDescripter.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDescripter.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDescripter.OutputWindow = ghwnd;
	dxgiSwapChainDescripter.SampleDesc.Count = 1;
	dxgiSwapChainDescripter.SampleDesc.Quality = 0;
	dxgiSwapChainDescripter.Windowed = TRUE;

	// Call D3D11 CreateDeviceAndSwapChain() For Required Driver
	numDrivers = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);
	for (UINT i = 0; i < numDrivers; i++)
	{
		d3dDriverType = d3dDriverTypes[i];

		hr = D3D11CreateDeviceAndSwapChain( NULL,
											d3dDriverType,
											NULL,
											createDeviceFlags,
											&d3dFeatureLevel_Required,
											numFeatureLevels,
											D3D11_SDK_VERSION,
											&dxgiSwapChainDescripter,
											&gpIDXGISwapChain,
											&gpID3D11Device,
											&d3dFeatureLevel_Aquired,
											&gpID3D11DeviceContext
										);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "initialize : D3D11CreateDeviceAndSwapChain() failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		
		// Print Obtained Driver Type
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf(gpFile, "D3D11 Obtained Hardware Driver.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf(gpFile, "D3D11 Obtained Warp Driver.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf(gpFile, "D3D11 Obtained Reference Driver.\n");
		}
		else
		{
			fprintf(gpFile, "D3D11 Obtained Unknown Driver.\n");
		}

		// Print Obtained D3D Feature Level
		if (d3dFeatureLevel_Aquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf(gpFile, "D3D Obtained 11.0 Feature Level.\n");
		}
		else if (d3dFeatureLevel_Aquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf(gpFile, "D3D Obtained 10.1 Feature Level.\n");
		}
		else if (d3dFeatureLevel_Aquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf(gpFile, "D3D Obtained 10.0 Feature Level.\n");
		}
		else
		{
			fprintf(gpFile, "D3D Obtained Unknown Feature Level.\n");
		}

		fclose(gpFile);
		gpFile = NULL;
	}

	// Vertex Shader
	const char* vertexShaderSourceCode =
		"cbuffer constantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"struct Vertex" \
		"{" \
		"float4 position:SV_POSITION;" \
		"float4 color:COLOR;" \
		"};" \
		"Vertex main(float4 position:POSITION, float4 color:COLOR)" \
		"{" \
		"Vertex output;" \
		"output.position = mul(worldViewProjectionMatrix, position);" \
		"output.color = color;" \
		"return output;" \
		"}";

	ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	// Compile Vertex Shader
	hr = D3DCompile(vertexShaderSourceCode,
					lstrlenA(vertexShaderSourceCode) + 1,
					"VS",
					NULL,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main",
					"vs_5_0",
					0,
					0,
					&pID3DBlob_VertexShaderCode,
					&pID3DBlob_Error);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");

		if (pID3DBlob_Error)
		{
			fprintf(gpFile, "D3DCompile() Failed For Vertex Shader : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else
		{
			fprintf(gpFile, "D3DCompile() Failed For Vertex Shader : Unknown\n\n");
		}
		
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "D3DCompile() Succeeded For Vertex Shader.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Create Vertex Shader
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "ID3D11Device :: CreateVertexShader() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "ID3D11Device :: CreateVertexShader() Succeeded.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Set this vertex shader in vertex shader stage of pipeline
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);


	// Pixel Shader
	const char* pixelShaderSourceCode =
		"struct Vertex" \
		"{" \
		"float4 position:SV_POSITION;" \
		"float4 color:COLOR;" \
		"};" \
		"float4 main(Vertex input):SV_TARGET" \
		"{" \
		"float4 color;" \
		"color = input.color;" \
		"return color;" \
		"}";
		
	ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;

	// Compile Pixel Shader
	hr = D3DCompile(pixelShaderSourceCode,
					lstrlenA(pixelShaderSourceCode) + 1,
					"PS",
					NULL,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					"main",
					"ps_5_0",
					0,
					0,
					&pID3DBlob_PixelShaderCode,
					&pID3DBlob_Error);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");

		if (pID3DBlob_Error)
		{
			fprintf(gpFile, "D3DCompile() Failed For Pixel Shader : %s\n\n", (char*)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else
		{
			fprintf(gpFile, "D3DCompile() Failed For Pixel Shader : Unknown\n\n");
		}

		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "D3DCompile() Succeeded For Pixel Shader.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Create Pixel Shader
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(), pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "ID3D11Device :: CreatePixelShader() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "ID3D11Device :: CreatePixelShader() Succeeded.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Set this pixel shader in pixel shader stage of pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);


	// Input Layout
	// Initialize Input Layout Structure
	D3D11_INPUT_ELEMENT_DESC d3d11InputElementDescripter[2];
	ZeroMemory((void*)d3d11InputElementDescripter, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3d11InputElementDescripter));
	
	d3d11InputElementDescripter[0].SemanticName = "POSITION";
	d3d11InputElementDescripter[0].SemanticIndex = 0;
	d3d11InputElementDescripter[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3d11InputElementDescripter[0].AlignedByteOffset = 0;
	d3d11InputElementDescripter[0].InputSlot = 0;
	d3d11InputElementDescripter[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3d11InputElementDescripter[0].InstanceDataStepRate = 0;

	d3d11InputElementDescripter[1].SemanticName = "COLOR";
	d3d11InputElementDescripter[1].SemanticIndex = 0;
	d3d11InputElementDescripter[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3d11InputElementDescripter[1].AlignedByteOffset = 0;
	d3d11InputElementDescripter[1].InputSlot = 1;
	d3d11InputElementDescripter[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3d11InputElementDescripter[1].InstanceDataStepRate = 0;

	// Create Input Layout 
	hr = gpID3D11Device->CreateInputLayout(d3d11InputElementDescripter, _ARRAYSIZE(d3d11InputElementDescripter), pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateInputLayout() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateInputLayout() Succeeded.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Set this input layout in input assembly state of pipeline
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;


	// Geometry
	const float pyramidPosition[] = {
										// front
										+0.0f, +1.0f, +0.0f,
										+1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, -1.0f,

										// right
										+ 0.0f, +1.0f, +0.0f,
										+1.0f, -1.0f, +1.0f,
										+1.0f, -1.0f, -1.0f,

										// back
										+0.0f, +1.0f, +0.0f,
										-1.0f, -1.0f, +1.0f,
										+1.0f, -1.0f, +1.0f,

										// left
										+0.0f, +1.0f, +0.0f,
										-1.0f, -1.0f, -1.0f,
										-1.0f, -1.0f, +1.0f
									};

	const float pyramidColor[] = {
										// front
										+1.0f, +0.0f, +0.0f,
										+0.0f, +0.0f, +1.0f,
										+0.0f, +1.0f, +0.0f,

										// right
										+1.0f, +0.0f, +0.0f,
										+0.0f, +1.0f, +0.0f,
										+0.0f, +0.0f, +1.0f,

										// back
										+1.0f, +0.0f, +0.0f,
										+0.0f, +0.0f, +1.0f,
										+0.0f, +1.0f, +0.0f,

										// left
										+1.0f, +0.0f, +0.0f,
										+0.0f, +1.0f, +0.0f,
										+0.0f, +0.0f, +1.0f,
									};

	const float cubePosition[] = {
									// SIDE 1 ( TOP )
									-1.0f, +1.0f, +1.0f,
									+1.0f, +1.0f, +1.0f,
									-1.0f, +1.0f, -1.0f,

									-1.0f, +1.0f, -1.0f,
									+1.0f, +1.0f, +1.0f,
									+1.0f, +1.0f, -1.0f,

									// SIDE 2 ( BOTTOM )
									+1.0f, -1.0f, -1.0f,
									+1.0f, -1.0f, +1.0f,
									-1.0f, -1.0f, -1.0f,

									-1.0f, -1.0f, -1.0f,
									+1.0f, -1.0f, +1.0f,
									-1.0f, -1.0f, +1.0f,

									// SIDE 3 ( FRONT )
									-1.0f, +1.0f, -1.0f,
									+1.0f, +1.0f, -1.0f,
									-1.0f, -1.0f, -1.0f,

									-1.0f, -1.0f, -1.0f,
									+1.0f, +1.0f, -1.0f,
									+1.0f, -1.0f, -1.0f,

									// SIDE 4 ( BACK )
									+1.0f, -1.0f, +1.0f,
									+1.0f, +1.0f, +1.0f,
									-1.0f, -1.0f, +1.0f,

									-1.0f, -1.0f, +1.0f,
									+1.0f, +1.0f, +1.0f,
									-1.0f, +1.0f, +1.0f,

									// SIDE 5 ( LEFT )
									-1.0f, +1.0f, +1.0f,
									-1.0f, +1.0f, -1.0f,
									-1.0f, -1.0f, +1.0f,

									-1.0f, -1.0f, +1.0f,
									-1.0f, +1.0f, -1.0f,
									-1.0f, -1.0f, -1.0f,

									// SIDE 6 ( RIGHT )
									+1.0f, -1.0f, -1.0f,
									+1.0f, +1.0f, -1.0f,
									+1.0f, -1.0f, +1.0f,

									+1.0f, -1.0f, +1.0f,
									+1.0f, +1.0f, -1.0f,
									+1.0f, +1.0f, +1.0f

								};

	const float cubeColor[] = {
									// SIDE 1 ( TOP )
									+1.0f, +0.0f, +0.0f,
									+1.0f, +0.0f, +0.0f,
									+1.0f, +0.0f, +0.0f,

									+1.0f, +0.0f, +0.0f,
									+1.0f, +0.0f, +0.0f,
									+1.0f, +0.0f, +0.0f,

									// SIDE 2 ( BOTTOM )
									+0.0f, +1.0f, +0.0f,
									+0.0f, +1.0f, +0.0f,
									+0.0f, +1.0f, +0.0f,

									+0.0f, +1.0f, +0.0f,
									+0.0f, +1.0f, +0.0f,
									+0.0f, +1.0f, +0.0f,

									// SIDE 3 ( FRONT )
									+0.0f, +0.0f, +1.0f,
									+0.0f, +0.0f, +1.0f,
									+0.0f, +0.0f, +1.0f,

									+0.0f, +0.0f, +1.0f,
									+0.0f, +0.0f, +1.0f,
									+0.0f, +0.0f, +1.0f,

									// SIDE 4 ( BACK )
									+0.0f, +1.0f, +1.0f,
									+0.0f, +1.0f, +1.0f,
									+0.0f, +1.0f, +1.0f,

									+0.0f, +1.0f, +1.0f,
									+0.0f, +1.0f, +1.0f,
									+0.0f, +1.0f, +1.0f,

									// SIDE 5 ( LEFT )
									+1.0f, +0.0f, +1.0f,
									+1.0f, +0.0f, +1.0f,
									+1.0f, +0.0f, +1.0f,

									+1.0f, +0.0f, +1.0f,
									+1.0f, +0.0f, +1.0f,
									+1.0f, +0.0f, +1.0f,

									// SIDE 6 ( RIGHT )
									+1.0f, +1.0f, +0.0f,
									+1.0f, +1.0f, +0.0f,
									+1.0f, +1.0f, +0.0f,

									+1.0f, +1.0f, +0.0f,
									+1.0f, +1.0f, +0.0f,
									+1.0f, +1.0f, +0.0f
	};

	// Pyramid
	// Create Vertex Buffer For Position
	// A-Initialize Buffer Descripter
	D3D11_BUFFER_DESC d3d11BufferDescripter;
	ZeroMemory((void*)&d3d11BufferDescripter, sizeof(D3D11_BUFFER_DESC));

	d3d11BufferDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDescripter.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramidPosition);
	d3d11BufferDescripter.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// B-Initialize Subresource data structure to put data into the buffer
	D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	d3d11SubresourceData.pSysMem = pyramidPosition;

	// C-Create the Actual buffer
	hr = gpID3D11Device->CreateBuffer(&d3d11BufferDescripter, &d3d11SubresourceData, &gpID3D11Buffer_PositionBuffer_Pyramid);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Failed For Pyramid Position.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Succeeded For Pyramid Position.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Create Vertex Buffer For Color
	// A-Initialize Buffer Descripter
	ZeroMemory((void*)&d3d11BufferDescripter, sizeof(D3D11_BUFFER_DESC));
	d3d11BufferDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDescripter.ByteWidth = sizeof(float) * _ARRAYSIZE(pyramidColor);
	d3d11BufferDescripter.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// B-Initialize Subresource data structure to put data into the buffer
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = pyramidColor;

	// C-Create the Actual buffer
	hr = gpID3D11Device->CreateBuffer(&d3d11BufferDescripter, &d3d11SubresourceData, &gpID3D11Buffer_ColorBuffer_Pyramid);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Failed For Pyramid Color.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Succeeded For Pyramid Color.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Cube
	// Create Vertex Buffer For Position
	// A-Initialize Buffer Descripter
	ZeroMemory((void*)&d3d11BufferDescripter, sizeof(D3D11_BUFFER_DESC));
	d3d11BufferDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDescripter.ByteWidth = sizeof(float) * _ARRAYSIZE(cubePosition);
	d3d11BufferDescripter.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// B-Initialize Subresource data structure to put data into the buffer
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = cubePosition;

	// C-Create the Actual buffer
	hr = gpID3D11Device->CreateBuffer(&d3d11BufferDescripter, &d3d11SubresourceData, &gpID3D11Buffer_PositionBuffer_Cube);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Failed For Cube Position.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Succeeded For Cube Position.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Create Vertex Buffer For Color
	// A-Initialize Buffer Descripter
	ZeroMemory((void*)&d3d11BufferDescripter, sizeof(D3D11_BUFFER_DESC));
	d3d11BufferDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDescripter.ByteWidth = sizeof(float) * _ARRAYSIZE(cubeColor);
	d3d11BufferDescripter.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// B-Initialize Subresource data structure to put data into the buffer
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = cubeColor;

	// C-Create the Actual buffer
	hr = gpID3D11Device->CreateBuffer(&d3d11BufferDescripter, &d3d11SubresourceData, &gpID3D11Buffer_ColorBuffer_Cube);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Failed For Cube Color.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Succeeded For Cube Color.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Constant Buffer
	// A-Initialize Buffer Descripter
	ZeroMemory((void*)&d3d11BufferDescripter, sizeof(D3D11_BUFFER_DESC));

	d3d11BufferDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11BufferDescripter.ByteWidth = sizeof(CBUFFER);
	d3d11BufferDescripter.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// B-Create the Actual buffer
	hr = gpID3D11Device->CreateBuffer(&d3d11BufferDescripter, NULL, &gpID3D11Buffer_ConstantBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Failed For Constant Buffer.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateBuffer() Succeeded For Constant Buffer.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// C-Set constant buffer into the vertex shader stage of pipeline
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);


	// Enabling Rasterization Stage
	// A-Initialize Rasterizer Descripter
	D3D11_RASTERIZER_DESC d3d11RasterizerDescripter;
	ZeroMemory((void*)&d3d11RasterizerDescripter, sizeof(D3D11_RASTERIZER_DESC));

	d3d11RasterizerDescripter.CullMode = D3D11_CULL_NONE;
	d3d11RasterizerDescripter.FillMode = D3D11_FILL_SOLID;
	d3d11RasterizerDescripter.FrontCounterClockwise = FALSE;
	d3d11RasterizerDescripter.MultisampleEnable = FALSE;
	d3d11RasterizerDescripter.ScissorEnable = FALSE;
	d3d11RasterizerDescripter.DepthClipEnable = TRUE;
	d3d11RasterizerDescripter.AntialiasedLineEnable = FALSE;
	d3d11RasterizerDescripter.DepthBias = 0;
	d3d11RasterizerDescripter.DepthBiasClamp = 0.0f;
	d3d11RasterizerDescripter.SlopeScaledDepthBias = FALSE;

	// B-Create Rasterization stage Accordingly
	hr = gpID3D11Device->CreateRasterizerState(&d3d11RasterizerDescripter, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateRasterizerState() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateRasterizerState() Succeeded.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	// Set this stage into rasterization stage of pipeline
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);


	// Initialize Clear Color Array
	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;		// Black
	clearColor[3] = 1.0f;		// Alpha

	perspectiveProjectionMatrix = XMMatrixIdentity();

	// Warmup Resize
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "initialize : resize() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}
	else
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "initialize : resize() Succeeded.\n");
		fclose(gpFile);
		gpFile = NULL;
	}

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
		fprintf(gpFile, "\n---------------------------------------------------------------------\n");
		fprintf(gpFile, "Graphic Card Name = %s.\n", str);
		fprintf(gpFile, "Graphic Card Memory(VRAM) = %d GB.\n", (int)ceil(dxgiAdapterDescripter.DedicatedVideoMemory / 1024.0 / 1024.0 / 1024.0));
		fprintf(gpFile, "-----------------------------------------------------------------------\n\n");
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

HRESULT resize(int width, int height)
{
	// Variable Declarations
	HRESULT hr = S_OK;

	// Code
	if (height <= 0)
		height = 1;					// To avoid 'Divide By 0' in future

	// Release Existing RTV
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	// Release Existing DSV
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	// Tell SwapChain To Resize Buffers According To New Width And Height
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Get new resized buffer from swapchain and fill it into a dumy texture
	ID3D11Texture2D* pID3D11Texture2D_BackBuffer = NULL;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pID3D11Texture2D_BackBuffer);

	// Now create new RTV using above buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "resize() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}

	// Release dumy texture interface
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	//Initialize texture 2D descripter
	D3D11_TEXTURE2D_DESC d3d11Texture2DDescripter;
	ZeroMemory((void*)&d3d11Texture2DDescripter, sizeof(D3D11_TEXTURE2D_DESC));

	d3d11Texture2DDescripter.Width = (UINT)width;
	d3d11Texture2DDescripter.Height = (UINT)height;
	d3d11Texture2DDescripter.ArraySize = 1;
	d3d11Texture2DDescripter.MipLevels = 1;
	d3d11Texture2DDescripter.SampleDesc.Count = 1;
	d3d11Texture2DDescripter.SampleDesc.Quality = 0;
	d3d11Texture2DDescripter.Usage = D3D11_USAGE_DEFAULT;
	d3d11Texture2DDescripter.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11Texture2DDescripter.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3d11Texture2DDescripter.CPUAccessFlags = 0;
	d3d11Texture2DDescripter.MiscFlags = 0;

	// declare 2D Texture which is going to be conveterted into depth buffer
	ID3D11Texture2D* pID3D11Texture2D_DepthBuffer = NULL;
	hr = gpID3D11Device->CreateTexture2D(&d3d11Texture2DDescripter, NULL, &pID3D11Texture2D_DepthBuffer);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateTexture2D() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;
		return hr;
	}

	// Initialize depth stensile view descripter
	D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DepthStencilViewDescripter;
	ZeroMemory((void*)&d3d11DepthStencilViewDescripter, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	d3d11DepthStencilViewDescripter.Format = DXGI_FORMAT_D32_FLOAT;
	d3d11DepthStencilViewDescripter.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	// Now create depth stencil view using above depth buffer and descripter
	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer, &d3d11DepthStencilViewDescripter, &gpID3D11DepthStencilView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "gpID3D11Device :: CreateDepthStencilView() Failed.\n");
		fclose(gpFile);
		gpFile = NULL;

		pID3D11Texture2D_DepthBuffer->Release();
		pID3D11Texture2D_DepthBuffer = NULL;
		
		return hr;
	}
	// Release local depth buffer
	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	// Set this new RTV in pipeline 
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	// Initialize viewport structure
	D3D11_VIEWPORT d3d11Viewport;
	d3d11Viewport.TopLeftX = 0.0f;
	d3d11Viewport.TopLeftY = 0.0f;
	d3d11Viewport.Width = (float)width;
	d3d11Viewport.Height = (float)height;
	d3d11Viewport.MinDepth = 0.0f;
	d3d11Viewport.MaxDepth = 1.0f;

	// Tell device context to Set this viewport in pipeline
	gpID3D11DeviceContext->RSSetViewports(1, &d3d11Viewport);

	// Set Projection Matrix
	perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

	return hr;
}

void display(void)
{
	// Code
	// Clear RTV By Using clearColor
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, clearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Pyramid
	// Set Position Buffer Into IA Stage Of  Pipeline
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer_Pyramid, &stride, &offset);

	// Set Color Buffer Into IA Stage Of  Pipeline
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_ColorBuffer_Pyramid, &stride, &offset);

	// Set Primitive Topology In IA Stage Of Pipeline
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Transformations
	// A-Initialize Matrices
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(-1.5f, 0.0f, 6.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(anglePyramid));
	worldMatrix = rotationMatrix * translationMatrix;									// Here Order Is Imp
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

	// B-Put matrices Into Constant Buffer
	CBUFFER constantBuffer;
	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	// C-Push Them Into The Shader
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// Draw The Primitive
	gpID3D11DeviceContext->Draw(12, 0);


	// Cube
	// Set Position Buffer Into IA Stage Of  Pipeline
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_PositionBuffer_Cube, &stride, &offset);

	// Set Color Buffer Into IA Stage Of  Pipeline
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_ColorBuffer_Cube, &stride, &offset);

	// Set Primitive Topology In IA Stage Of Pipeline
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Transformations
	// A-Initialize Matrices
	translationMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix_X = XMMatrixIdentity();
	XMMATRIX rotationMatrix_Y = XMMatrixIdentity();
	XMMATRIX rotationMatrix_Z = XMMatrixIdentity();
	rotationMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixIdentity();
	viewMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(1.5f, 0.0f, 6.0f);
	rotationMatrix_X = XMMatrixRotationX(XMConvertToRadians(angleCube));
	rotationMatrix_Y = XMMatrixRotationY(XMConvertToRadians(angleCube));
	rotationMatrix_Z = XMMatrixRotationZ(XMConvertToRadians(angleCube));
	scaleMatrix = XMMatrixScaling(0.75, 0.75, 0.75);
	rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;
	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;									// Here Order Is Imp
	wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

	// B-Put matrices Into Constant Buffer
	ZeroMemory((void*)&constantBuffer, sizeof(CBUFFER));
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;

	// C-Push Them Into The Shader
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// Draw The Primitive
	gpID3D11DeviceContext->Draw(6, 0);
	gpID3D11DeviceContext->Draw(6, 6);
	gpID3D11DeviceContext->Draw(6, 12);
	gpID3D11DeviceContext->Draw(6, 18);
	gpID3D11DeviceContext->Draw(6, 24);
	gpID3D11DeviceContext->Draw(6, 30);

	// Swap Buffers By Presenting Them
	gpIDXGISwapChain->Present(0, 0);
}

void update(void)
{
	// Code
	anglePyramid += 0.1f;
	if (anglePyramid >= 360.0f)
		anglePyramid -= 360.0f;

	angleCube += 0.1f;
	if (angleCube >= 360.0f)
		angleCube -= 360.0f;
}

void uninitialize(void)
{
	// Function Declarations
	void ToggleFullscreen(void);

	// Code
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if (gpID3D11Buffer_ColorBuffer_Cube)
	{
		gpID3D11Buffer_ColorBuffer_Cube->Release();
		gpID3D11Buffer_ColorBuffer_Cube = NULL;
	}

	if (gpID3D11Buffer_PositionBuffer_Cube)
	{
		gpID3D11Buffer_PositionBuffer_Cube->Release();
		gpID3D11Buffer_PositionBuffer_Cube = NULL;
	}

	if (gpID3D11Buffer_ColorBuffer_Pyramid)
	{
		gpID3D11Buffer_ColorBuffer_Pyramid->Release();
		gpID3D11Buffer_ColorBuffer_Pyramid = NULL;
	}

	if (gpID3D11Buffer_PositionBuffer_Pyramid)
	{
		gpID3D11Buffer_PositionBuffer_Pyramid->Release();
		gpID3D11Buffer_PositionBuffer_Pyramid = NULL;
	}

	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	if (gbFullscreen)
	{
		ToggleFullscreen();
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
}
