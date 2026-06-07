#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include "DumpThoughts.h"

namespace logger = SKSE::log;

static auto last_speech_timestamp = std::chrono::steady_clock::now();
static auto last_game_load_or_reload_timestamp = std::chrono::steady_clock::now();
static auto last_lactacid_added_speech_timestamp = std::chrono::steady_clock::now();

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping functions and message queuing function for thoughts are all
//  what this class can do.  
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

std::chrono::steady_clock::time_point DumpThoughts::GetLastSpeechTimestamp() {
	return last_speech_timestamp;
}
void DumpThoughts::reset_lactacid_added_speech_timestamp() {
	last_lactacid_added_speech_timestamp = std::chrono::steady_clock::now();
}
void DumpThoughts::reset_last_speech_timestamp() {
	last_speech_timestamp = std::chrono::steady_clock::now();
}
void DumpThoughts::reset_last_game_load_or_reload_timestamp() {
	// For DEBUGGING and TESTING we can't have the wait time after a reload.  So based on Player-name this is again skipped.
	
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		//RE::DebugMessageBox("SNMI:  Player 'LILLITH' is detected, so skipping the silence after game (re)load.");
		SKSE::log::info("SNMI:  Player 'LILLITH' is detected, so skipping the silence after game (re)load.");
		return;
	} 

	// We reset the game-reloaded-timestamp for silence and thus avoiding messages from reload-induced changes
	last_game_load_or_reload_timestamp = std::chrono::steady_clock::now();
}
bool DumpThoughts::too_early_after_game_load()
{
	auto now = std::chrono::steady_clock::now();
	auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_game_load_or_reload_timestamp);
	SKSE::log::info("Time since last game load or reload: {} seconds", runtime.count());
	const int minimum_time_since_last_game_load_or_reload = 60;  // in seconds, so 1 minute
	if (runtime.count() < minimum_time_since_last_game_load_or_reload) {
		SKSE::log::info("////////BLOCKING THOUGHT OUTPUT/////////It is too early after game load or reload to throw out thoughts, because only {} seconds have passed since the last game load or reload, which is less than the minimum of {} seconds.", runtime.count(), minimum_time_since_last_game_load_or_reload);
		return true;
	} else {
		return false;
	}
}
float DumpThoughts::seconds_since_game_load() {
	auto now = std::chrono::steady_clock::now();
	auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_game_load_or_reload_timestamp);
	return runtime.count();
}
bool DumpThoughts::too_early_for_next_lactacid_speech()
{
	auto now = std::chrono::steady_clock::now();
	auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_lactacid_added_speech_timestamp);
	SKSE::log::info("Time since last lactacid added speech: {} seconds", runtime.count());
	const int minimum_time_since_last_lactacid_added_speech = 60;  // in 60 seconds 
	if (runtime.count() < minimum_time_since_last_lactacid_added_speech) {
		SKSE::log::info("It is too early for the next lactacid added speech, because only {} seconds have passed since the last lactacid added speech, which is less than the minimum of {} seconds.", runtime.count(), minimum_time_since_last_lactacid_added_speech);
		return true;
	} else {
		return false;
	}
}

void DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::string my_message) {
	// The background channel shouldn't be flooded with text all the time.  Give the real user a chance to relax.  So only bring background stuff, when nothing else is going on.
	auto now = std::chrono::steady_clock::now();
	auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_speech_timestamp);
	SKSE::log::info("Time since last thought-speech on BACKGROUNDCHANNEL OR PRIORITIES CHANNELS: {} seconds", runtime.count());
	// RE::DebugMessageBox(("Time passed since the last speech: " + std::to_string(runtime.count()) + " seconds").c_str());
	const int minimum_time_since_last_speech = 90;  // in seconds, so 1.5 minutes
	if (runtime.count() < minimum_time_since_last_speech) {
		SKSE::log::info("Not throwing out the BACKGROUNDCHANNEL text as a TTS thought, because only {} seconds have passed since the last speech, which is less than the minimum of {} seconds.", runtime.count(), minimum_time_since_last_speech);
		// return RE::BSEventNotifyControl::kContinue;
	} else {
		//RE::DebugMessageBox(my_message.c_str());
		SKSE::log::info("The thought for the BACKGROUND TTS channel is: {} ", my_message.c_str());
		// We want to broadcast mod events.  So we need this event source.
		std::string  mod_event_name = "SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought";
		std::string  mod_event_string_arg = my_message; //  + standard_thought_instruction;
		auto eventSource = SKSE::GetModCallbackEventSource();

		if (DumpThoughts::too_early_after_game_load()) {
			SKSE::log::info("////////BLOCKING BACKGROUO	THOUGHT OUTPUT BECAUSE TOO EARLY AFTER RELOAD/////////");
			return;
		}
		SKSE::ModCallbackEvent my_event(
			mod_event_name,                        // event name
			mod_event_string_arg,                  // arbitrary string argument 
			123.0f,                                // arbitrary float argument
			RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
		);
		eventSource->SendEvent(&my_event);
		last_speech_timestamp=std::chrono::steady_clock::now();  // only reset the timer if real speech has been produced
	}
}	
void DumpThoughts::throw_out_TTS_thought_message(std::string my_message) {
	// RE::DebugMessageBox(my_message.c_str());
	SKSE::log::info("The thought for the NORMAL THOUGHT channel is: {} ", my_message.c_str());
	// We want to broadcast mod events.  So we need this event source.
	std::string  mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";  //  was, but was probably wrong:   SNMI_PlayerActivatedSomething";
	std::string  mod_event_string_arg = my_message; //  + standard_thought_instruction;
	auto eventSource = SKSE::GetModCallbackEventSource();

	if (DumpThoughts::too_early_after_game_load()) {
		SKSE::log::info("////////BLOCKING normal thought message THOUGHT OUTPUT BECAUSE TOO EARLY AFTER RELOAD/////////");
		return;
	}
	SKSE::ModCallbackEvent my_event(
		mod_event_name,                        // event name
		mod_event_string_arg,                  // arbitrary string argument 
		123.0f,                                // arbitrary float argument
		RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
	);
	eventSource->SendEvent(&my_event);
	last_speech_timestamp=std::chrono::steady_clock::now();		
}

void DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::string my_message) {
	// RE::DebugMessageBox(my_message.c_str());
	SKSE::log::info("The thought for the IMPORTANT THOUGHT channel is: {} ", my_message.c_str());
	// We want to broadcast mod events.  So we need this event source.
	std::string  mod_event_name = "SNMI_Pump_IMPORANT_PlayerThought";
	std::string  mod_event_string_arg = my_message; //  + standard_thought_instruction;
	auto eventSource = SKSE::GetModCallbackEventSource();

	if (DumpThoughts::too_early_after_game_load()) {
		SKSE::log::info("////////BLOCKING IMPORTANT thought message THOUGHT OUTPUT BECAUSE TOO EARLY AFTER RELOAD/////////");
		return;
	}

	SKSE::ModCallbackEvent my_event(
		mod_event_name,                        // event name
		mod_event_string_arg,                  // arbitrary string argument 
		123.0f,                                // arbitrary float argument
		RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
	);
	eventSource->SendEvent(&my_event);
	last_speech_timestamp=std::chrono::steady_clock::now();
}




