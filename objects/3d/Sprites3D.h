#pragma once
#include "util/COMUtils.h"
#include "graphic/3d/Texture3D.h"
namespace gamelib
{
	class Sprites3D
	{
	private:
	protected:
		D3DX10_SPRITE spriteInfo[MAX_SPRITES];
		D3DXVECTOR2 Positions[MAX_SPRITES];
		D3DXVECTOR2 Scales[MAX_SPRITES];
		Texture3D textures[MAX_SPRITES];
		ID3DX10Sprite* spriteInterface;
	public:
		unsigned int numSprites;
		Sprites3D();
		void setUVCoords(int id, float left, float top, float width, float height);
		void setColor(int id, float r, float g, float b, float a=1.0f);
		void setPos(int id, float m_xPos, float m_yPos);
		void setScale(int id, float m_xPos, float m_yPos);
		bool loadFromFile(int id, const std::wstring &filename);
		void free();
		void update();
	};
}
