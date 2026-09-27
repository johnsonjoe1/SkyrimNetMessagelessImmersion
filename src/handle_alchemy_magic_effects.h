#pragma once

#include "RE/Skyrim.h"

class handle_alchemy_magic_effects
{
public:
	static bool handle_magic_effect(
		const RE::TESActiveEffectApplyRemoveEvent* a_event,
		RE::ActiveEffect* a_effect);
};