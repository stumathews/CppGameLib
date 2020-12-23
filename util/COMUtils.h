#ifndef COMUTILS_H
#define COMUTILS_H

namespace gamelib
{
	#define SAFE_RELEASE(m_xPos) \
		if(m_xPos != NULL)		\
		{					\
			m_xPos->Release();	\
			m_xPos = NULL;		\
		}					\


	#define MAX_SPRITES 4096

#include <d3dx10.h>

	struct Vertex 
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR4 Color;
	};
}

#endif
