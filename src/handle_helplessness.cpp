#include "handle_helplessness.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <cstring>
#include <string>

namespace
{
	auto last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
}

bool handle_helplessness::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	// MOD EVENT:  Devious Helplessness Redux’s WD_Creatures.psc:  We have the following event through SexLab:  AnimationStarting_HelplessCreature
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationStarting_HelplessCreature") == 0)) {
		std::string thought_message = std::format("You are bound helpless and were just overcome by a creature wanting to fuck you.  You cannot prevent it due to your heavy bondage restraints.  Let us know your response to that, and make sure you implicitly explain that you cannto help getting fucked and used by the creature for its pleasure in your response. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return true;
	}
	// MOD EVENT:  Devious Helplessness Redux’s WD_Creatures.psc:  We have the following event through SexLab: StageStart_HelplessCreature 
	if ((std::strcmp(a_event->eventName.c_str(), "StageStart_HelplessCreature") == 0)) {
		std::string thought_message = std::format("You were overcome by an aggressive creature who exploited you being bound in bondage gear, so it could use you for sex.  Now the creature is moving you into yet another position to fuck you even more and even better exploit your helplessness.  You cannot prevent it due to your heavy bondage restraints.  And it seems the creature is by far not satisfied yet and still very horny and you cannot get away due to your helplessness in bondage.  Let us know your response to that in your response. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		return true;
	}
	// MOD EVENT:  Devious Helplessness Redux’s WD_Creatures.psc:  We have the following event through SexLab: AnimationEnd_HelplessCreature 
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationEnd_HelplessCreature") == 0)) {
		// This event is always about the player, nobody else.
		std::string thought_message = std::format("YOU, the player character, are trapped in bondage gear and were helpless and could not prevent the creature from fucking you as it wanted. But now the creature has finished and finally let go of you. You are still bound, but at least you are not raped by a creature any longer.  Say so in your response and let us know how you feel now.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnd_Helpless:  " + thought_message);
		return true;
	}


	// MOD EVENT:  Devious Helplessness Redux’s Util.psc: We have the following event through SexLab: AnimationStarting_Helpless 
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationStarting_Helpless") == 0)) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string thought_message = std::format("YOU, the player character, are currently still helpless in your bondage gear and at the mercy of others.  But now someone is coming to take advantage of your helplessness and is going to use your body for sex and breeding and there is nothing you can do about it due to being bound in bondage gear.  You can only cry out in desperation and fear.  Let us feel your fear of being used for sex in this way in your response and let us know that it is too late and the sexual assault is going to happen.  Announce that in your response, so that the player is alerted to the situation.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_Helpless:  " + thought_message);
		return true;
	}
	// MOD EVENT:  Devious Helplessness Redux’s Util.psc: We have the following event through SexLab: AnimationChange_Helpless 
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationChange_Helpless") == 0)) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string thought_message = std::format("YOU, the player character, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding and there was nothing you were able to do to prevent that.  But now the attacker wants sex in yet another position, while you are still completely helpless and can do nothing to prevent your own abuse and rape.  Announce that in your response.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationChange_Helpless:  " + thought_message);
		return true;
	}


	// MOD EVENT:  Devious Helplessness Redux’s Util.psc: We have the following event through SexLab: StageStart_Helpless 
	if ((std::strcmp(a_event->eventName.c_str(), "StageStart_Helpless") == 0)) {
		// This event is always about the player, nobody else.
		std::string thought_message = std::format("YOU, the player character, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding.  And now he (or she) is continuing to fuck you in yet another position.  And you can't do anything to stop it. Say so in your response and let us know how you feel.");

		// We need a cooldown here again for this part, because it might flood the queue too much otherwise.
		if (std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp < std::chrono::seconds(20)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: StageStart_Helpless because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp).count());
			return true;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this might need a cooldown.
		LillithOnlyBox("StageStart_Helpless:  " + thought_message);
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		return true;
	}
	// MOD EVENT:  Devious Helplessness Redux’s Util.psc: We have the following event through SexLab: AnimationEnd_Helpless
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationEnd_Helpless") == 0)) {
		// This event is always about the player, nobody else.
		std::string thought_message = std::format("YOU, the player character, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding and there was nothing you were able to do to prevent that.  But now your ordeal is over and the attacker has finally let go of you.  You are still trapped in your bondage gear, but at least the attacker stopped raping you.  Say so in your response and let us know how you feel now.");


		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnd_Helpless:  " + thought_message);
		return true;
	}
	// MOD EVENT:  the rest of the sequence, where we don't do anything and just return on those events.
	if ((std::strcmp(a_event->eventName.c_str(), "AnimationStart_Helpless") == 0) ||
		(std::strcmp(a_event->eventName.c_str(), "AnimationStarting_Helpless") == 0) ||
		// (std::strcmp(a_event->eventName.c_str(), "StageStart_Helpless") == 0) ||
		(std::strcmp(a_event->eventName.c_str(), "StageEnd_Helpless") == 0) ||
		(std::strcmp(a_event->eventName.c_str(), "AnimationEnding_Helpless") == 0)) {
		return true;
	}
	// The sequence is:
	// DONE:  AnimationStarting_Helpless
	// NOTHINGTODO:  AnimationStart_Helpless
	// NOTHINGTODO:  StageStart_Helpless
	// NOTHINGTODO:  StageEnd_Helpless
	// DONE:  AnimationChange_Helpless
	// NOTHINGTODO:  StageStart_Helpless
	// NOTHINGTODO:  StageEnd_Helpless
	// NOTHINGTODO:  AnimationEnding_Helpless
	// DONE:  AnimationEnd_Helpless

	return false;
}