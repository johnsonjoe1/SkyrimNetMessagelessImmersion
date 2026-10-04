#include "handle_DDUD.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <cstring>
#include <string>
#include <string_view>
#include <unordered_map>

namespace
{
	auto last_chain_sound_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_muzzle_gag_ding_a_ling_sound_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_device_equipped_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	auto last_device_removed_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

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

/*[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:419] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:420] Effect APPLIED on Lillith | UID=33
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:423] Base name: Muzzle Gag Ding-a-Ling Sounds Slow | Base ptr: 0x1d03e5c0d40 | Base-FormID: 110586B9 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:424] base-Effect EDID: zadx_SndMuzzleGagDingaLingSlowMgef | Source ptr: 0x1d03e1c1f00  |  Caster: Lillith 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:428] Magnitude: 0 | Duration: 0
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:431] Source name: Muzzle Gag Script | Source FormID: 110586B4 | Source EDID: zad_enchGagDingaLing 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:437] Form LookupByID 110586B9 found: Muzzle Gag Ding-a-Ling Sounds Slow

[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:923] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:924] Effect APPLIED on Lillith | UID=30
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:927] Base name: BellSoundEffect | Base ptr: 0x20afb956400 | Base-FormID: 1101BB50 | Base-Form Type: 18   (This means: MGEF) 
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:928] base-Effect EDID:  | Source ptr: 0x20afb965fc0  |  Caster: Lillith 
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:932] Magnitude: 0 | Duration: 0
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:935] Source name: BellEnchantment | Source FormID: 1101BB4E | Source EDID:  
[2026-10-04 10:36:35.098] [log] [info] [handle_active_magic_effect_changes.cpp:941] Form LookupByID 1101BB50 found: BellSoundEffect

*/

bool handle_DDUD::handle_DDUD_muzzle_gag_ding_a_ling_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}
	auto* base = a_effect->GetBaseObject();
	if (!base) {
		return false;
	}
	const auto* base_name = base->GetName();
	if (std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Slow") != 0 &&
		std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Medium") != 0 &&
		std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Fast") != 0 &&
		std::strcmp(base_name, "BellSoundEffect") != 0) {
		return false;
	}
	if (a_event->isApplied) {
		SKSE::log::info("Event handler for Muzzle Gag Ding-a-Ling Sounds effect application or general bell!");
		if (cooldown_has_passed(last_muzzle_gag_ding_a_ling_sound_timestamp, 60 * 5)) {
			if (std::strcmp(base_name, "BellSoundEffect") == 0) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("YOU, the player, are dangling a cow bell, that is attached to one of your genital plugs and keeps dingling cow bell sounds whenever you move.  This is so humiliating.  In your response, you should cry out in desperation, that you are feeling so humiliated with that cow bell, that you are unable to remove or muffle due to your bondage.  This event is so important, that you can elaborate in many words about your desperation here.");
			} else {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("YOU, the player, are now dangling a little bell, that is attached to your muzzle gag and keeps dingling little bell sounds whenever you move.  This is so humiliating.  It's all just because of the gag you are wearing.  In your response, you should cry out in desperation, that you are feeling so humiliated with that little bell, that you are unable to remove or muffle due to your bondage.  This event is so important, that you can elaborate in many words about your desperation here.");
			}
			last_muzzle_gag_ding_a_ling_sound_timestamp = std::chrono::steady_clock::now();
		} else {
			SKSE::log::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> But the cooldown in Event handler for Muzzle Gag Ding-a-Ling Sounds effect application hasn't passed yet!");
		}
	}
	return true;
}

	// Maybe the player tried to access inventory or magic menu, but was prevented from doing so by Unforgiving Devices to enhance immersion and frustration.
	// We comment accordingly to enhance player experince.
	/*[2026-08-14 17:08:20.783] [log] [info] [handle_mod_broadcasts.cpp:547] MOD EVENT:  Name: ''DDUDNG_Event_Hotkey_captured_and_stopped''  StrArg: ''Find the hotkey in the number argument below''  NumArg: 23
	[2026-08-14 17:08:20.783] [log] [info] [handle_mod_broadcasts.cpp:978] An unhandled mod-event was discovered: MOD EVENT:  Name: DDUDNG_Event_Hotkey_captured_and_stopped  StrArg: Find the hotkey in the number argument below  NumArg: 23  */	
bool handle_DDUD::handle_DDUD_hotkey_captured_and_stopped_event(
	const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event || std::strcmp(a_event->eventName.c_str(), "DDUDNG_Event_Hotkey_captured_and_stopped") != 0) {
		return false;
	}

	std::string thought_message;
	if (a_event->numArg == 23) {
		thought_message = std::format("The player just tried to use a hotkey to access your inventory, but it was captured and stopped by Unforgiving Devices, because the PC is bound in heavy bondage devices.  This should enhances immersion and frustration.  So YOU as the PC should describe to the player in first person, how your hands and fingers are unable to reach the items in your inventory like this.");
	} else if (a_event->numArg == 15) {
		thought_message = std::format("The player just tried to use a hotkey to access your magic menu, but it was captured and stopped by Unforgiving Devices, because the PC is bound in heavy bondage devices.  This should enhances immersion and frustration.  So YOU as the PC should describe to the player in first person, how your hands are bound and you can't do magic like this.");
	} else if (a_event->numArg == 16) {
		thought_message = std::format("The player just tried to use a hotkey to access your quick access menu, but it was captured and stopped by Unforgiving Devices, because the PC is bound in heavy bondage devices.  This should enhances immersion and frustration.  So YOU as the PC should explain to the player in first person, how your hands are bound and you can't reach your weapons or gear like this.");
	}

	DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
	return true;
}

bool handle_DDUD::handle_DDUD_skyrimnet_event(
	const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event || std::strcmp(a_event->eventName.c_str(), "SkyrimNetDDUDNG_Event") != 0) {
		return false;
	}

	if (a_event->strArg == "Lillith's Genital Piercing (Common Soul Gem) stops vibrating." ||
		a_event->strArg == "Lillith's Genital Piercing (Common Soul Gem) starts vibrating.") {
		return true;
	}

	std::string thought_message = std::format("SkyrimNetDDUDNG_Event: ''{}''", a_event->strArg.c_str());
	return true;
}

bool handle_DDUD::handle_DDUD_device_events(
	const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event) {
		return false;
	}

	// MOD EVENT:  Name: DeviousEventTrip and Fall  StrArg: Beea  NumArg: 0
	if ((std::strcmp(a_event->eventName.c_str(), "DeviousEventTrip and Fall") == 0) || (std::strcmp(a_event->eventName.c_str(), "DeviousEventLeg Cuffs Trip Over") == 0)) {
		// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		std::string thought_message = std::format("YOU, the player character, just tripped over your own feet, because you are wearing these devious bondage boots or leg cuffs that you are locked into and cannot take off.  What are you thinking now based on this?  Be sure to mention that you just tripped or fell BECAUSE of these bondage boots or leg cuffs that you are forced to wear.", a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return true;
	}

	// MOD EVENT:  Name: DeviousEventStruggle  :  this seems to be a random forced struggle event with no real struggle but just for roleplay.  We make a comment.
	if ((std::strcmp(a_event->eventName.c_str(), "DeviousEventStruggle") == 0)) {
		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {
			std::string thought_message = std::format("YOU, the player character, just can't take it any more.  In a frenzy, you must get out of your bondage items now.  Say so and let us know what you are feeling and thinking.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviousEventStruggle: Event noticed, but it's NOT ABOUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}
		return true;
	}
	// MOD EVENT:  Name: DeviousEventArm cuff fumble  StrArg: Lillith  NumArg: 0
	if ((std::strcmp(a_event->eventName.c_str(), "DeviousEventArm cuff fumble") == 0)) {
		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {
			std::string thought_message = std::format("YOU, the player character, just can't take having these arm cuffs locked onto your wrists any more.  In a frenzy, you panic and feel that you must get out of your bondage items now.  Say so and let us know what you are feeling and thinking about these arm cuffs locked onto your wrists.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviousEventArm cuff fumble: Event noticed, but it's NOT ABOUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}
		return true;
	}
	// MOD EVENT:  Name: DeviousEventNipple Piercings  StrArg: Lillith  NumArg: 0
	if ((std::strcmp(a_event->eventName.c_str(), "DeviousEventNipple Piercings") == 0)) {
		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {
			std::string thought_message = std::format("YOU, the player character, just can't take having these nipple piercings locked onto you any more.  In a frenzy, you panic and feel that you must get out of your bondage items now.  Say so and let us know what you are feeling and thinking about these nipple piercings locked onto you.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviousEventNipple Piercings: Event noticed, but it's NOT ABOUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}
		return true;
	}
	// MOD EVENT:  Name: DeviousEventTight Corset  StrArg: Lillith  NumArg: 0
	if ((std::strcmp(a_event->eventName.c_str(), "DeviousEventTight Corset") == 0)) {
		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {
			std::string thought_message = std::format("YOU, the player character, just can't take having this tight corset locked onto you any more.  In a frenzy, you panic and feel that you must get out of your bondage items now.  Say so and let us know what you are feeling and thinking about this tight corset locked onto you.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviousEventTight Corset: Event noticed, but it's NOT ABOUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}
		return true;
	}


	// MOD EVENT:  Name: DeviceActorOrgasm  StrArg: Lillith  NumArg: 0
	if ((std::strcmp(a_event->eventName.c_str(), "DeviceActorOrgasm") == 0)) {
		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {
			std::string thought_message = std::format("YOU, the player, just orgasmed from the vibrating devices locked onto your body and into your sensitive parts.  You couldn't prevent it.  Gods, that was intense!  Say so and let us know what you are feeling and thinking.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviceActorOrgasm: Event noticed, but it's NOT ABUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}
		return true;
	}

	return false;
}

bool handle_DDUD::handle_DDUD_device_equipped_event(const SKSE::ModCallbackEvent* a_event)
	{
		static const std::unordered_map<std::string_view, std::string_view> equipped_device_thoughts = {

			//  Head things here:
			{"DeviceEquippedGag", "YOU, the player, just got a gag placed in your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedHeavy Gag", "YOU, the player, just got a heavy gag placed in your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedTape", "YOU, the player, just got a tape gag placed on your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedMask", "YOU, the player, just got a locking face-mask or hood placed on your face.  This mask may prevent you from seeing or speaking clearly.  It is securely fastened so that you cannot simply remove it.   "},
			{"DeviceEquippedblindfold", "YOU, the player, just got a blindfold placed over your eyes.  This blindfold prevents you from seeing anything.  It is securely fastened so that you cannot simply remove it.   "},
			{"DeviceEquippedHeavy Blindfold", "YOU, the player, just got a heavy blindfold placed over your eyes.  This blindfold prevents you from seeing anything.  It is securely fastened so that you cannot simply remove it.   "},

			// Collar-and-Yoke-things here:
			{"DeviceEquippedCollar", "YOU, the player, just got a collar locked onto your neck.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedyoke", "YOU, the player, just got locked into an iron bondage yoke. Such a yoke is an iron bondage device, that locks around your neck and wrists, trapping your wrists in a position next to your shoulders, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			
			// Hand things here:
			{"DeviceEquippedGloves", "YOU, the player, just got locked into bondage gloves.  Such gloves prevent the use of your fingers, making it impossible to perform delicate tasks. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedBondage Mittens", "YOU, the player, just got locked into bondage gloves.  Such gloves prevent the use of your fingers, making it impossible to perform delicate tasks. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedArm Cuffs", "YOU, the player, just got locked into arm cuffs.  The arm cuffs wrap around your wrists.  In addition, these cuffs can be connected together, e.g. behind your back, to further immobilize and restrain you, but this hasn't happened yet.  These devices got locked onto you and now you cannot get of out them.   "},
			{"DeviceEquippedElbowbinder", "YOU, the player, just got locked into an elbow binder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRare Armbinder", "YOU, the player, just got locked into an armbinder, even the rare sort.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedarmbinder", "YOU, the player, just got locked into an armbinder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRogueBinder", "YOU, the player, just got locked into a rogue armbinder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			
			// Torso things here:
			{"DeviceEquippedChastity Bra", "YOU, the player, just got a chastity bra locked onto your body.  This device is like a normal bra, but it is sturdy and prevents stimulation including self-stimulation and all access to your breasts.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedStraitJacket", "YOU, the player, just got locked into a strait jacket.  The jacket holds your arms and hands in tight sleeves bound around your torso, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRope Harness", "YOU, the player, just got locked into a rope harness.  The rope harness wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedSuit", "YOU, the player, just got locked into a catsuit of sorts.  The suit wraps around your torso and limbs. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedHarness", "YOU, the player, just got locked into a harness.  The harness wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedChain Harness Wrist Shackles", "YOU, the player, just got a chain harness wrist shackles locked onto your wrists.  These shackles prevent you from moving your arms freely.  They are securely fastened so that you cannot simply remove them.   "},
			{"DeviceEquippedBoxbinder", "YOU, the player, just got locked into a box binder.  The box binder holds your arms and hands in a bag behind your torso, so that you are helpless and completely at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedcorset", "YOU, the player, just got locked into a corsett.  The corsett wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			


			// Crotch things here:
			{"DeviceEquippedClitoris Piercing", "YOU, the player, just got a clitoris piercing locked onto your clitoris.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedClitoral Piercing", "YOU, the player, just got a clitoris piercing locked onto your clitoris.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedNipple Piercings", "YOU, the player, just got a nipple piercing locked onto your nipples.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedPiercing", "YOU, the player, just got a nipple piercing locked onto your nipples.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedVaginal Plug", "YOU, the player, just got a vaginal plug locked into your vagina.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedVaginal Pear Plug", "YOU, the player, just got a vaginal pear plug locked into your vagina.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedAnal Plug", "YOU, the player, just got a anal plug locked into your anus.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedPlug", "YOU, the player, just got a anal plug locked into your anus.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedAnal Pear Plug", "YOU, the player, just got an Anal Pear Plug plugged into your ass.  This plug is plugged into the anus, and then it expands inside, so that you cannot remove it any more.  And it is locked in this state, locked onto your body and now you cannot get if out without somehow opening the lock first.   "},
			{"DeviceEquippedChastity Belt", "YOU, the player, just got a chastity belt locked onto your body.  This device is like a normal panties, but it is absolutely sturdy and prevents stimulation including self-stimulation and masturbation all access to your pussy.  On the one hand it keeps you from having intercourse with a man and thus also might prevent rape unless the attacker has the key, on the other hand you can't access your own sex this way, which might be very frustrating when you are very horny.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedBelt", "YOU, the player, just got a chastity belt locked onto your body.  This device is like a normal panties, but it is absolutely sturdy and prevents stimulation including self-stimulation and masturbation all access to your pussy.  On the one hand it keeps you from having intercourse with a man and thus also might prevent rape unless the attacker has the key, on the other hand you can't access your own sex this way, which might be very frustrating when you are very horny.  This device got locked onto you and now you cannot get if off.   "},

			// Feet things here:
			{"DeviceEquippedBoots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedPumps", "YOU, the player, just got locked into bondage pumps and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedBallet Boots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedIron Ballet Boots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedPony Boots", "YOU, the player, just got locked into pony boots.  These pony boots wrap around your legs shape them like a horse leg, with hooves and horseshoe and all. In addition to that and to further de-humanize the wearer, the pony boots are making a sound like a walking pony whereever you go. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedLeg Cuffs", "YOU, the player, just got locked into leg cuffs.  The leg cuffs wrap around your ankles.  In addition, these cuffs can be connected together, to further immobilize and restrain you and force you to make only tiny hobble steps, but this hasn't happened yet.  These devices got locked onto you and now you cannot get of out them.   "},
			{"DeviceEquippedRubber Socks", "YOU, the player, just got locked into locking rubber socks.  The rubber socks wrap around your feet.  These devices got locked onto you and now you cannot get of out them.   "},			

		};

		const std::string_view event_name = a_event->eventName.c_str();
		const auto it = equipped_device_thoughts.find(event_name);
		if (it == equipped_device_thoughts.end()) {
			return false;
		}

		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0) {

			//  We POTENTIALLY throw out a thought message about the devices being equipped.  But we don't want to flood the queue.
			auto now = std::chrono::steady_clock::now();
			auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_device_equipped_thought_timestamp);
			LillithOnlyBox(std::format("Device-Equipped-Thought-Runtime-since-last-event: {}", runtime.count()));

			const int minimum_time_since_last_device_equipped_thought = 20;  // in seconds
			if (runtime.count() >= minimum_time_since_last_device_equipped_thought) {
				std::string final_thought_string = std::format("{}.  Say that in your response.", it->second);
				LillithOnlyBox(final_thought_string);
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string);
				last_device_equipped_thought_timestamp = now;
			}
		}

		return true;
	}




	
	bool handle_DDUD::handle_DDUD_device_removed_event(const SKSE::ModCallbackEvent* a_event)
	{
		static const std::unordered_map<std::string_view, std::string_view> removed_device_thoughts = {
			//  Feet things here:
			{"DeviceRemovedBoots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedBallet Boots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedIron Ballet Boots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedPony Boots", "YOU, the player, just got released from your pony boots.  Those boots had shaped your legs like horse legs and added hoof-like movement sounds, but these effects no longer apply now that they are removed.   "},
			
			
			{"DeviceRemovedyoke", "YOU, the player, just got released from your iron bondage yoke.  Your neck and wrists are no longer locked in that restrictive yoke posture, so those restraints no longer apply.   "},
			{"DeviceRemovedGloves", "YOU, the player, just got unlocked from gloves.  The gloves wrap around your hands like boxing gloves but with no thumb, so that you were unable to use your fingers for anything.  These devices got unlocked from your hands and now you can use your fingers again.   "},
			{"DeviceRemovedBondage Mittens", "YOU, the player, just got released from your bondage mittens.  Your fingers are no longer restrained by those mittens, so those restrictions no longer apply.   "},
			{"DeviceRemovedClitoris Piercing", "YOU, the player, just got your clitoris piercing removed.  That piercing can cause intense stimulation and inconvenient vibration, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedNipple Piercings", "YOU, the player, just got your nipple piercings removed.  Those piercings can cause intense stimulation and inconvenient vibration, but those effects no longer apply now that they are gone.   "},
			// NOTE:  These two again come with two different spellings.
			{"DeviceRemovedVaginal Plug", "YOU, the player, just got your vaginal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedPlugVaginal", "YOU, the player, just got your vaginal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			// Two different spellings again:
			{"DeviceRemovedAnal Plug", "YOU, the player, just got your anal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedPlugAnal", "YOU, the player, just got your anal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			
			
			
			{"DeviceRemovedCollar", "YOU, the player, just got your collar removed.  The feeling of being locked and restrained by that collar no longer applies now that it is gone.   "},
			{"DeviceRemovedChastity Bra", "YOU, the player, just got your chastity bra removed.  Access and stimulation restrictions on your breasts no longer apply now that the device is gone.   "},
			{"DeviceRemovedChastity Belt", "YOU, the player, just got your chastity belt removed.  The access and stimulation restrictions from that belt no longer apply now that it is gone.   "},
			{"DeviceRemovedBelt", "YOU, the player, just got your chastity belt removed.  The access and stimulation restrictions from that belt no longer apply now that it is gone.   "},
			
			
			{"DeviceRemovedStraitJacket", "YOU, the player, just got released from your strait jacket.  Your arms and hands are no longer bound against your torso, so those restraints no longer apply.   "},
			{"DeviceRemovedElbowbinder", "YOU, the player, just got released from your elbow binder.  Your arms are no longer forced behind your back in that restrictive position, so those restraints no longer apply.   "},
			{"DeviceRemovedRope Harness", "YOU, the player, just got released from your rope harness.  The constricting pressure around your torso no longer applies now that it is removed.   "},
			{"DeviceRemovedHarness", "YOU, the player, just got released from your harness.  The constricting and restrictive pressure from that harness no longer applies now that it is removed.   "},
			// NOTE:  These two again come with two different spellings.
			{"DeviceRemovedArmCuffs", "YOU, the player, just got your arm cuffs removed.  Your wrists are no longer cuffed and restrained, so those restrictions no longer apply.   "},
			{"DeviceRemovedArm Cuffs", "YOU, the player, just got your arm cuffs removed.  Your wrists are no longer cuffed and restrained, so those restrictions no longer apply.   "},
			{"DeviceRemovedWristRestraint", "YOU, the player, just got your wrist restraint removed.  Your wrists are no longer restrained, and you can move them freely now.   "},
			{"DeviceRemovedLeg Cuffs", "YOU, the player, just got your leg cuffs removed.  Your ankles are no longer cuffed and your movement is no longer restricted by them.   "},
			{"DeviceRemovedAnal Pear Plug", "YOU, the player, just got your anal pear plug removed.  The internal pressure and restrictive lock-in effects no longer apply now that it is gone.   "},
			{"DeviceRemovedGag", "YOU, the player, just got your gag removed.  Your mouth is no longer restrained, and you can speak and breathe freely again.   "},
			{"DeviceRemovedblindfold", "YOU, the player, just got your blindfold removed.  Your vision is no longer obstructed, so that sensory restriction no longer applies.   "},
			{"DeviceRemovedChain Harness Wrist Shackles", "YOU, the player, just got your chain harness wrist shackles removed.  Your wrists are no longer locked by those shackles, so those restraints no longer apply.   "},
			{"DeviceRemovedHood", "YOU, the player, just got your hood removed.  Your head and face are no longer covered, so that sensory restriction no longer applies.   "},
		};

		const std::string_view event_name = a_event->eventName.c_str();
		const auto it = removed_device_thoughts.find(event_name);
		if (it == removed_device_thoughts.end()) {
			return false;
		}

		if (std::strcmp(a_event->strArg.c_str(), RE::PlayerCharacter::GetSingleton()->GetName()) == 0 && !it->second.empty()) {

			//  We POTENTIALLY throw out a thought message about the devices being removed.  But we don't want to flood the queue.
			auto now = std::chrono::steady_clock::now();
			auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_device_removed_thought_timestamp);
			const int minimum_time_since_last_device_removed_thought = 20;  // in seconds
			if (runtime.count() >= minimum_time_since_last_device_removed_thought) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::string(it->second));
				last_device_removed_thought_timestamp = now;
			}
		}

		return true;
	}


// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
bool handle_DDUD::handle_DDUD_sentient_dialogue_event(const SKSE::ModCallbackEvent* a_event)
{
	if (!a_event || std::strcmp(a_event->eventName.c_str(), "UD_SentientDialogue") != 0) {
		return false;
	}

	// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
	std::string thought_message = std::format("YOU, the player, suddenly have a feeling like your {} is speaking to you, even though it is just an item and not a living creature.  Is it maybe time to question your sanity?  What is going on?  You have no clue, but you suspect it's some sentient device speaking to you.  Say so in your response. ", a_event->strArg.c_str());
	DumpThoughts::throw_out_TTS_thought_message(thought_message);
	return true;
}
