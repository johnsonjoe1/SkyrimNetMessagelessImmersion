#include "handle_SLAC.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <chrono>
#include <string>
#include <string_view>
#include <unordered_set>

namespace logger = SKSE::log;

namespace
{
	auto last_player_involving_SLAC_scene_start = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_player_involving_SLAC_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_SLAC_npc_start_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_SLAC_npc_end_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
}

bool handle_SLAC::update_scene_status_from_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	const std::string_view eventName = a_event->eventName.c_str();
	if ((eventName == "SNMI_SLAC_AnimationStarting_Player") || (eventName == "SNMI_SLAC_StageStart_Player")) {
		logger::info("Mod-Event-Based DISABLING OF CLOTHING-CHANGE-COMMENTS: {}  StrArg: {} ", a_event->eventName.c_str(), a_event->strArg.c_str());
		set_current_animation_status("in_a_scene", eventName);
		return true;
	}
	if (eventName == "SNMI_SLAC_AnimationEnding_Player") {
		logger::info("Mod-Event-Based RE-ENABLING OF CLOTHING-CHANGE-COMMENTS: {}\n{}\n.", a_event->eventName.c_str(), a_event->strArg.c_str());
		set_current_animation_status("not_in_a_scene", eventName);
		return true;
	}
	return false;
}

bool handle_SLAC::is_known_irrelevant_event(std::string_view a_eventName)
{
	static const std::unordered_set<std::string_view> ignoredEvents = {
		"OrgasmStart_slacEngagement",       // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"AnimationStarting_slacEngagement", // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"AnimationStart_slacEngagement",    // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"StageStart_slacEngagement",        // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"StageEnd_slacEngagement",          // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"AnimationChange_slacEngagement",
		"AnimationEnding_slacEngagement",   // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"AnimationEnd_slacEngagement",      // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
		"ActorChangeStart_slacEngagement",  // We disable all native SLAC events here, since we get our own version which tells us:  Player is involved or not in the scene
	};
	return ignoredEvents.contains(a_eventName);
}

bool handle_SLAC::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	const std::string_view eventName = a_event->eventName.c_str();

	if (eventName == "SNMI_SLAC_AnimationStarting_Player") {
		last_player_involving_SLAC_scene_start = std::chrono::steady_clock::now();
		std::string thought_message = std::format("A creature, an animal or a monster, has just managed to take advantage of you and start a sexual encounter with you, and you somehow were too horny and couldn't resist or couldn't escape in time and then just submitted into the sexual encounter.  Let us know your response to that, and make sure you mention or implicitly point out, that the creature managed to bring you down and initiate sex with you. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("SNMI_SLAC_AnimationStarting_Player:  " + thought_message);
		return true;	// Successfully handling and no further processing in the calling function
	}

	// Player involved:    SNMI_SLAC_StageStart_Player
	// No player involved: SNMI_SLAC_StageStart_NPC

	// Each stage change can receive it's own thought.
	if (eventName == "SNMI_SLAC_StageStart_Player") {
		// We wait 40 seconds after the initial comment for the first stage-comment.
		if (!cooldown_has_passed(last_player_involving_SLAC_scene_start, 40)) {
			return true;
		}
		// We wait 40 seconds after each stage-comment for the next stage comment.
		if (!cooldown_has_passed(last_player_involving_SLAC_thought_timestamp, 40)) {
			return true;
		}
		last_player_involving_SLAC_thought_timestamp = std::chrono::steady_clock::now();

		std::string thought_message = std::format("The creature, animal or monster, that came after you to have sex with you got you and it still isn't satisfied and wants to have even more sex with you and you were also too horny to really stop yourself.  Let us know your response to that, and make sure you mention or implicitly point out, that you are having sex with a creature.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("StageStart_slacEngagement:  " + thought_message);
		return true;	// Successfully handling and no further processing in the calling function
	}

	if (eventName == "SNMI_SLAC_AnimationEnding_Player") {
		std::string thought_message = std::format("Your sexual encounter with a creature, animal, or monster has just ended, and you are free to move on again. Let us know your immediate response to the encounter ending, and make sure you mention or implicitly point out that you just had sex with a creature. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("SNMI_SLAC_AnimationEnding_Player:  " + thought_message);
		return true;	// Successfully handling and no further processing in the calling function
	}

	// NPC SLAC events can also be commented.
	if (eventName == "SNMI_SLAC_AnimationStarting_NPC" || eventName == "SNMI_SLAC_AnimationEnding_NPC") {
		// We only comment, if the player isn't in a scene OF ANY KIND herself.
		if (player_is_in_ANY_SL_scene()) {		
			logger::info("Player is in an SL scene herself, so comment on other NPCs in SLAC events will be skipped for now.");			
			return true;	// Successfully handling and no further processing in the calling function
		}

		// Those events take a long time, but multiple such events can trigger in parallel, so we still implement a cooldown. 
		const bool isStarting = eventName == "SNMI_SLAC_AnimationStarting_NPC";
		auto& last_thought_timestamp = isStarting ? last_SLAC_npc_start_thought_timestamp : last_SLAC_npc_end_thought_timestamp;
		// if (!cooldown_has_passed(last_thought_timestamp, 30)) {
		//	return true;
		// }
		std::string context = a_event->strArg.c_str();
		const auto separator = context.find('|');
		const std::string npc_name = separator == std::string::npos ? "a nearby NPC" : context.substr(0, separator);
		const std::string creature_name = separator == std::string::npos ? "a creature" : context.substr(separator + 1);
		std::string thought_message = isStarting
			? std::format("You have just witnessed the very horny {} successfully engage with the poor and helpless {} and begin a sexual encounter with them nearby. Let us know your immediate response to seeing the creature and NPC begin their encounter, maybe with some empathy for the poor and helpless {}.  Be sure to explain what is happening at all in your response, because the player may not see the scene on screen and therefore might be confused if you don't give enough context and and don't explain what you are talking about here.", creature_name, npc_name, npc_name)
			: std::format("The sexual encounter between {} and {} that you witnessed nearby has just ended and {} has finally let go of {}. Let us know your immediate response to seeing the creature and NPC finish their scene, maybe with some empathy for the poor and helpless {}.  Be sure to explain what is happening at all in your response, because the player may not see the scene on screen and therefore might be confused if you don't give enough context and and don't explain what you are talking about here.", creature_name, npc_name, creature_name, npc_name, npc_name);
		DumpThoughts::throw_out_TTS_thought_message(thought_message);
		last_thought_timestamp = std::chrono::steady_clock::now();
		LillithOnlyBox(std::string(isStarting ? "SNMI_SLAC_AnimationStarting_NPC:  " : "SNMI_SLAC_AnimationEnding_NPC:  ") + thought_message);
		return true;	// Successfully handling and no further processing in the calling function
	}

	// NPC SLAC events can also be commented.
	if (eventName == "SNMI_SLAC_StageStart_NPC" ) {
		// We only comment, if the player isn't in a scene OF ANY KIND herself.
		if (player_is_in_ANY_SL_scene()) {		
			logger::info("Player is in an SL scene herself, so comment on other NPCs in SLAC events will be skipped for now.");			
			return true;	// Successfully handling and no further processing in the calling function
		}

		// Those events take a long time, but multiple such events can trigger in parallel, so we still implement a cooldown. 
		const bool isStarting = eventName == "SNMI_SLAC_StageStart_NPC";
		auto& last_thought_timestamp = isStarting ? last_SLAC_npc_start_thought_timestamp : last_SLAC_npc_end_thought_timestamp;
		if (!cooldown_has_passed(last_thought_timestamp, 40)) {
			return true;
		}
		std::string context = a_event->strArg.c_str();
		const auto separator = context.find('|');
		const std::string npc_name = separator == std::string::npos ? "a nearby NPC" : context.substr(0, separator);
		const std::string creature_name = separator == std::string::npos ? "a creature" : context.substr(separator + 1);
		std::string thought_message = std::format("You have just witnessed the very horny {} successfully engage with the poor and helpless {} and mounting her like animals do with their mates.  Now {} has been fucked by the animal.  But the horny creature still isn't satisfied and wants to fuck in even more and even differnt ways now.  Let us know your immediate response to seeing the creature and NPC begin their encounter, maybe with some empathy for the poor and helpless {}.  Be sure to explain what is happening at all in your response, because the player may not see the scene on screen and therefore might be confused if you don't give enough context and and don't explain what you are talking about here.", creature_name, npc_name, npc_name, npc_name);

		DumpThoughts::throw_out_TTS_thought_message(thought_message);
		last_thought_timestamp = std::chrono::steady_clock::now();
		LillithOnlyBox(std::string(isStarting ? "SNMI_SLAC_StageStart_NPC:  " : "SNMI_SLAC_StageStart_NPC:  ") + thought_message);
		return true;	// Successfully handling and no further processing in the calling function
	}

	return false;
}
