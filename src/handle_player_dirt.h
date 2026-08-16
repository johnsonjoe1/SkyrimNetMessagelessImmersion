#pragma once
#include <string>   //  ChatGPT suggested this might be needed?????

class handle_player_dirt
{
public:
	static void handle_player_dirt_changes();
	static void try_to_reset_player_dirt_after_game_load_or_start();
};

