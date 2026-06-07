#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"

namespace logger = SKSE::log;

std::array<int, 23> AND_previous_faction_rank_sorted = {
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0
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

	int rank = player->GetFactionRank(nudeFaction, true);
	bool isNude = (rank == 1);
	logger::info("SUCCESSFULLY QUERIED AND Factions: nudeFaction={}, isNude={}", nudeFaction->GetFormID(), isNude);
	rank = player->GetFactionRank(topModestyFaction, true);
	bool isModest = (rank == 1);
	logger::info("SUCCESSFULLY QUERIED AND Factions: topModestyFaction={}, isModest={}", topModestyFaction->GetFormID(), isModest);




	std::string constructed_change_description = R"SKSE(From the change of clothing (or from moving too fast), you are now suddenly )SKSE";
	bool first_boolean_element = true;
	for (std::size_t my_i = 0; my_i < AND_faction_list_sorted.size(); ++my_i) {
		// logger::info("{} = {}", my_i, AND_faction_list_sorted[my_i]);

		auto* current_Faction =
		RE::TESForm::LookupByEditorID<RE::TESFaction>(AND_faction_list_sorted[my_i].c_str());
		if (!current_Faction) {
			logger::info("SEVERE ERROR: {} doesn't seem to exist!!", AND_faction_list_sorted[my_i]);
			continue;
		}
		rank = player->GetFactionRank(current_Faction, true);
		logger::info("SUCCESSFULLY QUERIED AND-Modesty-Factions: current_Faction={}, rank={}", AND_faction_list_sorted[my_i], rank);

		if (my_i <= (22-5)) {
			if (rank != AND_previous_faction_rank_sorted[my_i]) {
				logger::info("Player changed rank from previously {} to now {}!", AND_faction_list_sorted[my_i], rank);

				if (rank == 0) { // Not showing anything now but was showing in the past.
					constructed_change_description += " NO LONGER ";
				}

				if (!first_boolean_element) {
					constructed_change_description += " and ";
				}
				first_boolean_element = false;

				constructed_change_description += AND_faction_verbalalized_and_sorted[my_i]; // This is the verbalized version of the faction name, used for messages to the player.
				// std::string AND_rank_status_update_string = std::format("Player changed rank from previously {} to now {} in faction {}!", AND_previous_faction_rank_sorted[my_i], rank, AND_faction_list_sorted[my_i]);
				// RE::DebugMessageBox(AND_rank_status_update_string.c_str());	// This is so rare, it can afford to have a message box.
			}
		} else {
			if (rank != AND_previous_faction_rank_sorted[my_i]) {
				std::string AND_rank_status_update_string = std::format("NOTE:  SIGNIFICANT UNUSUAL CHANGE IN AND-Modesty-Factions: ONE OF THE DEEPER CATEGORIES HAS JUST CHANGES:  Player changed rank from previously {} to now {} in faction {}!", AND_previous_faction_rank_sorted[my_i], rank, AND_faction_list_sorted[my_i]);
				RE::DebugMessageBox(AND_rank_status_update_string.c_str());	// This is so rare, it can afford to have a message box.
			}
		}

		// We update the previous faction rank for each faction.
		AND_previous_faction_rank_sorted[my_i] = rank;
	}
	if (!first_boolean_element) {  // We actually have a change in one of the single body spots.
		constructed_change_description += ". Say so in your response to the player, to make him aware of your modesty situation, and tell us how that makes you feel.";
		RE::DebugMessageBox(constructed_change_description.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(constructed_change_description);
		SKSE::log::info("Note:  AND-MODESTY-UPDATE-STRING CONSTRUCTED: {}.", constructed_change_description);
	}

}

