#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include <string_view>
#include <unordered_set>

std::string final_lillith_message;
static std::string current_animation_status = "not_in_a_scene";   // by default, we assume no SL scene is going on.
static auto last_disablement_timestamp = std::chrono::steady_clock::now();

void handle_timeout_for_stale_scenes() {

	if (current_animation_status == "not_in_a_scene") {
		return;  // If we are not in a scene, then we don't need to check for stale scenes.
	}
	
	auto now = std::chrono::steady_clock::now();
	auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_disablement_timestamp);
	SKSE::log::info("Time since last refresh or set of in_a_scene property (for filtering SL scenes): {} seconds", runtime.count());

	const int maximum_time_since_last_game_load_or_reload = 90;  // in seconds 

	/*
	// We make the timeout again dependend on the player name
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		SKSE::log::info("OVERRIDING natural timeout-after-game-load, because it's LILLITH, the debug character playing.", runtime.count(), minimum_time_since_last_game_load_or_reload);
		return false; 
	} 
	*/

	if (runtime.count() > maximum_time_since_last_game_load_or_reload) {
		SKSE::log::info("This scene is going on more than 90 seconds.  It may have run away.  We should reset to no-scene, just to be safe.");
		LillithOnlyBox("SNMI:  This scene is going on more than 90 seconds.  It may have run away.  We reset to no-scene, just to be safe.");
		set_current_animation_status("not_in_a_scene");
	}
}

bool player_is_in_a_SL_scene() {
	return current_animation_status == "in_a_scene";
}

void set_current_animation_status(std::string_view a_status)
{
	current_animation_status = std::string(a_status);
	if (current_animation_status == "in_a_scene") {
		last_disablement_timestamp = std::chrono::steady_clock::now();
	} else if (current_animation_status == "not_in_a_scene") {
		// do nothing here
	} else {
		// This is a bug and needs to be addressed
		SKSE::log::error("Unexpected animation status: {}", current_animation_status);

		RE::DebugMessageBox(("SNMI: Unexpected animation status: " + current_animation_status).c_str());  // Show the message in a message box if the player's name is Lillith.
	}

	SKSE::log::info("Current animation status updated to: {}", current_animation_status);
}

void LillithOnlyBox(std::string_view a_message)
{
	/*  ORIGINAL:
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(("SNMI:  An unhandled mod-event was discovered: " + debug_message).c_str());
	} else {
		SKSE::log::info("SNMI:  An unhandled mod-event was discovered: {}", debug_message);
	}  
	*/
	final_lillith_message = std::string("SNMI: LILLITH: ");
	final_lillith_message += a_message;
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(final_lillith_message.c_str());  // Show the message in a message box if the player's name is Lillith.
	} else {
		SKSE::log::info("LILLITH-MESSAGEBOX-REDIRECTED-TO-LOG: {}", a_message);  // Otherwise, log the message to the console.
	}  
}

