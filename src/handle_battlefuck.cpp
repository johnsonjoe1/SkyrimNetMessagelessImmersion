#include "handle_battlefuck.h"
#include "DumpThoughts.h"
#include <string>
#include <string_view>
#include "log.h"
#include "misc.h"

static auto last_battlefuck_stage_start_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static bool insertion_occurred = false;

bool handle_battlefuck::has_insertion_occurred()
{
	return insertion_occurred;
}

bool handle_battlefuck::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	const std::string_view event_name = a_event->eventName.c_str();

	if (event_name == "_BF_InsertionStarted") {
		insertion_occurred = true;
		LillithOnlyBox("BattleFuck insertion detected!!! (3x message)");
		LillithOnlyBox("BattleFuck insertion detected!!! (3x message)");
		LillithOnlyBox("BattleFuck insertion detected!!! (3x message)");
		set_current_battlefuck_status("in_a_scene", "BattleFuck insertion started");
		auto* aggressor = a_event->sender ? a_event->sender->As<RE::Actor>() : nullptr;
		const char* name = aggressor ? aggressor->GetDisplayFullName() : nullptr;
		std::string thought_message = "During the ongoing assault, the attacker has just crossed another boundary. Express your immediate emotional reaction in character, without describing the act.";
		if (name && *name) {
			thought_message += std::format(" The attacker is {}.", name);
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		return true;
	}

	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		insertion_occurred = false;
		// This event is always about the player, nobody else.
		set_current_battlefuck_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		std::string  thought_message = std::format("YOU, the player character, are now getting ambushed in a sexual assault.  Someone gips you from behind.  He probably wants to strip away your clothing and armour, so he can then rape you right in front of everybody later.  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped forefully of all your clothes and make it clear from your response, that you are going to be stripped of all your clothing if you cannot manage to break free from his grip.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  Name: StageStart_BattleFuck
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuck") == 0)  ) {
		set_current_battlefuck_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		if (!cooldown_has_passed(last_battlefuck_stage_start_thought_timestamp, 40))
		{
			return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		last_battlefuck_stage_start_thought_timestamp = std::chrono::steady_clock::now();

		std::string  thought_message = std::format("YOU, the player character, are now forcefully stripped of more and more of your clothing and armour.  Also your attacker is pushing you down, to prepare you for the coming rape.  Let us know how you feel now.");
		if (has_insertion_occurred())
		{
			thought_message = std::format("YOU, the player character, have already been stripped of your protecting clothes and armour.  He has also tied your hands together, so you can not resist any more, at least until you can untie your wrists.  And your attacker has successfully managed to penetrated you. Now the current event is this:  He is either fucking you in the first position after penetration or he is switching your body into another position to fuck you even more and at some point will reach his climax inside of you.  Let us know your situation and how you feel now.");
		}

		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("StageStart_BattleFuck:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// BattleFuck includes the new spectator's display name in this event's string argument.
	if ( (std::strcmp(a_event->eventName.c_str() , "_BF_Onlookers_AddNotification") == 0)  ) {
		set_current_battlefuck_status("in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		std::string thought_message = std::format(
			"YOU, the player character, realize that another onlooker has arrived, but instead of helping you, that person has stopped to watch you get molested and used, and they are enjoying that greatly.  Express what you feel and think about that additional detail and make sure you mention that this onlooker has arrived and doesn't help you at all.  BattleFuck described the new spectator as follows: '{}'.  If that description contains the spectator's name, use the name, but do not invent one.",
			a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("_BF_Onlookers_AddNotification:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// MOD EVENT:  Name:  "AnimationEnding_BattleFuck" :  this is the end of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BattleFuck") == 0)  ) {
		insertion_occurred = false;
		set_current_battlefuck_status("not_in_a_scene", std::format("Mod Event detected: {}", a_event->eventName.c_str()));
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player character, survived a sexual assault.  Maybe you got completely undressed by the attacker and maybe he even fucked you and came inside of you, while everybody else stood by and watched, but regardless of that, the assault is over now and you can get dressed again.  Say so and let us know what you are feeling and thinking in that moment.");
		// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		// LillithOnlyBox("AnimationEnding_BattleFuck:  " + thought_message);
		// NO MESSAGE FOR NOW, AS WE DON'T KNOW WHAT THAT REALLY MEANS
		LillithOnlyBox("AnimationEnding_BattleFuck event received.  JUST FOR DEBUGGING.  NO MESSAGE FOR NOW.");
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


	// MOD EVENT:  Name: AnimationStarting_BattleFuckRape :  this is the end of a BattleFuck rape animation.  We may want to comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuckRape") == 0)  ) {
		std::string  thought_message = std::format("YOU, the player character, have been raped by your attacker and are totally exhausted and powerless now. Other men standing around are using you powerlessness and also want to have a go with you. So the next guy is already here and starting to fuck you now. Let us know what is going on and tell us your feelings about it in character.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// MOD EVENT:  Name:StageStart_BattleFuckRape :  this is the start of a BattleFuck rape sub-stage.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BattleFuckRape") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player character, are currently getting raped by a different guy after you have been raped by the inital attacker already.  This new guy is either starting to rape you or already switching your body into a new position to fuck you even more. In either case, after the first guy successfully raped you, you stopped resisting for one reason or another. Let us know what you are feeling and thinking in that moment.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("StageStart_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationChange_BattleFuckRape") == 0)  ) {
		// This event is always about the player, nobody else.
		// NOTHING TO DO HERE, since we don't respond to sub-stage ends for sex scenes in general and also not here.
		LillithOnlyBox("AnimationChange_BattleFuckRape:  WE JUST RECEIVED AN ANIMATION-CHANGE-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		LillithOnlyBox("AnimationChange_BattleFuckRape:  WE JUST RECEIVED AN ANIMATION-CHANGE-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		LillithOnlyBox("AnimationChange_BattleFuckRape:  WE JUST RECEIVED AN ANIMATION-CHANGE-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	if ( (std::strcmp(a_event->eventName.c_str() , "ActorChangeStart_BattleFuckRape") == 0)  ) {
		// This event is always about the player, nobody else.
		// NOTHING TO DO HERE, since we don't respond to actor-change-start for sex scenes in general and also not here.
		LillithOnlyBox("ActorChangeStart_BattleFuckRape:  WE JUST RECEIVED AN ACTOR-CHANGE-START-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		LillithOnlyBox("ActorChangeStart_BattleFuckRape:  WE JUST RECEIVED AN ACTOR-CHANGE-START-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		LillithOnlyBox("ActorChangeStart_BattleFuckRape:  WE JUST RECEIVED AN ACTOR-CHANGE-START-EVENT.  WHAT DOES THAT IMPLY?  (3x message)");
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	// MOD EVENT:  Name:AnimationEnd_BattleFuckRape :  this is the end of a BattleFuck rape animation.  We may want to comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnd_BattleFuckRape") == 0)  ) {
		std::string  thought_message = std::format("YOU, the player character, just got raped at least two times now and now finally the rape is over.  After the first guy, you stopped resisting and the other guys took your body as they pleased. Reflect on the experience and express your feelings about it in character.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnd_BattleFuckRape:  " + thought_message);
		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// Now down here we handle any events that we do not need but also do not need to flag as unhandled
	if ( (std::strcmp(a_event->eventName.c_str() , "StageEnd_BattleFuck") == 0)  ||   // Small sub-stage-ends don't get a comment
	     (std::strcmp(a_event->eventName.c_str() , "StageEnd_BattleFuckRape") == 0) ||      // We respont to stage-start only, otherwise the thought channel gets spammed.
		 (std::strcmp(a_event->eventName.c_str() , "AnimationStart_BattleFuck") == 0) ||       // We respond to AnimationStarting_BattleFuck instead
		 (std::strcmp(a_event->eventName.c_str() , "AnimationStart_BattleFuckRape") == 0) ||   // We respond to AnimationStartING instead
		 (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BattleFuck") == 0) ||      // We respond to AnimationEnd_BattleFuck instead
		 (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BattleFuckRape") == 0)     // We respond to AnimationEnd_BattleFuckRape instead
		) {
		// Nothing to do in that case
		// No need for a message, like for super-irrelevant it's the same:  SKSE::log::info("SKIPPING HANDLING OF IRRELEVANT MOD EVENT: Name: {}  StrArg: {}  NumArg: {}", a_event->eventName.c_str(), a_event->strArg, a_event->numArg);

		return true;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	


	return false;  // If we didn't handle the event, return false.
}
