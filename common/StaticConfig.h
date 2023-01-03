#pragma once
namespace gamelib
{
	class StaticConfig
	{
	public:	
		static constexpr int tick_time_ms = 50; // 20 times a second = 50 milliseconds, 1 second is 20*50 = 1000 milliseconds
		static constexpr int square_width = 30;
		static constexpr int sprite_width = 100;
		static constexpr int max_loops = 4;
		static constexpr int screen_width = 800;
		static constexpr int screen_height = 600;
		static constexpr int move_interval = 2;
		static constexpr auto frames_per_row = 3;
		static constexpr auto frames_per_column = 3;
		static constexpr auto player_init_pos_x = 100;
		static constexpr auto player_init_pos_y = 100;
		static constexpr bool verbose = false;
		static constexpr bool print_debugging_text = false;
		static constexpr bool use_3d_render_manager = false;
	};
}
