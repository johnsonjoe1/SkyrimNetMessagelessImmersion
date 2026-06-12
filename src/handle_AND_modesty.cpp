#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "misc.h"
#include <string.h>

namespace logger = SKSE::log;

std::array<int, 23> AND_previous_faction_rank_sorted = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1
};

std::array<std::string, 23> AND_faction_list_sorted = {
"AND_ShowingAssFaction",       // 0
"AND_ShowingChestFaction",     // 1
"AND_ShowingGenitalsFaction",  // 2
"AND_NudeActorFaction",        // 3
"AND_ToplessFaction",          // 4
"AND_BottomlessFaction",       // 5
"AND_ShowingBraFaction",       // 6
"AND_ShowingUnderwearFaction", // 7
"AND_FlashingChestCurtain",    // 8
"AND_FlashingPelvicCurtain",   // 9
"AND_FlashingAssCurtain",      // 10
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


std::string RemoveAllOccurrences(std::string str, const std::string& toRemove)
{
    if (toRemove.empty()) {
        return str;
    }
    size_t pos = 0;
    while ((pos = str.find(toRemove, pos)) != std::string::npos) {
        str.erase(pos, toRemove.length());
    }
    return str;
}


// ****************************************************************************************************************
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

bool hard_change_in_slots_0_to_7()
{
	logger::info("handle_AND_modesty -- checking change in slots 0 to 7 of AND!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty--change_in_slots_0_to_7 function!!");
		return false;
	}

	// "AND_FlashingChestCurtain",    // 8
	// "AND_FlashingPelvicCurtain",   // 9
	// "AND_FlashingAssCurtain",      // 10
	auto* current_Faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[8].c_str());
	int flashing_chest = player->GetFactionRank(current_Faction, true);
	current_Faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[9].c_str());
	int flashing_pelvis = player->GetFactionRank(current_Faction, true);
	current_Faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[10].c_str());
	int flashing_ass = player->GetFactionRank(current_Faction, true);


	bool found_change = false;
	for (std::size_t my_i = 0; my_i <= 7; ++my_i) {  // The first 0-7 slots are REAL CLOTHING CHANGES!!!
		current_Faction = RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
		if (!current_Faction) {
			logger::info("SEVERE ERROR: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
			continue;
		}
		int rank = player->GetFactionRank(current_Faction, true);
		logger::info("SUCCESSFULLY QUERIED FOR HARD CLOTHING CHANGE-Factions: current_Faction={}, rank={} old_rank={} ", AND_faction_list_sorted[my_i], rank, AND_previous_faction_rank_sorted[my_i]);
		if ( (rank != AND_previous_faction_rank_sorted[my_i]) ) {
			logger::info("==>  THE ABOVE INDICATES A HARD CLOTHING CHANGE HAS HAPPEND!!!!");
/*
			"AND_ShowingAssFaction",       // 0
			"AND_ShowingChestFaction",     // 1
			"AND_ShowingGenitalsFaction",  // 2
			"AND_NudeActorFaction",        // 3
			"AND_ToplessFaction",          // 4
			"AND_BottomlessFaction",       // 5
			"AND_ShowingBraFaction",       // 6
			"AND_ShowingUnderwearFaction", // 7
*/
			bool bottom_flashing=false;
			if (flashing_ass | flashing_pelvis) {
				bottom_flashing=true;
			} 
			// bottom stuff will only count with no bottom-flashing active
			if ( (my_i == 0) | (my_i == 2) | (my_i == 7) )  { // bottom stuff
				if (!bottom_flashing) {
					found_change = true;
				}
			}
			if ( (my_i == 1) | (my_i == 4) | (my_i == 6) )  { // bottom stuff
				if (!flashing_chest) {
					found_change = true;
				}
			}
		}
	}
	logger::info("SUCCESSFULLY QUERIED FOR HARD CLOTHING CHANGE-Factions: >>>>>>>>>>>>>>>>>>>>>>>>>> FINAL RESULT IS READY:  found_change = {} ", found_change);
	return found_change;
}

void handle_hard_change_in_slots_0_to_7()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty--change_in_slots_0_to_7 function!!");
		return;
	}
	std::string constructed_change_description = R"SKSE(From the change of clothing, you are now suddenly )SKSE";
	bool first_boolean_element = true;
	for (std::size_t my_i = 0; my_i <= 7; ++my_i) {  // The first 0-7 slots are REAL CLOTHING CHANGES!!!
		// logger::info("{} = {}", my_i, AND_faction_list_sorted[my_i]);

		auto* current_Faction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
		if (!current_Faction) {
			logger::info("SEVERE ERROR: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
			continue;
		}
		int rank = player->GetFactionRank(current_Faction, true);
		logger::info("NOW HANDLING HARD CLOTHING CHANGE-Factions: current_Faction={}, rank={}", AND_faction_list_sorted[my_i], rank);

		if ( (rank != AND_previous_faction_rank_sorted[my_i])  ) {
			logger::info("Player changed rank from previously {} to now {}!", AND_previous_faction_rank_sorted[my_i], rank);



			if (!first_boolean_element) {
				constructed_change_description += " and you are ";
			}
			first_boolean_element = false;
			if ((rank == 0) ){ // Not showing anything now but was showing in the past.  but in case of quick flashing, the no-longer isn't needed either. 
				constructed_change_description += " NO LONGER ";
			}
			constructed_change_description += AND_faction_verbalalized_and_sorted[my_i]; // This is the verbalized version of the faction name, used for messages to the player.
		} 
		// We update the previous faction rank for each faction, so we don't repeatedly announce the same change of clothes.
		AND_previous_faction_rank_sorted[my_i] = rank;
	}
	constructed_change_description += ". Say so in your response to the player, to make him aware of your modesty situation, and tell us how that makes you feel.";
	LillithOnlyBox(constructed_change_description.c_str());
	if (DumpThoughts::seconds_since_game_load() >= 00.0f) {
		// Actually getting nakes is important enough to force-push the message
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(constructed_change_description);
	} else {
		logger::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED, but not delivering it to TTS because it's too soon after game load: {}.", constructed_change_description);
	}
}

void handle_AND_modesty::initialize_AND_status()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty_and_nakedness_stuff function!!");
		return;
	}
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
}

void handle_current_flashing_state()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty_and_nakedness_stuff function!!");
		return;
	}

	std::string constructed_change_description;
	
	bool first_boolean_element = true;
	for (std::size_t my_i = 8; my_i < 22-5; ++my_i) {
		auto* current_Faction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
		if (!current_Faction) {
			logger::info("SEVERE ERROR: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
			continue;
		}
		int rank = player->GetFactionRank(current_Faction, true);
		logger::info("SUCCESSFULLY QUERIED FOR SOFT-AND-FLASHING-Factions: current_Faction={}, rank={}", AND_faction_list_sorted[my_i], rank);
		bool real_clothes_change = false;

		if (rank) {
			logger::info("Player didn't change rank, but is currently flashing something! {} is flashing because rank={} !", AND_faction_list_sorted[my_i], rank);
			if (!first_boolean_element) {
				constructed_change_description += " and you are ";
			}
			first_boolean_element = false;
			constructed_change_description += AND_faction_verbalalized_and_sorted[my_i]; // This is the verbalized version of the faction name, used for messages to the player.
		}
		// We update the previous faction rank for each faction.
		AND_previous_faction_rank_sorted[my_i] = rank;
	}
	constructed_change_description = R"SKSE(Due to wind and movement, you are currently )SKSE" + constructed_change_description;			
	constructed_change_description += ". Say so in your response to the player, to make him aware of your modesty situation, and tell us how that makes you feel.";
	LillithOnlyBox(constructed_change_description.c_str());
	if (DumpThoughts::seconds_since_game_load() >= 20.0f) {
		SKSE::log::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED: {}.", constructed_change_description);
		// Flashing your private parts is not so important as to always push the message.  It's something that belongs into the background channel.
		DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(constructed_change_description);
		// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(constructed_change_description);
	} else {
		logger::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED, but not delivering it to TTS because it's too soon after game load: {}.", constructed_change_description);
	}
}
void handle_AND_modesty::handle_AND_modesty_and_nakedness_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_AND_modesty_and_nakedness_stuff function!!");
		return;
	}
	// Make sure the game is initialized and we don't get confused at game startup.
	if (AND_previous_faction_rank_sorted[0] == -1) {
		logger::info("AND-Ranks are not initialized yet!!  Initialize them and then return for now and for this round!!");
		initialize_AND_status();
		return;
	}

	// FIRST we check for actual clothing changes (ie. on slots 0-7).  If something happens there, that is the message and the rest is irrelevant anyway.
	if (hard_change_in_slots_0_to_7()) {
		handle_hard_change_in_slots_0_to_7();
		return;
	}

	// Now we know, there is no pure change of clothing, but rather the constant ON-OFF of the flashing system
	if (false) {
		//  disable that for now, just to test.  
		handle_current_flashing_state();
	}

}

