#include "RenderManager3D.h"

namespace gamelib
{
	D3DRenderManager::D3DRenderManager()
	{
		driverType = D3D10_DRIVER_TYPE_NULL;
		d3dDevice = NULL;	
		renderTargetView = NULL;
		hInst = NULL;
		hwnd = NULL;	
	}

	LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lparam)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		switch(message)
		{
			case WM_PAINT:
				hdc = BeginPaint( hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc( hWnd, message, wParam, lparam);
		}
		return 0;
	}

	class Window3D
	{
	public:	
		// Define a Window to draw in
		Window3D(HINSTANCE hInstance, int width, int height) : Window3D()
		{
			rect = { 0, 0, width, height };
			this->hInstance = hInstance;

			// Define a Window		
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WndProc;
			wcex.cbWndExtra = 0;
			wcex.cbClsExtra = 0;
			wcex.hInstance = hInstance;
			wcex.lpszMenuName = NULL;
			wcex.lpszClassName = L"class_engine";
		}	

		bool Initialize()
		{
			// Register the window with Windows
			if(!RegisterClass())
				return false;
		
			// Calculate and get the size of a WS_OVERLAPPEDWINDOW window		
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
			initialized = true;
			return true;
		}

		// Create the window as a Hwnd
		HWND Create()
		{
			if(!initialized)
				return NULL;
			// Calculate and get the size of a WS_OVERLAPPEDWINDOW window
			AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
		
			// Create the window as a Hwnd
			hwnd = CreateWindow( L"class_engine", L"Window Title goes here dude", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
			return hwnd;
		}
		
		// Get drawable region of window (ClientRect)
		RECT GetWindowClientArea()
		{
			ZeroMemory(&rect, sizeof(RECT));
			GetClientRect(hwnd, &rect);
			return rect;
		}

		// Show the Window
		void Show()
		{
			ShowWindow(hwnd, SW_SHOWNORMAL);
		}
	private:
		WNDCLASSEX wcex;
		HWND hwnd;
		RECT rect = {};
		HINSTANCE hInstance;
		bool initialized = false;

		// Create window
		Window3D()		
		{		
			ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		}

		// Register class
		WORD RegisterClass()
		{
			if(!RegisterClassEx(&wcex))
				return false;
		}
	};


	bool D3DRenderManager::Initialize(HINSTANCE hInstance, unsigned int width, unsigned int height, bool fullScreen, const char* windowTitle)
	{	
		Window3D window(hInstance, width, height);
		hInst = hInstance;

		if(!window.Initialize())
			return false;	
		
		if(!(hwnd = window.Create()))
			return false;	
		
		window.Show();	

		RECT clientArea = window.GetWindowClientArea();

		swapChain = MySwapChain(clientArea.right - clientArea.left, clientArea.bottom - clientArea.top, hwnd);	
			
		if(FAILED(CreateSwapChainAndDevice()))
			return false;

		// Create a render target view from the back buffer
		HRESULT hr = d3dDevice->CreateRenderTargetView(swapChain.GetBackBufferAsTexture(), NULL, &renderTargetView);	
		
		swapChain.GetBackBufferAsTexture()->Release();
		if(FAILED(hr))
			return false;

		// Register the render target view with the device
		d3dDevice->OMSetRenderTargets(1, &renderTargetView, NULL);

		// Define the viewport
		D3D10_VIEWPORT viewPort;
		viewPort.Width = width;
		viewPort.Height = height;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;

		// Register the viewport with the device
		d3dDevice->RSSetViewports( 1, &viewPort);
		
		this->viewPort = viewPort;
		return true;
	}

	// See which driver type we can get to create the a) device and b) swap chain with	
	HRESULT D3DRenderManager::CreateSwapChainAndDevice()
	{
		HRESULT hr = S_OK;
		D3D10_DRIVER_TYPE driverTypes[] =
		{
			D3D10_DRIVER_TYPE_HARDWARE,
			D3D10_DRIVER_TYPE_REFERENCE
		};
		UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);
		UINT createDeviceFlags = 0;

		for (UINT i = 0; i < numDriverTypes; i++) {
			auto a = swapChain.GetDescription();
			hr = D3D10CreateDeviceAndSwapChain(nullptr, driverTypes[i], nullptr, createDeviceFlags, D3D10_SDK_VERSION, &a, &swapChain.D3DInterface, &d3dDevice);
			if (SUCCEEDED(hr))
				break;
		}
		return hr;
	}

	void D3DRenderManager::free()
	{
		// Free textures
		for( auto texture : textures) {
			texture->free();
		}
		// Free meshes
		for(auto mesh : meshes){ 
			mesh->free();
		}

		// Free sprites
		sprites.free();

		// Clear Device state
		if(d3dDevice)
			d3dDevice->ClearState();
		
		// Free render target, swapchain and device
		SAFE_RELEASE(renderTargetView);
		SAFE_RELEASE(swapChain.D3DInterface);
		SAFE_RELEASE(d3dDevice);
	}

	bool D3DRenderManager::update()
	{
		// Blank out the drawing area
		d3dDevice->ClearRenderTargetView(renderTargetView, clearColor);
		
		// Update all drawings
		sprites.update();
		for( auto mesh : meshes)
		{
			mesh->update();
		}
		
		// Show the updated drawings
		swapChain.D3DInterface->Present(0,0);
		return true;
	}
}
