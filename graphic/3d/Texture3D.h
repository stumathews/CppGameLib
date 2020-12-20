#pragma once
#include "util/COMUtils.h"
#include <iostream>

namespace gamelib
{	
	class Texture3D
	{
	private:
	protected:
	public:
		ID3D10Resource* textureResource;
		ID3D10Texture2D* textureInterface;
		D3DX10_IMAGE_INFO* fileInfo;
		D3D10_TEXTURE2D_DESC textureDescription;
		bool isLoaded;
		bool isOwned;
		Texture3D();
		virtual bool loadFromFile(std::wstring fileName);
		virtual bool free();
		void blitRectToBackBuffer(RECT tect, int m_xPos, int m_yPos);
	};
}

