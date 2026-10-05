#include "handle_battlefuck.h"
#include "DumpThoughts.h"
#include <string>
#include <string_view>
#include "log.h"
#include "misc.h"

bool handle_battlefuck::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	const std::string_view event_name = a_event->eventName.c_str();

	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can the rape you right in from of everybody.  There may be bystanders who come to watch the spectacle as you are potentially getting raped.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped and raped and make it clear from your response, that a rape is about to happen to you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// BattleFuck includes the new spectator's display name in this event's string argument.
	if ( (std::strcmp(a_event->eventName.c_str() , "_BF_Onlookers_AddNotification") == 0)  ) {
		std::string thought_message = std::format(
			"YOU, the player, realize that another onlooker has arrived, but instead of helping you, that person has stopped to watch you get molested and used.  Express what you feel and think about that betrayal or humiliation.  BattleFuck described the new spectator as follows: '{}'.  If that description contains the spectator's name, use the name, but do not invent one.",
			a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("_BF_Onlookers_AddNotification:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// MOD EVENT:  Name:  "AnimationEnding_BattleFuck" :  this is the end of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, survived a sexual assault.  Maybe you got completely undressed by the attacker and maybe he even fucked you and came inside of you, while everybody else stood by and watched, but regardless of that, the assault is over now and you can get dressed again.  Say so and let us know what you are feeling and thinking in that moment.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnding_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	return false;  // If we didn't handle the event, return false.
}