#include "handle_jailrape.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <string_view>

bool handle_jailrape::try_handle_mod_event(
	const SKSE::ModCallbackEvent* a_event,
	std::chrono::steady_clock::time_point& a_lastThoughtTimestamp)
{
	const std::string_view eventName = a_event->eventName.c_str();
	if (eventName == "AnimationStarting_JailRapePC") {
		a_lastThoughtTimestamp = std::chrono::steady_clock::now();
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. Now one of them is going to rape you for his own fun and pleasure. You can only cry out in desperation and fear about what is to come. Announce that in your response, so that the player is alerted to the situation.";
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("AnimationStarting_JailRapePC: " + thoughtMessage);
		return true;
	}

	if (eventName == "AnimationChange_JailRapePC") {
		a_lastThoughtTimestamp = std::chrono::steady_clock::now();
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. One of them has already used you for his own fun and pleasure. But now he wants even more sex. He wants to try even more different sex positions with you. And to use your body in yet more ways. You cannot stop him from doing what he wants with you.";
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("AnimationChange_JailRapePC: " + thoughtMessage);
		return true;
	}

	if (eventName == "StageStart_JailRapePC") {
		const std::string thoughtMessage = "YOU, the player, are imprisoned by the guards. One of them has already used you for his own fun and pleasure. But he wants even more sex. You are forced to play along and do what he wants. You cannot stop what is happening to you, because the attacker is too strong. You can try to resist, but that might make him even more aggressive. You can try not to get excited from the sexual stimulation of your body, but even that is becoming more difficult, and you can slowly feel yourself getting involuntarily more sexually excited. Or you can start to break and start to submit and lose your will to resist entirely, accepting the guards as your new masters, and accepting that it is better to obey them than face more punishment and hoping, that if you can please the guards better, they might let you go and not be mean to you any more. ";
		if (std::chrono::steady_clock::now() - a_lastThoughtTimestamp < std::chrono::seconds(15)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: StageStart_JailRapePC because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - a_lastThoughtTimestamp).count());
			return true;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
		LillithOnlyBox("StageStart_JailRapePC: " + thoughtMessage);
		a_lastThoughtTimestamp = std::chrono::steady_clock::now();
		return true;
	}

	return false;
}