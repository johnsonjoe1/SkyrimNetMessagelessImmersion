#include "handle_DDUD.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <cstring>
#include <string>

namespace
{
	auto last_chain_sound_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

/*
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:864] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=12
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Blindfold Script | Base ptr: 0x18a9a8f3900 | Base-FormID: 10031C77 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b41d780  |  Caster: Lillith 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Hood Script | Source FormID: 1103D2DF | Source EDID:  
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 10031C77 found: Blindfold Script
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:889] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=32
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Gag Food Removal Script | Base ptr: 0x18a9a8f2b00 | Base-FormID: 1002F13F | Base-Form Type: 18   
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b41d780  |  Caster: Lillith 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Hood Script | Source FormID: 1103D2DF | Source EDID:  
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 1002F13F found: Gag Food Removal Script
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:889] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=33
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Gag Script | Base ptr: 0x18a9b818f00 | Base-FormID: 1002B077 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b41d780  |  Caster: Lillith 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Hood Script | Source FormID: 1103D2DF | Source EDID:  
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 1002B077 found: Gag Script
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:889] .
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=34
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Muffling Script | Base ptr: 0x18a9b6ee400 | Base-FormID: 10090000 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b41d780  |  Caster: Lillith 
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Hood Script | Source FormID: 1103D2DF | Source EDID:  
[2026-09-21 22:11:01.959] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 10090000 found: Muffling Script
*/    
	bool try_handle_hood_effect(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* a_effect)
	{
		auto* base = a_effect->GetBaseObject();
		auto* source = a_effect->spell;
		if (!base || !source || std::strcmp(source->GetName(), "Hood Script") != 0) {
			return false;
		}

		if (std::strcmp(base->GetName(), "Blindfold Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood doesn't even let you see anything, so that you are completely blindfolded. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage hood, and that hood was keeping you completely blindfolded, but now you can see again. Say as much in your response, and be sure to make it clear, that you speak about the hood that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Blindfold-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		if (std::strcmp(base->GetName(), "Gag Food Removal Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood doesn't even let you eat or drink anything. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage hood, and that hood was keeping you from eating or drinking anything, but now you can eat and drink again. Say as much in your response, and be sure to make it clear, that you speak about the hood that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-Food-Removal-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		if (std::strcmp(base->GetName(), "Gag Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood gags you completely so that you cannot utter a single word. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage gag, and that gag was keeping you completely gagged, but now you can speak again. Say as much in your response, and be sure to make it clear, that you speak about the gag that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		return std::strcmp(base->GetName(), "Muffling Script") == 0;
	}

	/*
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:864] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=39
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Gag Food Removal Script | Base ptr: 0x18a9a8f2b00 | Base-FormID: 1002F13F | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b493b80  |  Caster: Lillith 
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Gag Script | Source FormID: 1002B078 | Source EDID:  
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 1002F13F found: Gag Food Removal Script
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:889] .
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:890] ABOVE IS A POTENTIALLY UNHANDLED MAGIC EFFECT??? CHECK THE BASE NAME AND SOURCE NAME TO SEE IF IT'S SOMETHING YOU WANT TO REACT TO, OR IF IT'S SOME RANDOM EFFECT THAT YOU DON'T CARE ABOUT.  IF IT'S THE LATTER, THEN YOU PROBABLY WANT TO ADD A NEW IF-STATEMENT FOR THIS EFFECT IN THIS HANDLER, SO THAT IT DOESN'T GET LOGGED IN SUCH DETAIL ANY MORE, BECAUSE THAT WOULD BE ANNOYING.  CHECK THE BASE NAME AND SOURCE NAME TO SEE WHAT EFFECT THIS IS ABOUT.  IF IT'S AN EFFECT YOU CARE ABOUT, THEN CONSIDER ADDING A CUSTOM MESSAGE FOR IT IN THIS HANDLER, SO THAT YOUR TTS CAN REACT TO IT IN A MEANINGFUL WAY! 
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:282] CHECKING FOR _SLS_STUFF! Currently investigating: Gag Script
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:864] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=40
[2026-09-21 22:40:07.968] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Gag Script | Base ptr: 0x18a9b818f00 | Base-FormID: 1002B077 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b493b80  |  Caster: Lillith 
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Gag Script | Source FormID: 1002B078 | Source EDID:  
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 1002B077 found: Gag Script
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:40:07.969] [log] [info] [handle_active_magic_effect_changes.cpp:889] .	
	*/    
	bool try_handle_gag_effect(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* a_effect)
	{
		auto* base = a_effect->GetBaseObject();
		auto* source = a_effect->spell;
		if (!base || !source || std::strcmp(source->GetName(), "Gag Script") != 0) {
			return false;
		}

		if (std::strcmp(base->GetName(), "Gag Food Removal Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a gag, and that gag doesn't even let you eat or drink anything. Say as much in your response, and be sure to make it clear, that you speak about the gag that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage gag, and that gag was keeping you from eating or drinking anything, but now you can eat and drink again. Say as much in your response, and be sure to make it clear, that you speak about the gag that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-Food-Removal-through-gag: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		if (std::strcmp(base->GetName(), "Gag Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a gag, and that gag gags you completely so that you cannot utter a single word. Say as much in your response, and be sure to make it clear, that you speak about the gag that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage gag, and that gag was keeping you completely gagged, but now you can speak again. Say as much in your response, and be sure to make it clear, that you speak about the gag that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-through-gag: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		return false;
	}
}

bool handle_DDUD::handle_DDUD_struggle_exhaustion_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	auto* base = a_effect->GetBaseObject();
	auto* source = a_effect->spell;
	if (!base || !source || std::strcmp(base->GetName(), "Exhaustion") != 0 ||
		std::strcmp(source->GetName(), "Struggle exhaustion") != 0) {
		return false;
	}

	if (a_event->isApplied) {
		SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION APPLICATION!");
		DumpThoughts::throw_out_TTS_thought_message(
			"YOU, the player, just tried getting out of your locking bondage devices for a whole while. You may have made some progress, but nevertheless now you are too exhausted to continue. Say as much in your response.");
	} else {
		SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION REMOVAL!");
		DumpThoughts::throw_out_TTS_thought_message(
			"YOU, the player, just were trying to get out of your locking bondage devices for a whole while. You may have made some progress, but in any case, that activity had made you exhausted to the point where you couldn't continue any more. But now time has passed and you're feeling better and you're good to go and maybe could continue trying. Say as much in your response.");
	}




    
	return true;
}

bool handle_DDUD::handle_DDUD_hood_magic_effect_stuff(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	return try_handle_hood_effect(a_event, a_effect);
}

bool handle_DDUD::handle_DDUD_gag_magic_effect_stuff(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	return try_handle_gag_effect(a_event, a_effect);
}


/*[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:536] Effect APPLIED on Lillith | UID=47
[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:539] Base name: Stagger when shouting | Base ptr: 0x20e387bae80 | Base-FormID: 1005380A | Base-Form Type: 18   (This means: MGEF) 
[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:540] base-Effect EDID: zad_effShoutStagger | Source ptr: 0x20e3854ee00  |  Caster: Lillith 
[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:544] Magnitude: 0 | Duration: 0
[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:547] Source name: Restrictive Corset | Source FormID: 10052D45 | Source EDID: zad_EnchCorset 
[2026-08-16 17:56:35.887] [log] [info] [handle_active_magic_effect_changes.cpp:553] Form LookupByID 1005380A found: Stagger when shouting*/
bool handle_DDUD::handle_DDUD_restrictive_corset_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	auto* base = a_effect->GetBaseObject();
	auto* source = a_effect->spell;
	if (!base || !source || std::strcmp(base->GetName(), "Stagger when shouting") != 0 ||
		std::strcmp(source->GetName(), "Restrictive Corset") != 0) {
		return false;
	}

	if (a_event->isApplied) {
		SKSE::log::info("Event handler for Stagger when shouting APPLICATION!");
		const std::string thought = "The corset you as the player just got locked into is so restrictive, that you can't even shout properly without staggering in this thing!  Say as much in your response, and be sure to make it clear, that you speak about the corset that you are wearing and also make it clear, that you can't shout properly any more (without staggering) while locked into this item.";
		DumpThoughts::throw_out_TTS_thought_message("Active Effect: Stagger when shouting: THOUGHT: " + thought);
		LillithOnlyBox(thought);
	}

	return true;
}

/*[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:536] Effect APPLIED on Lillith | UID=31
[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:539] Base name: ChainSoundEffect | Base ptr: 0x20e3799c480 | Base-FormID: 1101BB51 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:540] base-Effect EDID: zadx_HR_ChainSoundMagicEffect | Source ptr: 0x20e384a2740  |  Caster: Lillith 
[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:544] Magnitude: 0 | Duration: 0
[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:547] Source name: AnkleShackles-Enchant | Source FormID: 1102F518 | Source EDID: zadx_EnchAnkleShackles 
[2026-08-16 17:56:54.805] [log] [info] [handle_active_magic_effect_changes.cpp:553] Form LookupByID 1101BB51 found: ChainSoundEffect*/

bool handle_DDUD::handle_DDUD_chain_sound_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	auto* base = a_effect->GetBaseObject();
	auto* source = a_effect->spell;
	if (!base || !source || std::strcmp(base->GetName(), "ChainSoundEffect") != 0 ||
		std::strcmp(source->GetName(), "AnkleShackles-Enchant") != 0) {
		return false;
	}

	if (a_event->isApplied) {
		if (cooldown_has_passed(last_chain_sound_thought_timestamp, 180)) {
			SKSE::log::info("Event handler for ChainSoundEffect APPLICATION!");
			const std::string thought = "YOU, the player, are wearing ankle shackles with a chain, that is making sounds with every movement, so that it's impossible to move silently and sneak away in these things.  Say as much in your response, and be sure to make it clear, that you speak about the sounds from the ankle chains that you are wearing.";
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: ChainSoundEffect: THOUGHT: " + thought);
			LillithOnlyBox(thought);
			last_chain_sound_thought_timestamp = std::chrono::steady_clock::now();
		} else {
			SKSE::log::info("Skipping ChainSoundEffect thought because cooldown has not expired yet.");
		}
	}

	return true;
}