#include "Sprite.h"
#include "SDL.h"
#include "windows.h"

namespace gamelib
{
	void Sprite::update()
	{
		const unsigned long time_since_last_frame = timeGetTime() - m_TimeLastFrame;
		if(time_since_last_frame >= m_Speed) 
		{
			m_CurrentFrame++;
			if(m_CurrentFrame >= m_TotalFrames) 
				m_CurrentFrame = m_StartFrame;

			set_frame_rect(m_CurrentFrame);

			m_TimeLastFrame = timeGetTime();
		
		}
	}

	void Sprite::play()
	{
		const auto resource = get_graphic_asset();
		if(!is_resource_loaded())
			return;
		//auto TmpSurface = resource->m_Surface;
		//m_FrameWidth =  TmpSurface->w/m_FramesPerRow;
		//m_FrameHeight = TmpSurface->h/m_FramesPerColumn;
		m_CurrentFrame = m_StartFrame;
		set_frame_rect(m_CurrentFrame);
		m_TimeLastFrame = timeGetTime();
	}

	void Sprite::stop()
	{
		stopped = true;
	}

	void Sprite::set_frame_rect(uint FrameNumber) const
	{
		if(!is_resource_loaded())
			return;
		/*uint RowNumber = floor(FrameNumber/m_FramesPerRow);
		uint ColumnNumber = FrameNumber;

		if(RowNumber > 0)
			ColumnNumber = FrameNumber - (RowNumber * m_FramesPerRow);*/
		
		SDL_Rect typicalFrame[] = { 
			{0,0,66,92},
			{66,0,66,93},
			{132,0,66,93},
			{0,93,66,93},
			{66,93,66,93},
			{132,93,71,92},
			{0,186,71,93},
			{71,186,71,93},
			{142,186,70,92},
			{0,279,71,93},
			{71,279,66,97},
		};

		/*GetResource()->m_viewPort.x = ColumnNumber * m_FrameWidth;
		GetResource()->m_viewPort.y = RowNumber * m_FrameHeight;
		GetResource()->m_viewPort.w = m_FrameWidth;
		GetResource()->m_viewPort.h = m_FrameHeight;*/

		get_graphic_asset()->view_port.x = typicalFrame[FrameNumber].x;
		get_graphic_asset()->view_port.y = typicalFrame[FrameNumber].y;
		get_graphic_asset()->view_port.w = typicalFrame[FrameNumber].w;
		get_graphic_asset()->view_port.h = typicalFrame[FrameNumber].h;
	}
}