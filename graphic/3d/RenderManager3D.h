#pragma once

#include "Mesh3D.h"
#include "Texture3D.h"
#include "objects/3d/Sprites3D.h"
#include "util/COMUtils.h"


namespace gamelib
{

	// Define the SwapChain requirements
	class MySwapChain
	{
	public:
		IDXGISwapChain* D3DInterface;	

		MySwapChain() { InitMemory(); }

		// Define the SwapChain requirements
		MySwapChain(UINT backBufferWidth, UINT backBufferHeight, HWND outputWindow) : backBufferHeight(backBufferHeight), backBufferWidth(backBufferWidth), hwnd(outputWindow), D3DInterface(NULL) 
		{	
			InitMemory();
			Setup();
		}
		
		inline DXGI_SWAP_CHAIN_DESC GetDescription() {return scd;}	
		inline D3D10_TEXTURE2D_DESC GetBackBufferDescription() { return backBufferInfo; }
		
		// Re-Interpret/Cast the backbuffer itself as a ID3DTexture2D object
		ID3D10Texture2D* GetBackBufferAsTexture()
		{	
			if(D3DInterface == NULL)
				return NULL;

			HRESULT hr = D3DInterface->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &backBuffer);
			if(FAILED(hr)){
				NULL;
			}

			// Now get more details about he back buffer
			backBuffer->GetDesc(&backBufferInfo);
			return backBuffer;
		}


	private:	
		UINT backBufferWidth;
		UINT backBufferHeight;
		HWND hwnd;
		DXGI_SWAP_CHAIN_DESC scd;
		inline void Setup()
		{		
			scd.BufferCount = 1;
			scd.BufferDesc.Width = backBufferWidth;
			scd.BufferDesc.Height = backBufferHeight;
			scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scd.BufferDesc.RefreshRate.Numerator = 60;
			scd.BufferDesc.RefreshRate.Denominator = 1;
			scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scd.OutputWindow = hwnd;
			scd.SampleDesc.Count = 1;
			scd.SampleDesc.Quality = 0;
			scd.Windowed = TRUE;
		}
		inline void InitMemory()
		{
			ZeroMemory(&backBufferInfo, sizeof(D3D10_TEXTURE2D_DESC));
			ZeroMemory(&scd, sizeof(scd));
		}
		ID3D10Texture2D* backBuffer = NULL;
		D3D10_TEXTURE2D_DESC backBufferInfo = {};
	};


	class D3DRenderManager
	{
	private:

		float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	protected:
			D3DRenderManager();
			D3DRenderManager(D3DRenderManager const&)  = delete;
	        void operator=(D3DRenderManager const&)  = delete;
	public:
		static D3DRenderManager& GetInstance()
	    {
	        static D3DRenderManager instance;			
	        return instance;
	    }
		D3D10_DRIVER_TYPE driverType;
		ID3D10Device* d3dDevice;
		
		// My abstraction of the swap chain
		MySwapChain swapChain;
		ID3D10RenderTargetView* renderTargetView;
		
		HINSTANCE hInst;
		HWND hwnd;
		D3D10_VIEWPORT viewPort;

		Sprites3D sprites;
		std::list<Texture3D*> textures;
		std::list<Mesh3D*> meshes;
		bool Initialize(HINSTANCE hInstance, unsigned int width, unsigned int height, bool fullScreen, const char* windowTitle);
		HRESULT CreateSwapChainAndDevice();
		void free();
		bool update();
		
	};

}