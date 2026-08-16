#pragma once
#include <string>   

class handle_iNeed 
{
public:

	inline static float previous_iNeed_fatigue_level = 1000000;  // this will not trigger any getting-more-tired messages at game start
	inline static float previous_iNeed_thirst_level = 1000000;  // this will not trigger any getting-more-thirsty messages at game start
	inline static float previous_iNeed_hunger_level = 1000000;  // this will not trigger any getting-more-hungry messages at game start

	static void try_to_reset_iNeed_stuff_after_game_load_or_start();
	static void handle_iNeed_hunger_thirst_and_fatigue_stuff();

	static void handle_iNeed_but_only_fatigue_stuff();
	static void handle_iNeed_but_only_thirst_stuff();
	static void handle_iNeed_but_only_hunger_stuff();
	
};

