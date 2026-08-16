#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_player_dirt.h"
#include "handle_config_ini_file.h"
#include "misc.h"
#include "papyrus_interface.h"
#include <unordered_set>
#include <optional>
#include <chrono>
#include <string>

namespace logger = SKSE::log;

float previous_dirt_value = 100000;  // some impossible value, so that no message occurs (unless dirt value 0, which wouldn't likely be the case in mid-game)

void handle_player_dirt::handle_player_dirt_changes()
{
	// We have from another mod:
	// PlayerDirt = Game.GetFormFromFile(0x000DA8, "Bathing in Skyrim.esp") as GlobalVariable
	// DirtString = " Dirt: " + ((Bis.GetPlayerDirt() * 100.0) as Int) + "%"
	//
	// So this should be a value between 0 and 1, and 1 meaning 100% dirty, and 0 meaning completely clean.
	// This should allow for direct native access to the same from C++:
	//	
	auto* playerDirt = RE::TESDataHandler::GetSingleton() ->LookupForm<RE::TESGlobal>(0x000DA8, "Bathing in Skyrim.esp");
	if (playerDirt) {
		float dirtValue = playerDirt->value;
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Player dirtiness: {} (previous value: {} )", dirtValue, previous_dirt_value);
		
		if ((previous_dirt_value < 0.6f) && (dirtValue >= 0.6f)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are getting really dirty now! This is not just a little bit, but really dirty so that you are not as charming when talking to people and maybe you also could get sick more easily.  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!"));
			SKSE::log::info("Note:  Dirtiness-level-update thought 1 was delivered.");
		} else if ((previous_dirt_value > 0.2f) && (dirtValue <= 0.2f)  && (dirtValue >= 0.19f)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("Finally some of the dirt on your body has washed off.  It's not as clean as washing with soap, but you are still more fresh and clean than before!  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!", static_cast<int>(dirtValue * 100)));
			SKSE::log::info("Note:  Update according to empirical values:  some swimming brings dirtiness down to 0.2 absolute it seems.");
		} else if ((previous_dirt_value > 0.01f) && (dirtValue <= 0.01f) ) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("Finally you are completely clean, like only a nice bath with soap can achieve!  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!", static_cast<int>(dirtValue * 100)));
			SKSE::log::info("Note:  Dirtiness-level-update thought 2 was delivered.");
		}
		previous_dirt_value = dirtValue;  // Update the previous dirt value for the
	} else {
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Player dirtiness global variable not found.");
	}
}


