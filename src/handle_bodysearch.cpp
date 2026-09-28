#include "handle_bodysearch.h"

#include "DumpThoughts.h"

#include <string>
#include <string_view>

bool handle_bodysearch::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	const std::string_view event_name = a_event->eventName.c_str();

	// MOD EVENT:  From BodySearch, we have the following event:  AnimationStarting_BodySearch
	if (event_name == "AnimationStarting_BodySearch") {
		std::string thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But now the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. Let us know your response to that, and make sure you implicitly explain that you are being groped for pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		// More in this context:
		// StageStart_BodySearch
		// 4 seconds later:  AnimationStart_BodySearch
		// StageEnd_BodySearch
		return true;
	}
	if (event_name == "StageStart_BodySearch") {
		std::string thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But now the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. That has been going on for a while.  And now he is continuing to grope you even more as he pleases. Let us know your response to that. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		// More in this context:
		// StageStart_BodySearch
		// 4 seconds later:  AnimationStart_BodySearch
		// StageEnd_BodySearch
		return true;
	}
	if (event_name == "StageEnd_BodySearch" ||
		event_name == "AnimationStart_BodySearch" ||
		event_name == "AnimationEnd_BodySearch") {
		// nothing to do here, just exit.
		return true;
	}
	if (event_name == "AnimationEnding_BodySearch") {
		std::string thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But then the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. Finally now, the guard has finished groping you under the pretext of a body search and is finally letting go of you.  Let us know your response to that.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return true;
	}

	return false;
}