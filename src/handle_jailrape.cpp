#include "handle_jailrape.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <string_view>
#include <unordered_set>

namespace
{
	auto last_jailrape_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_jailrape_npc_start_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_jailrape_npc_orgasm_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_jailrape_npc_end_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
}

bool handle_jailrape::is_known_irrelevant_event(std::string_view a_eventName)
{
	static const std::unordered_set<std::string_view> ignoredEvents = {
		// "AnimationStarting_JailRapeNPC",  // this will be used below
		"AnimationStart_JailRapeNPC",
		"AnimationChange_JailRapeNPC",
		// "OrgasmStart_JailRapeNPC",  // this will be used below
		"StageStart_JailRapeNPC",
		"StageEnd_JailRapeNPC",
		// "AnimationEnding_JailRapeNPC",  // this will be used below
		"AnimationEnd_JailRapeNPC",
		
		"AnimationStart_JailRapePC",
		"StageEnd_JailRapePC",
	};
	return ignoredEvents.contains(a_eventName);
}

bool handle_jailrape::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	const std::string_view eventName = a_event->eventName.c_str();
	if (eventName == "AnimationStarting_JailRapePC") {
		set_current_animation_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		last_jailrape_thought_timestamp = std::chrono::steady_clock::now();
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. Now one of them is going to rape you for his own fun and pleasure. You can only cry out in desperation and fear about what is to come. Announce that in your response, so that the player is alerted to the situation.";
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("AnimationStarting_JailRapePC: " + thoughtMessage);
		return true;
	}
	if (eventName == "AnimationChange_JailRapePC") {
		set_current_animation_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		last_jailrape_thought_timestamp = std::chrono::steady_clock::now();
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. One of them has already used you for his own fun and pleasure. But now he wants even more sex. He wants to try even more different sex positions with you. And to use your body in yet more ways. You cannot stop him from doing what he wants with you.";
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("AnimationChange_JailRapePC: " + thoughtMessage);
		return true;
	}
	if (eventName == "StageStart_JailRapePC") {
		set_current_animation_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. One of them has already used you for his own fun and pleasure. But he wants even more sex. You are forced to play along and do what he wants. You cannot stop what is happening to you, because the attacker is too strong. You can try to resist, but that might make him even more aggressive. You can try not to get excited from the sexual stimulation of your body, but even that is becoming more difficult, and you can slowly feel yourself getting involuntarily more sexually excited. Or you can start to break and start to submit and lose your will to resist entirely, accepting the guards as your new masters, and accepting that it is better to obey them than face more punishment and hoping, that if you can please the guards better, they might let you go and not be mean to you any more. ";
		if (std::chrono::steady_clock::now() - last_jailrape_thought_timestamp < std::chrono::seconds(15)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: StageStart_JailRapePC because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_jailrape_thought_timestamp).count());
			return true;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("StageStart_JailRapePC: " + thoughtMessage);
		last_jailrape_thought_timestamp = std::chrono::steady_clock::now();
		return true;
	}
	if (eventName == "AnimationEnd_JailRapePC") {
		set_current_animation_status("not_in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		last_jailrape_thought_timestamp = std::chrono::steady_clock::now();
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. Now has finished raping you for his own fun and pleasure. And now he is finished and lets go of your body.  Respond in character and let us know how you feel, and metion that the rape is finally over in your response.";
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("AnimationStarting_JailRapePC: " + thoughtMessage);
		return true;
	}

	// NOW WE ALSO TREAT OTHER PRISONERS BEING USED BY THE GUARDS, but only if there is no scene involving the player yet 
	if (eventName == "AnimationStarting_JailRapeNPC") {
		if (!player_is_in_ANY_SL_scene() && cooldown_has_passed(last_jailrape_npc_start_thought_timestamp, 150)) { // We only speak about other prisoners being used, if the player isn't being used herself
			const std::string thoughtMessage = "YOU, the player, can hear in the distance how the guards are starting to use another prisoner for their own fun and pleasure. Respond in character and let the player know through your response, that the guards are starting to use another prisoner somewhere else in jail. Your response may be full of empathy for the poor woman.";
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
			LillithOnlyBox("AnimationStarting_JailRapeNPC: " + thoughtMessage);
			last_jailrape_npc_start_thought_timestamp = std::chrono::steady_clock::now();
		}
		return true;		
	}
	if (eventName == "OrgasmStart_JailRapeNPC") {
		if (!player_is_in_ANY_SL_scene() && cooldown_has_passed(last_jailrape_npc_orgasm_thought_timestamp, 150)) { // We only speak about other prisoners being used, if the player isn't being used herself
			const std::string thoughtMessage = "The guards are using another prisoner for their own fun and pleasure in the distance for quite a while.  Now you can hear, how the guards managed to make the poor other woman have an orgasm from that treatment. Respond in character and let the player know through your response, that the other prisoner was just made to orgasm somewhere else in jail. Your response may be full of empathy for the poor woman.";
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
			LillithOnlyBox("OrgasmStart_JailRapeNPC: " + thoughtMessage);
			last_jailrape_npc_orgasm_thought_timestamp = std::chrono::steady_clock::now();
		}
		return true;	
	}
	if (eventName == "AnimationEnding_JailRapeNPC") {
		if (!player_is_in_ANY_SL_scene() && cooldown_has_passed(last_jailrape_npc_end_thought_timestamp, 150)) { // We only speak about other prisoners being used, if the player isn't being used herself
			const std::string thoughtMessage = "YOU, the player character, can hear in the distance how the guards have now finished using another prisoner for their own fun and pleasure. Respond in character and let the player know through your response, that from just hearing it, you can tell the guards now let go of the other woman somewhere else in jail. Make it clear that you are speaking about the other prisoner's experience. Your response may be full of empathy for the poor woman.";
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
			LillithOnlyBox("AnimationEnding_JailRapeNPC: " + thoughtMessage);
			last_jailrape_npc_end_thought_timestamp = std::chrono::steady_clock::now();
		}
		return true;	
	}

	return false;
}