#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "misc.h"

namespace logger = SKSE::log;

std::array<int, 23> AND_previous_faction_rank_sorted = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1
};

std::array<std::string, 23> AND_faction_list_sorted = {
"AND_ShowingAssFaction", 
"AND_ShowingChestFaction",
"AND_ShowingGenitalsFaction",
"AND_NudeActorFaction",
"AND_ToplessFaction",
"AND_BottomlessFaction",
"AND_ShowingBraFaction", 
"AND_ShowingUnderwearFaction", 
"AND_FlashingChestCurtain", 
"AND_FlashingPelvicCurtain", 
"AND_FlashingAssCurtain", 
"AND_FlashingTop", 
"AND_FlashingBra", 
"AND_FlashingBottom", 
"AND_FlashingUnderwear", 
"AND_FlashingHotpants", 
"AND_FlashingSkirt", 
"AND_FlashingCString", 
"AND_ShyWithMales", 
"AND_ShyWithFemales", 
"AND_TopModestyFaction", 
"AND_BottomModestyFaction", 
"AND_ModestyFaction"
};

std::array<std::string, 23> AND_faction_verbalalized_and_sorted = {
"showing your ass", 
"showing your chest",
"showing your genitals",
"completely nude",
"completely topless",
"completely bottomless",
"showing your bra",
"showing your underwear",
"flashing your chest",
"flashing your pelvic",
"flashing your ass",
"flashing your top",
"flashing your bra",
"flashing your bottom",
"flashing your underwear",
"flashing your hotpants", 
"flashing your skirt", 
"flashing your C-string", 
"shy with males",
"shy with females",
"top modesty faction", 
"bottom modesty faction", 
"modesty faction"
};

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping functions and message queuing function for thoughts are all
//  what this class can do.  
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

void handle_AND_modesty::handle_AND_modesty_and_nakedness_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty_and_nakedness_stuff function!!");
		return;
	}
	auto* nudeFaction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(
			"AND_NudeActorFaction");
	if (!nudeFaction) {
		logger::info("SEVERE ERROR: AND_NudeActorFaction doesn't seem to exist!!");
		return;
	}				
	auto* topModestyFaction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(
			"AND_TopModestyFaction");
	if (!topModestyFaction) {
		logger::info("SEVERE ERROR: AND_TopModestyFaction doesn't seem to exist!!");
		return;
	}


	if (AND_previous_faction_rank_sorted[0] == -1) {
		logger::info("AND-Ranks are not initialized yet!!  Initialize them and then return for now and for this round!!");
		for (std::size_t my_i = 0; my_i < AND_faction_list_sorted.size(); ++my_i) {
			auto* current_Faction =
			RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
			if (!current_Faction) {
				logger::info("SEVERE ERROR IN INITIALIZATION RUN: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
				continue;
			}
			int rank = player->GetFactionRank(current_Faction, true);
			AND_previous_faction_rank_sorted[my_i] = rank;
			logger::info("SUCCESSFULLY QUERIED AND-Modesty-Factions IN INITIALIZATION RUN: current_Faction={}, rank={}", AND_faction_list_sorted[my_i], rank);
		}
		return;
	}


	std::string constructed_change_description;
	
	bool first_boolean_element = true;
	for (std::size_t my_i = 0; my_i < AND_faction_list_sorted.size(); ++my_i) {
		// logger::info("{} = {}", my_i, AND_faction_list_sorted[my_i]);

		auto* current_Faction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
		if (!current_Faction) {
			logger::info("SEVERE ERROR: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
			continue;
		}
		int rank = player->GetFactionRank(current_Faction, true);
		logger::info("SUCCESSFULLY QUERIED AND-Modesty-Factions: current_Faction={}, rank={}", AND_faction_list_sorted[my_i], rank);
		bool real_clothes_change = false;
		if (my_i <= (22-5)) {  // NOTE: Positions 0-7 are REAL CLOTHES CHANGES.  The rest is flashing of body parts.
			if (rank != AND_previous_faction_rank_sorted[my_i]) {
				logger::info("Player changed rank from previously {} to now {}!", AND_faction_list_sorted[my_i], rank);
				if (!first_boolean_element) {
					constructed_change_description += " and ";
				}
				first_boolean_element = false;
				if ((rank == 0) && (my_i <= 7) ){ // Not showing anything now but was showing in the past.  but in case of quick flashing, the no-longer isn't needed either. 
					constructed_change_description += " NO LONGER ";
				}
				constructed_change_description += AND_faction_verbalalized_and_sorted[my_i]; // This is the verbalized version of the faction name, used for messages to the player.
			}
		} else {
			if (rank != AND_previous_faction_rank_sorted[my_i]) {
				std::string AND_rank_status_update_string = std::format("NOTE:  SIGNIFICANT UNUSUAL CHANGE IN AND-Modesty-Factions: ONE OF THE DEEPER CATEGORIES HAS JUST CHANGES:  Player changed rank from previously {} to now {} in faction {}!", AND_previous_faction_rank_sorted[my_i], rank, AND_faction_list_sorted[my_i]);
				LillithOnlyBox(AND_rank_status_update_string.c_str());	
			}
		}

		// We update the previous faction rank for each faction.
		AND_previous_faction_rank_sorted[my_i] = rank;
	}
	if (!first_boolean_element) {  // We actually have a change in one of the single body spots.

		// Now there can be 2 cases:  There was an actual change of wardrobe (0-7) or there was some kind of flashing (8-17).
		// In case of flashing, the word flashing is always there.  If is wasnt there, then we announce the clothing change.
		bool this_was_just_flashing = false;
		if (constructed_change_description.find("flashing") != std::string::npos) {
			logger::info("FLASHING-KEYWORD FOUND IN OUR AND-RANK-CHANGE-STRING!!!!  THIS MEANS WE HAVE JUST FLASHING AND SAY SO VIA THOUGHTS!!!");
			constructed_change_description = R"SKSE(From moving so fast, you accidentially flashed your feminine parts to onlookers.  You cannot be sure that anybody saw, but you were clearly )SKSE" + constructed_change_description;	
		} else {
			logger::info("FLASHING-KEYWORD *NOT* FOUND IN OUR AND-RANK-CHANGE-STRING!!!!  THIS MEANS WE HAVE REAL WARDROBE CHANGE!!");
			constructed_change_description = R"SKSE(From the change of clothing, you are now suddenly )SKSE" + constructed_change_description;			
		}

		constructed_change_description += ". Say so in your response to the player, to make him aware of your modesty situation, and tell us how that makes you feel.";
		LillithOnlyBox(constructed_change_description.c_str());

		if (DumpThoughts::seconds_since_game_load() >= 20.0f) {
			SKSE::log::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED: {}.", constructed_change_description);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(constructed_change_description);
		} else {
			logger::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED, but not delivering it to TTS because it's too soon after game load: {}.", constructed_change_description);
		}
	} else {


	}

}

