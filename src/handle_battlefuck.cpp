#include "handle_battlefuck.h"
#include "DumpThoughts.h"
#include <string>
#include <string_view>
#include "log.h"
#include "misc.h"

static auto last_AnimationEnd_BattleFuck_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

bool handle_battlefuck::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	const std::string_view event_name = a_event->eventName.c_str();

	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can then rape you right in front of everybody later.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped forefully of all your clothes and make it clear from your response, that you are going to be stripped of all your clothing if you cannot manage to break free from his grip.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// MOD EVENT:  Name: StageStart_BattleFuck
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuck") == 0)  ) {
		if (!cooldown_has_passed(last_AnimationEnd_BattleFuck_thought_timestamp, 20))
		{
			return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		last_AnimationEnd_BattleFuck_thought_timestamp = std::chrono::steady_clock::now();
		std::string  thought_message = std::format("YOU, the player, are now forcefully stripped of more and more of your clothing and armour.  Also your attacker is pushing you down, to prepare you for the coming rape.  Let us know how you feel now.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("StageStart_BattleFuck:  " + thought_message);
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
		// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		// LillithOnlyBox("AnimationEnding_BattleFuck:  " + thought_message);
		// NO MESSAGE FOR NOW, AS WE DON'T KNOW WHAT THAT REALLY MEANS
		LillithOnlyBox("AnimationEnding_BattleFuck event received.  JUST FOR DEBUGGING.  NO MESSAGE FOR NOW.");
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can the rape you right in from of everybody.  There may be bystanders who come to watch the spectacle as you are potentially getting raped.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped and raped and make it clear from your response, that a rape is about to happen to you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// MOD EVENT:  Name:StageStart_BattleFuckRape :  this is the start of a BattleFuck rape sub-stage.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuckRape") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can the rape you right in from of everybody.  There may be bystanders who come to watch the spectacle as you are potentially getting raped.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped and raped and make it clear from your response, that a rape is about to happen to you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("StageStart_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// MOD EVENT:  Name:StageStart_BattleFuckRape :  this is the start of a BattleFuck rape sub-stage.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuckRape") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are currently getting raped.  Let us know what you are feeling and thinking in that moment.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("StageStart_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	
	// MOD EVENT:  Name: AnimationStarting_BattleFuckRape :  this is the end of a BattleFuck rape animation.  We may want to comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuckRape") == 0)  ) {
		std::string  thought_message = std::format("YOU, the player, couldn't fend of your attacker, and he stripped you of all your clothes, pushed you down and inserted himself into you.  Your are now about to get raped.  Reflect on the upcoming experience and express your feelings about it in character.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	// MOD EVENT:  Name:AnimationEnd_BattleFuckRape :  this is the end of a BattleFuck rape animation.  We may want to comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnd_BattleFuckRape") == 0)  ) {
		std::string  thought_message = std::format("YOU, the player, have just got raped and now finally the rape is over.  Reflect on the experience and express your feelings about it in character.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnd_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}



	// Now down here we handle any events that we do not need but also do not need to flag as unhandled
	if ( (std::strcmp(a_event->eventName.c_str() , "StageEnd_BattleFuck") == 0)  ||   // Small sub-stage-ends don't get a comment
		 (std::strcmp(a_event->eventName.c_str() , "AnimationStart_BattleFuck") == 0) ||  // We respond to AnimationStarting_BattleFuck instead
		 (std::strcmp(a_event->eventName.c_str() , "AnimationEnd_BattleFuck") == 0) ||      // We respond to AnimationEnding_BattleFuck instead
		 (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuckRape") == 0) ||
		 (std::strcmp(a_event->eventName.c_str() , "AnimationStart_BattleFuckRape") == 0)    // We resond to AnimationStartING instead
		) {
		// Nothing to do in that case
		// No need for a message, like for super-irrelevant it's the same:  SKSE::log::info("SKIPPING HANDLING OF IRRELEVANT MOD EVENT: Name: {}  StrArg: {}  NumArg: {}", a_event->eventName.c_str(), a_event->strArg, a_event->numArg);

		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	


	return false;  // If we didn't handle the event, return false.
}