#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

class handle_DDUD
{
public:
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
	static bool handle_DDUD_hotkey_captured_and_stopped_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_skyrimnet_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_events(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_equipped_event(
		const SKSE::ModCallbackEvent* a_event);
	static bool handle_DDUD_device_removed_event(
		const SKSE::ModCallbackEvent* a_event);
};