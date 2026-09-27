#include "handle_alchemy_magic_effects.h"

#include "DumpThoughts.h"
#include "log.h"

#include <cstring>
#include <format>




	/*
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:677] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:678] Effect APPLIED on Non-Lillith | UID=34
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:681] Base name: Fatigue | Base ptr: 0x1d5e06b17c0 | Base-FormID: 73F23 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:682] base-Effect EDID: AlchDamageStaminaRavage | Source ptr: 0x1d5ded89a80  |  Caster: Non-Lillith 
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:686] Magnitude: -1 | Duration: 2.6599998
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:689] Source name: Thistle Branch | Source FormID: 134AA | Source EDID: Thistle01 
[2026-09-06 09:23:38.663] [log] [info] [handle_active_magic_effect_changes.cpp:695] Form LookupByID 73F23 found: Fatigue
*/
/*
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:677] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:678] Effect APPLIED on Non-Lillith | UID=33
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:681] Base name: Drain Intelligence | Base ptr: 0x1d5f4645b40 | Base-FormID: 6904C7FB | Base-Form Type: 18   (This means: MGEF) 
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:682] base-Effect EDID: AlchDrainIntelligence_KRY | Source ptr: 0x1d5ded8d080  |  Caster: Non-Lillith 
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:686] Magnitude: -1.38 | Duration: 30
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:689] Source name: Red Mountain Flower | Source FormID: 77E1D | Source EDID: MountainFlower01Red 
[2026-09-06 09:23:36.988] [log] [info] [handle_active_magic_effect_changes.cpp:695] Form LookupByID 6904C7FB found: Drain Intelligence

[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1111] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1112] Effect APPLIED on Lillith | UID=33
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1115] Base name: Drain Intelligence | Base ptr: 0x1d9f0da5d00 | Base-FormID: 6604C7FB | Base-Form Type: 18   (This means: MGEF) 
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1116] base-Effect EDID:  | Source ptr: 0x1da0c6342c0  |  Caster: Lillith 
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1120] Magnitude: -3.16 | Duration: 30
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1123] Source name: Ayleid Moon Moth | Source FormID: 6632B1A3 | Source EDID:  
[2026-09-26 15:43:14.067] [log] [info] [handle_active_magic_effect_changes.cpp:1129] Form LookupByID 6604C7FB found: Drain Intelligence

[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1111] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1112] Effect APPLIED on Lillith | UID=36
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1115] Base name: Damage Health | Base ptr: 0x1d9dea5f200 | Base-FormID: 3EB42 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1116] base-Effect EDID:  | Source ptr: 0x1d9dd14c2c0  |  Caster: Lillith 
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1120] Magnitude: -9.38 | Duration: 0.01
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1123] Source name: Nirnroot | Source FormID: 59B86 | Source EDID:  
[2026-09-26 15:43:27.173] [log] [info] [handle_active_magic_effect_changes.cpp:1129] Form LookupByID 3EB42 found: Damage Health

*/
/*
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:718] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:719] Effect APPLIED on Non-Lillith | UID=39
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:722] Base name: Damage Stamina Regen | Base ptr: 0x1b1dfb82240 | Base-FormID: 73F2C | Base-Form Type: 18   (This means: MGEF) 
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:723] base-Effect EDID: AlchDamageStaminaRate | Source ptr: 0x1b1df66b240  |  Caster: Non-Lillith 
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:727] Magnitude: -4.02 | Duration: 30
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:730] Source name: Skeever Tail | Source FormID: 3AD6F | Source EDID: SkeeverTail 
[2026-09-06 12:17:39.192] [log] [info] [handle_active_magic_effect_changes.cpp:736] Form LookupByID 73F2C found: Damage Stamina Regen
*/
/*
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:718] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:719] Effect APPLIED on Non-Lillith | UID=40
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:722] Base name: Damage Stamina Regen | Base ptr: 0x1b1dfb82240 | Base-FormID: 73F2C | Base-Form Type: 18   (This means: MGEF) 
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:723] base-Effect EDID: AlchDamageStaminaRate | Source ptr: 0x1b20714ed40  |  Caster: Non-Lillith 
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:727] Magnitude: -4.36 | Duration: 0.29999998
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:730] Source name: Spider Silk | Source FormID: 1CC0638 | Source EDID: CACO_SpiderSilk 
[2026-09-06 12:17:44.455] [log] [info] [handle_active_magic_effect_changes.cpp:736] Form LookupByID 73F2C found: Damage Stamina Regen
*/
/*
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:756] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:757] Effect REMOVED on Non-Lillith | UID=32
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:760] Base name: Reduce Orgasm Resistance | Base ptr: 0x2a5f1b90800 | Base-FormID: 241553D6 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:761] base-Effect EDID: UD_ReduceOrgasmResist_ME | Source ptr: 0x2a5f2250e00  |  Caster: Non-Lillith 
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:765] Magnitude: 2.5 | Duration: 0.35
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:768] Source name: Ancient Seed | Source FormID: 241553DB | Source EDID: UD_AncientSeed 
[2026-09-06 15:17:37.685] [log] [info] [handle_active_magic_effect_changes.cpp:774] Form LookupByID 241553D6 found: Reduce Orgasm Resistance
*/


bool handle_alchemy_magic_effects::handle_magic_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	auto* base = a_effect->GetBaseObject();
	auto* source = a_effect->spell;
	if (!base || !source) {
		return false;
	}

	const auto* base_name = base->GetName();
	const auto* source_name = source->GetName();
	if (std::strcmp(base_name, "Fatigue") == 0 && std::strcmp(source_name, "Thistle Branch") == 0) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for FATIGUE effect application from Thistle Branch!");
			DumpThoughts::throw_out_TTS_thought_message("YOU, the player, just ate a piece of Thistle Branch and received a fatigue effect from it.  Say as much in your response, and make sure you make it clear that Thistle Branch simply causes fatigue.");
		}
		return true;
	}

	if (std::strcmp(base_name, "Drain Intelligence") == 0 &&
		(std::strcmp(source_name, "Red Mountain Flower") == 0 || std::strcmp(source_name, "Ayleid Moon Moth") == 0)) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for DRAIN INTELLIGENCE effect application from Red Mountain Flower or Ayleid Moon Moth!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just ate a piece of {} and received a drain intelligence effect from it.    Say as much in your response, and make sure you make it clear that {} simply causes drain intelligence effect.", source_name, source_name));
		}
		return true;
	}

	if (std::strcmp(base_name, "Damage Health") == 0 && std::strcmp(source_name, "Nirnroot") == 0) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for DAMAGE HEALTH effect application from Nirnroot or Ayleid Moon Moth!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just ate a piece of {} and received a damage health effect from it.    Say as much in your response, and make sure you make it clear that {} simply causes damage health effect.", source_name, source_name));
		}
		return true;
	}

	if (std::strcmp(base_name, "Damage Stamina Regen") == 0 && std::strcmp(source_name, "Skeever Tail") == 0) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for DAMAGE STAMINA REGEN effect application from Skeever Tail!");
			DumpThoughts::throw_out_TTS_thought_message("YOU, the player, just ate a piece of Skeever Tail and received a damage stamina regen effect from it.    Say as much in your response, and make sure you make it clear that Skeever Tail simply causes damage stamina regeneration effect.");
		}
		return true;
	}

	if (std::strcmp(base_name, "Damage Stamina Regen") == 0 && std::strcmp(source_name, "Spider Silk") == 0) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for DAMAGE STAMINA REGEN effect application from Spider Silk!");
			DumpThoughts::throw_out_TTS_thought_message("YOU, the player, just consumed Spider Silk and received a damage stamina regen effect from it.    Say as much in your response, and make sure you make it clear that Spider Silk simply causes damage stamina regeneration effect.");
		}
		return true;
	}

	if (std::strcmp(base_name, "Reduce Orgasm Resistance") == 0 && std::strcmp(source_name, "Spider Silk") == 0) {
		if (a_event->isApplied) {
			SKSE::log::info("Event handler for REDUCE ORGASM RESISTANCE effect application from Ancient Seed!");
			DumpThoughts::throw_out_TTS_thought_message("YOU, the player, just consumed Ancient Seed and received a reduce orgasm resistance effect from it.    Say as much in your response, and make sure you make it clear that Ancient Seed simply causes reduce orgasm resistance effect.");
		}
		return true;
	}

	return false;
}