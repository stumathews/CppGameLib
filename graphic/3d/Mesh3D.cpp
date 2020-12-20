#include "Mesh3D.h"

#include <d3dx10async.h>

#include "RenderManager3D.h"

namespace gamelib
{
	Mesh3D::Mesh3D()
	{
		effect = NULL;
		technique = NULL;
		mesh = NULL;
		vertexlayout = NULL;
		worldVariable = NULL;
		viewVariable = NULL;
		projectionVariable = NULL;

		D3DXMatrixIdentity(&world);
		D3DXMatrixIdentity(&view);
		D3DXMatrixIdentity(&projection);
	}

	void Mesh3D::create()
	{
		D3DX10CreateEffectFromFile(L"MyShader.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, D3DRenderManager::GetInstance().d3dDevice, NULL, NULL, &effect, NULL, NULL);
		
		// Fetch technique from Shader file
		technique = effect->GetTechniqueByName("Render");

		// Define what the vertex structure will hold
		D3D10_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
		};

		// Number of parts/elements to a vertex layout
		UINT numElements = sizeof(layout) / sizeof(layout[0]);

		// Create the above InputLayout (vertex format) in the device
		D3D10_PASS_DESC passDescription;
		technique->GetPassByIndex(0)->GetDesc(&passDescription);		
		D3DRenderManager::GetInstance().d3dDevice->CreateInputLayout(layout, numElements, passDescription.pIAInputSignature, passDescription.IAInputSignatureSize, &vertexlayout);

		// Register/Set the input layout (vertex format) with the device
		D3DRenderManager::GetInstance().d3dDevice->IASetInputLayout(vertexlayout);
			
		// These will be the vertices, setup in the format as described above in the input layout
		 Vertex vertices1[] =
			{
			      // POSITION						 // COLOUR
				{ D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ) },
				{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 1.0f ) },
				{ D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 1.0f ) },
				{ D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f ) },
				{ D3DXVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 1.0f ) },
				{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 1.0f ) },
				{ D3DXVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) },
				{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f ) },
			};

		 auto green = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f );
		 auto blue = D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f );
		 auto red = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f );
		  Vertex vertices[] =
			{
			      // POSITION						 // COLOUR
				{ D3DXVECTOR3( -1.0f, 1.0f, -1.0f ), red },
				{ D3DXVECTOR3( 1.0f, 1.0f, -1.0f ), blue },
				{ D3DXVECTOR3( 1.0f, 1.0f, 1.0f ), green },
				{ D3DXVECTOR3( -1.0f, 1.0f, 1.0f ), red },
				{ D3DXVECTOR3( -1.0f, -1.0f, -1.0f ),blue },
				{ D3DXVECTOR3( 1.0f, -1.0f, -1.0f ), green},
				{ D3DXVECTOR3( 1.0f, -1.0f, 1.0f ),red},
				{ D3DXVECTOR3( -1.0f, -1.0f, 1.0f ),green },
			};

		UINT numVertices = sizeof(vertices)/sizeof(vertices[0]);
		DWORD indices[] = 
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,

		};

		UINT numIndices = sizeof(indices)/sizeof(indices[0]);
		
		// Create an empty mesh frome the vertices;
		auto meshCreated = D3DX10CreateMesh(D3DRenderManager::GetInstance().d3dDevice, layout, numElements, "POSITION", numVertices, numIndices/3, D3DX10_MESH_32_BIT, &mesh);
		if(SUCCEEDED(meshCreated))
		{
			// Populate the mesh with the verticies
			mesh->SetVertexData(0,vertices);
			
			// Populate the mesh with the indicies data
			mesh->SetIndexData(indices, numIndices);
			
			// Complete building the mesh object
			mesh->CommitToDevice();
		}
	}

	void Mesh3D::update()
	{
		worldVariable = effect->GetVariableByName("World")->AsMatrix();
		viewVariable = effect->GetVariableByName("View")->AsMatrix();
		projectionVariable = effect->GetVariableByName("Projection")->AsMatrix();

		D3DXVECTOR3 eye(0.0f, 1.0f, -5.0f);
		D3DXVECTOR3 at(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMatrixLookAtLH(&view, &eye, &at, &up);

		D3DXMatrixPerspectiveFovLH(&projection, (float) D3DX_PI * 0.5f, D3DRenderManager::GetInstance().viewPort.Width / (float) D3DRenderManager::GetInstance().viewPort.Height, 0.1f, 100.0f);

		worldVariable->SetMatrix(world);
		viewVariable->SetMatrix(view);
		projectionVariable->SetMatrix(projection);

		D3D10_TECHNIQUE_DESC techniqueDescrition;
		technique->GetDesc(&techniqueDescrition);
		for(UINT p = 0; p < techniqueDescrition.Passes; ++p)
		{
			technique->GetPassByIndex(p)->Apply(0);
			mesh->DrawSubset(0);
		}


	}

	void Mesh3D::free()
	{
	}
}