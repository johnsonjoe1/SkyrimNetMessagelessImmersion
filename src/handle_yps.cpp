#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_yps.h"
#include "misc.h"
#include "papyrus_interface.h"
#include <unordered_set>
#include <optional>

namespace logger = SKSE::log;

struct ParsedCondition
{
    std::string name;
    std::optional<int> value;
};


ParsedCondition ParseCondition(const std::string& str)
{
	size_t pos = str.size();

	while (pos > 0 && std::isdigit(static_cast<unsigned char>(str[pos - 1]))) {
		--pos;
	}

	ParsedCondition result;
	result.name = str.substr(0, pos);

	if (pos < str.size()) {
		result.value = std::stoi(str.substr(pos));
	}

	return result;
}


std::unordered_set<std::string> ParseConditions(const std::string& str)
{
	std::unordered_set<std::string> result;
	size_t start = 0;
	while (start < str.size()) {
		size_t end = str.find('|', start);
		if (end == std::string::npos) {
			end = str.size();
		}
		if (end > start) {

			std::string current_substring;
			current_substring = str.substr(start, end - start);
			SKSE::log::info("YPS-String-Parsing:  Current substring: {}", current_substring);
			result.emplace(current_substring);

			// Let's handle some special cases or specially interesting variables
			auto c = ParseCondition(current_substring);

			logger::info("Name = {}", c.name);

			if (c.value) {
				logger::info("Value = {}", *c.value);
			} else { logger::info("Value = missing / no value supplied"); }


		}
		start = end + 1;
	}
	return result;
}


// ****************************************************************************************************************
//  We handle changes of the yps-Status here.
//
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

void handle_yps::handle_yps_fashion_detection_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_yps_fashion_detection_stuff function!!");
		return;
	}



	// Now, PERIODICALLY, we take care of changes in the values of other mods.  We put this here, because this gets
	// triggered reasonably often.  
	std::string current_yps_condition_string = SNMIPapyrus::GetYpsConditionString();
	logger::info("The lates YPS-condition-String was: {}" , current_yps_condition_string);
	SKSE::log::info("Current yps_condition_string: {}", current_yps_condition_string);  
	std::unordered_set<std::string> current_unorderd_yps_set = ParseConditions(current_yps_condition_string);



	/*
	auto* storage = StorageUtilAPI::GetSingleton();

	int addictionLevel =
		storage->GetIntValue(
			nullptr,
			"yps_AddictionLevel",
			0);

	int addictionBuff =
		storage->GetIntValue(
			nullptr,
			"yps_AddictionBuff",
			0);
			
	*/

	// We have from another mod:
	//
	// GlobalVariable function GetINeedFatigue() global
	// return Game.GetFormFromFile(0x12DC, "iNeed.esp") as GlobalVariable
	// endFunction
	//
	// This should allow for direct native access to the same from C++:
	//
	// 
	/* 
	auto* fatigueGV =
		RE::TESDataHandler::GetSingleton()
			->LookupForm<RE::TESGlobal>(0x12DC, "iNeed.esp");
	if (fatigueGV) {
		float fatigue = fatigueGV->value;
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: value={}", fatigue);
		if (fatigue == 0) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means NO FATIGUE AT ALL, NOTHING! ");
			if ( (fatigue < previous_iNeed_fatigue_level) & (previous_iNeed_fatigue_level != 1000000 ) ) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are full rested from sleep and you are completely rid of your fatigue now!  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				SKSE::log::info("Note:  Fatigue-level-update thought 1 was delivered.");
			}							
		} else if (fatigue == 1) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means MILD FATIGUE! ");
			if (fatigue > previous_iNeed_fatigue_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling mild fatigue.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				SKSE::log::info("Note:  Fatigue-level-update thought 2 was delivered.");
			}
		} else {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means some unknown level of fatigue! ");
		}
		// We only update fatigue, if the whole iNeed stuff worked.
		previous_iNeed_fatigue_level = fatigue;					
	}
	*/
}

