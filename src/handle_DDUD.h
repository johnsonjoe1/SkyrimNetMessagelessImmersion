#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

class handle_DDUD
{
public:

	// FIRST all the active magic effects handlers.
	static bool handle_DDUD_abadon_domination_shout_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_struggle_exhaustion_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_hood_magic_effect_stuff(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_gag_magic_effect_stuff(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_restrictive_corset_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_chain_sound_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_muzzle_gag_ding_a_ling_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_plug_chargeable_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_inflatable_plug_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_hobble_skirt_forced_walk_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_lockpick_removal_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_thieving_removal_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_chastity_bra_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_rubber_dress_squeaking_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_active_magic_effects_from_various_devices(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);

	// THEN all the Mod Broadcast event handlers.
	static bool handle_DDUD_hotkey_captured_and_stopped_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_skyrimnet_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_mod_broadcast_events(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_equipped_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_removed_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_sentient_dialogue_event(
		const SKSE::ModCallbackEvent* a_event);
};