#include "Sprites3D.h"

#include "graphic/3d/RenderManager3D.h"


namespace gamelib
{
	Sprites3D::Sprites3D()
	{
		for(int i = 0; i < MAX_SPRITES; i++)
		{
			ZeroMemory(&spriteInfo[i], sizeof(D3DX10_SPRITE));
			Positions[i] = D3DXVECTOR2(0,0);
			Scales[i] = D3DXVECTOR2(0,0);
			spriteInfo[i].TexSize.x = 1.0f;
			spriteInfo[i].TexSize.y = 1.0f;
			spriteInfo[i].ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		}
		spriteInterface = NULL;
		numSprites = 0;
	}

	void Sprites3D::setUVCoords(int id, float left, float top, float width, float height)
	{
		spriteInfo[id].TexCoord.x = left;
		spriteInfo[id].TexCoord.y = top;
		spriteInfo[id].TexSize.x = width;
		spriteInfo[id].TexSize.y = height;
	}

	void Sprites3D::setColor(int id, float r, float g, float b, float a)
	{
		spriteInfo[id].ColorModulate = D3DXCOLOR(r,g,b,a);
	}

	void Sprites3D::setPos(int id, float x, float y)
	{
		Positions[id].x = x;
		Positions[id].y = y;
	}

	void Sprites3D::setScale(int id, float x, float y)
	{
		Scales[id].x = x;
		Scales[id].y = y;
	}

	bool Sprites3D::loadFromFile(int id, std::wstring filename)
	{
		if(!textures[id].loadFromFile(filename))
			return false;
		
		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = textures[id].textureDescription.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = textures[id].textureDescription.MipLevels;

		D3DRenderManager::GetInstance().d3dDevice->CreateShaderResourceView(textures[id].textureInterface, &SRVDesc, &spriteInfo[id].pTexture);
		if(!spriteInterface)
			D3DX10CreateSprite(D3DRenderManager::GetInstance().d3dDevice, 0, &spriteInterface);

		return true;
	}

	void Sprites3D::free()
	{
		SAFE_RELEASE(spriteInterface);
		for(int i = 0; i < MAX_SPRITES;i++) {
			SAFE_RELEASE(spriteInfo[i].pTexture);
		}
	}

	void Sprites3D::update()
	{
		if(numSprites <= 0)
			return;

		D3DXMATRIX projectionMatrix;
		D3DXMatrixOrthoOffCenterLH(&projectionMatrix, 
			D3DRenderManager::GetInstance().viewPort.TopLeftX,
			D3DRenderManager::GetInstance().viewPort.Width,
			D3DRenderManager::GetInstance().viewPort.TopLeftY,
			D3DRenderManager::GetInstance().viewPort.Height, 0.0f, 10);

		spriteInterface->SetProjectionTransform(&projectionMatrix);
		for(int i = 0; i < numSprites; i++){
			D3DXMATRIX transalationMatrix;
			D3DXMATRIX scalingMatrix;

			D3DXMatrixIdentity(&transalationMatrix);
			D3DXMatrixIdentity(&scalingMatrix);

			D3DXMatrixTranslation(&transalationMatrix, Positions[i].x, Positions[i].y, 0);
			D3DXMatrixScaling(&scalingMatrix, Scales[i].x, Scales[i].y,0);

			spriteInfo[i].matWorld = transalationMatrix * scalingMatrix;
			spriteInterface->Begin(D3DX10_SPRITE_SORT_TEXTURE);
			spriteInterface->DrawSpritesImmediate(spriteInfo, numSprites, 0, 0);
			spriteInterface->End();

		}
	}
}
