#pragma once
#include "game_object.h"

namespace gamelib
{
	class sprite final : public game_object
	{
	private:
		unsigned long m_TimeLastFrame;
	public:
		sprite(uint xPos, uint yPos, uint speed = 0, uint totalFrames = 1, uint framesPerRow = 1, uint framesPerColumn = 1, uint frameWidth = 0, uint frame = 0) : 
			m_Speed(100), m_TotalFrames(totalFrames), m_FramesPerRow(framesPerRow), m_FramesPerColumn(framesPerColumn),
			m_CurrentFrame(0), m_FrameHeight(64), m_FrameWidth(64), m_StartFrame(0), m_TimeLastFrame(0), game_object(xPos, yPos) { }
		
		uint m_TotalFrames;
		uint m_FramesPerRow;
		uint m_FramesPerColumn;
		uint m_CurrentFrame;
		uint m_StartFrame;
		float m_Speed;
		uint m_FrameWidth;
		uint m_FrameHeight;
		

		void draw(SDL_Renderer* renderer) override
		{
			game_object::draw(renderer);
			update();		
		}
			
		void update() override;
		void play();
		void stop();
		void setFrameRect(uint FrameNumber);
		virtual ~sprite(){};
		bool stopped = false;
	};
}
