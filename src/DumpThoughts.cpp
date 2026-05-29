#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include "DumpThoughts.h"

namespace logger = SKSE::log;

static auto last_speech_timestamp = std::chrono::steady_clock::now();

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping functions and message queuing function for thoughts are all
//  what this class can do.  
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

std::chrono::steady_clock::time_point DumpThoughts::GetLastSpeechTimestamp() {
	return last_speech_timestamp;
}
void DumpThoughts::reset_last_speech_timestamp() {
	last_speech_timestamp = std::chrono::steady_clock::now();
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
	SKSE::ModCallbackEvent my_event(
		mod_event_name,                        // event name
		mod_event_string_arg,                  // arbitrary string argument 
		123.0f,                                // arbitrary float argument
		RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
	);
	eventSource->SendEvent(&my_event);
	last_speech_timestamp=std::chrono::steady_clock::now();
}




