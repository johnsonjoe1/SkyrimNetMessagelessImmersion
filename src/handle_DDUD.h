#pragma once

#include "RE/Skyrim.h"

class handle_DDUD
{
public:
	static bool handle_DDUD_struggle_exhaustion_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
	static bool handle_DDUD_hood_magic_effect_stuff(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
};