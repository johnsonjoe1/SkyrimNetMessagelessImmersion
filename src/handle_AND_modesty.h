#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????
#include <vector>

#include "RE/Skyrim.h"

struct CurrentlyWornItemRecord
{
	RE::FormID form_id{};
	RE::TESObjectARMO* armor{nullptr};
	std::uint32_t slot_mask{0};
	RE::TESBoundObject* item{nullptr};
	std::vector<RE::BGSKeyword*> keywords;
};

extern std::vector<CurrentlyWornItemRecord> currently_worn_item_records;
extern std::vector<CurrentlyWornItemRecord> historic_worn_item_records;

void refresh_currently_worn_item_records();

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class handle_AND_modesty 
{
public:
	static void handle_AND_modesty_and_nakedness_stuff();
	static void reset_previous_rank_to_current_rank();
};

