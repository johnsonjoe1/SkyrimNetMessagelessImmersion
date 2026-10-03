#include "handle_bimbos.h"
#include "handle_worn_equipment_change.h"
#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <string>
#include <string_view>

bool handle_bimbos::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event || a_event->eventName != "CC_ModBimboCorruption") {
		return false;
	}

/* [2026-08-16 17:57:25.863] MOD EVENT: Name: CC_ModBimboCorruption  StrArg: I love pretty jewellery!  NumArg: 2*/
	if (player_is_in_a_SL_scene() && a_event->strArg != "Oooh... I could get used to not being in control...") {
		SKSE::log::info("CC_ModBimboCorruption event detected, but player is in a scene and it's not the corrupting sex as a message, so we will not process it.");
		return true;  // In this case it really was a CC_ModBimboCorruption event and that means no further processing necessary in the main mod boadcast module.
	}

	std::string thought_message = "";
	std::string debug_message;
	if (a_event->strArg == "I love pretty jewellery!") {
		std::string slutty_item_worn = name_of_worn_slutty_item();
		if (slutty_item_worn.empty()) {
			// No matching named worn item
			LillithOnlyBox("No matching named worn item found for the bimbo corruption event, even though 'I love pretty jewellery!' was detected.  THIS IS A BUG!!");
			LillithOnlyBox("No matching named worn item found for the bimbo corruption event, even though 'I love pretty jewellery!' was detected.  THIS IS A BUG!!");
			LillithOnlyBox("No matching named worn item found for the bimbo corruption event, even though 'I love pretty jewellery!' was detected.  THIS IS A BUG!!");

			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  That is, beause she is wearing the bimbo jewelry, usually some piercings with jewelry to be precise, that add to the bimbo corruption of the PC.  Speak in character and let us know, that the pretty jewellery is getting to your mind and enhancing the bimbo corruption, turning you a bit more into a bimbo, or that you may end up a total bimbo, if you keep wearing it too long.");
		} else {
			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  That is, beause she is wearing the a very slutty item, the {} , and that adds to the bimbo corruption of the player.  Speak in character and let us know, that the extremely slutty item, is getting to your mind and enhancing the bimbo corruption, turning you a bit more into a bimbo, or that you may end up a total bimbo, if you keep wearing it too long.  And be sure to name the item {} in your response.", slutty_item_worn, slutty_item_worn);
		}
		debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
	} else if (a_event->strArg == "Dragons are so powerful. They just make me feel like... submitting.") {
		if (a_event->numArg < 0) {
			// Killing the dragon reduced bimbo corruption
			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  But now some of the bimbo corruption has been reduced from killing a dragon and absorbing its power.  Submitting to the dragon had cleared your mind a bit from the impure thoughts of the bimbofication curse.  Speak in character and let us know, that your mind feels now.");
		} else {
			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  At present, the source of the additional bimbo corruption is revealed via the string: {} .  Speak in character and let the player know, that additional bimbo corruption is seeping into your mind and turning you more into a bimbo from the source revealed in that string we just gave you.", a_event->strArg.c_str());
			debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
		}
		debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
	} else if (a_event->strArg == "This is a cure so I'd be worried if you saw this....") {
		if (a_event->numArg < 0) {
			// Receiving a cure from a priest or vigilant of Stendarr
			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  But now some of the bimbo corruption has been reduced from the special cure, that you just received from a priest or a vigilant.  This cure has cleared your mind from the impure thoughts of the bimbofication curse.  Speak in character and let us know, how your mind feels now.");
		} else {
			thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  The player character is slowly turned into a bimbo via a special bimbofication mod.  But now some of the bimbo corruption has been reduced from the special cure, that you just received from a priest or a vigilant.  But somehow the components you offered were too weak or you were just unlucky, and the cure backfired instead, and made the impure thoughts from the bimbofication curse even stronger now.  You can feel your mind slipping away.  Speak in character and let the player know, that something went wrong and additional bimbo corruption is seeping into your mind and turning you more into a bimbo.");
			debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
		}
		debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
	} else {
		thought_message = std::format("The player character is slowly turned into a bimbo via a special bimbofication mod.  At present, the source of the additional bimbo corruption is revealed via the string: {} .  Speak in character and let the player know, that additional bimbo corruption is seeping into your mind and turning you more into a bimbo from the source revealed in that string we just gave you.", a_event->strArg.c_str());
		debug_message = std::format("CC_ModBimboCorruption:  STR-ARG: {}  NUM-ARG: {}  ThoughtMessage: {}", a_event->strArg.c_str(), a_event->numArg, thought_message);
	}
	DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.

	LillithOnlyBox(debug_message);
	return true;
}