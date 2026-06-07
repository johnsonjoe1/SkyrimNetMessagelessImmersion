#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class handle_iNeed 
{
public:

	inline static float previous_iNeed_fatigue_level = 1000000;  // this will not trigger any getting-more-tired messages at game start
	inline static float previous_iNeed_thirst_level = 1000000;  // this will not trigger any getting-more-thirsty messages at game start
	inline static float previous_iNeed_hunger_level = 1000000;  // this will not trigger any getting-more-hungry messages at game start

	static void handle_iNeed_hunger_thirst_and_fatigue_stuff();
};

