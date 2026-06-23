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
bool actually_wearing_heels_according_to_yps_thoughts = false;


struct ParsedCondition
{
    std::string name;
    std::optional<int> value;
};
ParsedCondition ParseCondition(const std::string& str)
{
	size_t pos = str.size();
	while (pos > 0 &&  (std::isdigit(static_cast<unsigned char>(str[pos - 1])) || (str[pos - 1] == '-'))) {
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
	actually_wearing_heels_according_to_yps_thoughts = false;
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
			// SKSE::log::info("YPS-String-Parsing:  {}", current_substring);
			result.emplace(current_substring);

			// Let's handle some special cases or specially interesting variables
			auto c = ParseCondition(current_substring);

			//	SKSE::log::info("YPS-String-Parsing:  {}", str);

			std::string value_info;
			if (c.value) {
				value_info = std::format("Value = {}", *c.value);
				// logger::info("Value = {}", *c.value);
			} else { value_info = std::format("Value = missing / no value supplied"); }
			SKSE::log::info("YPS-String-Parsing: {} name: {} value: {}", current_substring, c.name, value_info);

			if (c.name == "ypsHeelsWorn") {
				actually_wearing_heels_according_to_yps_thoughts = true;
			}
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
	// triggered reasonably often.  Not clean, admittedly, but cleaning that up will have to come later.
	std::string current_yps_condition_string = SNMIPapyrus::GetYpsConditionString();
	logger::info("The lates YPS-condition-String was: {}" , current_yps_condition_string);
	SKSE::log::info("Current yps_condition_string: {}", current_yps_condition_string);  
	std::unordered_set<std::string> current_unorderd_yps_set = ParseConditions(current_yps_condition_string);
}



void handle_yps::handle_yps_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect)
{
	logger::info("*********************YPS-MAGIC-EFFECT-HANDLER STARTED!!!**********************************");
	// We got a magic effect.  We check once again everything, even though it has been checked in the calling function.

	auto* base = effect->GetBaseObject();
	auto* source = effect->spell;
	auto base_name = base->GetName();

	// For the protocol, we give some debug information.
	/* 	
	HeelTrainingStatusName[0] = "Untrained Feet"
	HeelTrainingStatusName[1] = "High Heel Novice"
	HeelTrainingStatusName[2] = "Flexible Feet"
	HeelTrainingStatusName[3] = "High Heel Walker"
	HeelTrainingStatusName[4] = "Arched Feet"
	HeelTrainingStatusName[5] = "Bondage Feet"  */

	SKSE::log::info("The MAGIC EFFECT Source->GetName() of the YPS-heels move penalty gave:  {} with magnitude:  {}", source->GetName(), effect->magnitude);

	// Let's try to track YPS-Fashion-High-Heels-slowdown-effect:  this gets activated and deactivated all the time, so we have to keep our messages in check
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "High Heel Novice") == 0)  ) )
	{
		if ( (a_event->isApplied) )
		{
			// Due to what seems like a bug to me, there will be slowdown effect even when not wearing heels.  In that case of course we don't complain about the heels.  So we have to catch that here.
			if ( (effect->magnitude < 0) && actually_wearing_heels_according_to_yps_thoughts) {
				SKSE::log::info("xxxxxxxxxxx High Heels Novice MAGIC EFFECT PROPERLY DETECTED");
				DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You already have some experience with them, but you are still a High Heels Novice, so they still slow you down a bit. It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			}
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		} else {  //  i.e. if (!a_event->isApplied) {
			// We do nothing here, as we just have stopped moving, nothing else.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of High Heels Novice MAGIC EFFECT.");
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Untrained Feet") == 0)  ) ) 
	{
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Untrained Feet' (high heels) MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS UNTRAINED FEET! ", base_name);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You are totally untrained with high heels. You are not even a High Heels Novice yet. So they slow you down massively now.  It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
		if ( (!a_event->isApplied) ) {
			// We do nothing here, as we just have stopped moving, nothing else.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Untrained Feet MAGIC EFFECT.");
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}

	logger::info("*********************YPS-MAGIC-EFFECT-HANDLER FINISHED!!!**********************************");
}
