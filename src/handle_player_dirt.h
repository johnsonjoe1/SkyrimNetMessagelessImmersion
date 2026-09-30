#pragma once
#include <string> 

class handle_player_dirt
{
public:
	static void handle_player_dirt_changes();
	static void try_to_reset_player_dirt_after_game_load_or_start();
	static bool is_player_almost_at_filthy_dirt_stage();
	static bool is_player_very_dirty();
};

