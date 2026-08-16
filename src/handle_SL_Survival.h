#pragma once
#include <string>   
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

class handle_SL_Survival
{
public:
	static void handle_sl_survival_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect);
};
