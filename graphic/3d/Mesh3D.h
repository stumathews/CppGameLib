#pragma once

#include "util/COMUtils.h"

namespace gamelib
{
	class Mesh3D
	{
	private:
	protected:
	public:
		ID3D10Effect* effect;
		ID3D10EffectTechnique* technique;
		ID3DX10Mesh* mesh;
		ID3D10InputLayout* vertexlayout;
		ID3D10EffectMatrixVariable* worldVariable;
		ID3D10EffectMatrixVariable* viewVariable;
		ID3D10EffectMatrixVariable* projectionVariable;

		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;

		Mesh3D();
		void create();
		void update();
		void free();
	};
}
