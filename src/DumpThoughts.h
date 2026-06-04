#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class DumpThoughts
{
public:
	static void throw_out_BACKGROUND_TTS_thought_message(std::string my_message);
	static void throw_out_TTS_thought_message(std::string my_message);
	static void throw_out_IMPORTANT_TTS_thought_message(std::string my_message);
	static std::chrono::steady_clock::time_point GetLastSpeechTimestamp();
	static void reset_last_speech_timestamp();
	static void reset_lactacid_added_speech_timestamp();
	static void reset_last_game_load_or_reload_timestamp();
	static bool too_early_after_game_load();
	static bool too_early_for_next_lactacid_speech();
};

