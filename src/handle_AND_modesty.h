#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????
#include "handle_worn_equipment_change.h"

std::string get_currently_worn_transparent_top_item_names();

class handle_AND_modesty 
{
public:
	static void handle_AND_modesty_and_nakedness_stuff();
	static void reset_previous_rank_to_current_rank();
};
