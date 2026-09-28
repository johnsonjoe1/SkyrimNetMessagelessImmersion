#include "handle_captive_defeat.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <chrono>
#include <string>
#include <string_view>

namespace
{
	auto last_captive_defeat_end_sex_scene_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

	void emit_thought(std::string_view event_name, std::string_view thought)
	{
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::string(thought));
		LillithOnlyBox(std::string(event_name) + ":  " + std::string(thought));
	}
}

bool handle_captive_defeat::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	const std::string_view event_name = a_event->eventName.c_str();
	if (event_name == "CaptiveDefeatWounded") {
		emit_thought(event_name, "It seems you have been wounded badly in your battle with your adversaries.  They have not captured you yet.  You must try to regain your strength quickly, or they might take you captive and do god-knows-what with you.  Explain that to the player via your response.  You can also add your feelings about this, while you speak in-character.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.");
		return true;
	}

	if (event_name == "CaptiveDefeatSurrender") {
		emit_thought(event_name, "It seems you have lost the battle with your adversaries. They have defeated you and you went down.  You are unable to fight and can only surrender from here on.  Tell us how you feel about that.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.");
		return true;
	}

	if (event_name == "CaptivePlayerRestrain") {
		emit_thought(event_name, "It seems you have lost the battle with your adversaries. They have defeated you and you went down.  But you didn't die.  But you are unable to fight.  So now they come and tie you up to take you captive and use you for whatever purpose, sell you as a slave or use you as a sex toy.  In any case, they are slowly tying you up in bondage and chains.  Tell us how you feel about that.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.");
		return true;
	}

	if (event_name == "CaptiveDefeatStart") {
		emit_thought(event_name, "It seems you have lost the battle with your adversaries. They have defeated you and tied you up.  Now you are facing the prospect of a life in captivity as a slave or as a sex toy.  Tell us how you feel about that.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.");
		return true;
	}

	if (event_name == "CaptiveDefeatRobPlayer") {
		emit_thought(event_name, "It seems your enemies have taken you captive. Now you also have been robbed of all your things, even your armor and clothes and you are completely defenseless.  Tell us how you feel about that.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.");
		return true;
	}

	if (event_name != "CaptiveDefeatEndSexScene") {
		return false;
	}

	if (!cooldown_has_passed(last_captive_defeat_end_sex_scene_thought_timestamp, 180)) {
		SKSE::log::info("=====SKIPPING MOD EVENT: CaptiveDefeatEndSexScene because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_captive_defeat_end_sex_scene_thought_timestamp).count());
		return true;
	}

	const std::string_view thought = "It seems your enemies have taken you captive.  You were forced into a humiliating sex scene, but now your captors seem satisfied and the raping has just stopped, at least for the moment.  But you are still a captive and will be for the foreseeable future, unable to escape their control.  Tell us how you feel about that.  Since this is a major event and will lead to potentially longer enslavement, you can speak in more details and many words about this and understands the implications of what is happening to you now.";
	DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::string(thought));
	last_captive_defeat_end_sex_scene_thought_timestamp = std::chrono::steady_clock::now();
	LillithOnlyBox("CaptiveDefeatEndSexScene:  " + std::string(thought));
	return true;
}