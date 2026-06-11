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

bool yps_high_heels_thought_flag_on = false;


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


}

