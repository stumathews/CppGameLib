#pragma once
#include "game_object.h"

namespace gamelib
{
	class sprite final : public game_object
	{
	private:
		unsigned long m_TimeLastFrame;
	public:
		sprite(uint xPos, uint yPos, uint speed, uint totalFrames, uint framesPerRow, uint framesPerColumn, uint frameWidth, uint frame, bool is_visible, std::shared_ptr<settings_manager> settings_admin) : 
			m_Speed(100), m_TotalFrames(totalFrames), m_FramesPerRow(framesPerRow), m_FramesPerColumn(framesPerColumn),
			m_CurrentFrame(0), m_FrameHeight(64), m_FrameWidth(64), m_StartFrame(0), m_TimeLastFrame(0), game_object(xPos, yPos, is_visible, settings_admin ) { }
		object_type get_type() override { return object_type::sprite; }
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
			update();	// why do we have to do this in the draw function?	
		}
			
		void update() override;
		void play();
		void stop();
		void setFrameRect(uint FrameNumber);
		virtual ~sprite(){};
		bool stopped = false;
	};
}
