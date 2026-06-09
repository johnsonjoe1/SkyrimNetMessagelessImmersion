#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_yps.h"
#include "misc.h"

namespace logger = SKSE::log;


// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping functions and message queuing function for thoughts are all
//  what this class can do.  
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

void handle_yps::handle_yps_fashion_detection_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_yps_fashion_detection_stuff function!!");
		return;
	}


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

