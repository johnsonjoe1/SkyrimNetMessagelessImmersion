#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "handle_iNeed.h"
#include "misc.h"

namespace logger = SKSE::log;



// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping functions and message queuing function for thoughts are all
//  what this class can do.  
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

void handle_iNeed::handle_iNeed_hunger_thirst_and_fatigue_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_iNeed_hunger_thirst_and_fatigue_stuff function!!");
		return;
	}


	// We have from another mod:
	//
	// GlobalVariable function GetINeedFatigue() global
	// return Game.GetFormFromFile(0x12DC, "iNeed.esp") as GlobalVariable
	// endFunction
	//
	// This should allow for direct native access to the same from C++:
	//
	// 
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
		} else if (fatigue == 2) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means MODERATE FATIGUE! ");
			if (fatigue > previous_iNeed_fatigue_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling moderate fatigue.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				SKSE::log::info("Note:  Fatigue-level-update thought 3 was delivered.");
			}				
		} else if (fatigue == 3) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means SEVERE FATIGUE! ");
			if (fatigue > previous_iNeed_fatigue_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling severe fatigue!  This is not just a little bit, but really severe fatigue that is impairing your abilities.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				SKSE::log::info("Note:  Fatigue-level-update thought 4 was delivered.");
			}				
		} else {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means some unknown level of fatigue! ");
		}
		// We only update fatigue, if the whole iNeed stuff worked.
		previous_iNeed_fatigue_level = fatigue;					
	}
	// We have from another mod:
	//
	//GlobalVariable function GetINeedThirst() global
	//    return Game.GetFormFromFile(0x4378, "iNeed.esp") as GlobalVariable
	//endFunction
	//
	// This should allow for direct native access to the same from C++:
	//		
	auto* thirstGV =
		RE::TESDataHandler::GetSingleton()
			->LookupForm<RE::TESGlobal>(0x4378, "iNeed.esp");
	if (thirstGV) {
		float thirst = thirstGV->value;
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: value={}", thirst);
		if (thirst == 0) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: i guess that means NO thirst AT ALL, NOTHING! ");
			if (thirst < previous_iNeed_thirst_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You completely rid of your thirst now!  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your thirst in your response!"));
				SKSE::log::info("Note:  Thirst-level-update thought 1 was delivered.");
			}							
		} else if (thirst == 1) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: i guess that means MILD thirst! ");
			if (thirst > previous_iNeed_thirst_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling mild thirst.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your thirst in your response!"));
				SKSE::log::info("Note:  Thirst-level-update thought 2 was delivered.");
			}
		} else if (thirst == 2) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: i guess that means MODERATE thirst! ");
			if (thirst > previous_iNeed_thirst_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling moderate thirst.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your thirst in your response!"));
				SKSE::log::info("Note:  Thirst-level-update thought 3 was delivered.");
			}				
		} else if (thirst == 3) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: i guess that means SEVERE thirst! ");
			if (thirst > previous_iNeed_thirst_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling severe thirst!  This is not just a little bit, but really severe thirst that is impairing your abilities.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your thirst in your response!"));
				SKSE::log::info("Note:  Thirst-level-update thought 4 was delivered.");
			}				
		} else {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed thirst GlobalVariable found: i guess that means some unknown level of thirst! ");
		}
		// We only update thirst, if the whole iNeed stuff worked.
		previous_iNeed_thirst_level = thirst;					
	}

	// We have from another mod:
	//
	// GlobalVariable function GetINeedHunger() global
	//     return Game.GetFormFromFile(0x12DB, "iNeed.esp") as GlobalVariable
	// endFunction
	//
	// This should allow for direct native access to the same from C++:
	//		
	auto* hungerGV =
		RE::TESDataHandler::GetSingleton()
			->LookupForm<RE::TESGlobal>(0x12DB, "iNeed.esp");
	if (hungerGV) {
		float hunger = hungerGV->value;
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: value={}", hunger);
		if (hunger == 0) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: i guess that means NO hunger AT ALL, NOTHING! ");
			if (hunger < previous_iNeed_hunger_level) {
				SKSE::log::info("Note:  Hunger-level-update thought 0 was delivered.");
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are completely rid of your hunger now!  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your hunger in your response!"));
			}							
		} else if (hunger == 1) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: i guess that means MILD hunger! ");
			if (hunger > previous_iNeed_hunger_level) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling mild hunger.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your hunger in your response!"));
				SKSE::log::info("Note:  Hunger-level-update thought 1 was delivered.");
			}
		} else if (hunger == 2) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: i guess that means MODERATE hunger! ");
			if (hunger > previous_iNeed_hunger_level) {
				SKSE::log::info("Note:  Hunger-level-update thought 2 was delivered.");
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling moderate hunger.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your hunger in your response!"));
			}				
		} else if (hunger == 3) {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: i guess that means SEVERE hunger! ");
			if (hunger > previous_iNeed_hunger_level) {
				SKSE::log::info("Note:  Hunger-level-update thought 3 was delivered.");
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling severe hunger!  This is not just a little bit, but really severe hunger that is impairing your abilities.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your hunger in your response!"));
			}				
		} else {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed hunger GlobalVariable found: i guess that means some unknown level of hunger! ");
		}
		// We only update hunger, if the whole iNeed stuff worked.
		previous_iNeed_hunger_level = hunger;					
	}


}

