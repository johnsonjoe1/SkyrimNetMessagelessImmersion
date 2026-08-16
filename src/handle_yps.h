#pragma once

#include <string>   
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

class handle_yps
{
public:
	static void handle_yps_fashion_detection_stuff();
	static void handle_yps_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect);
	static bool try_handle_yps_mod_stuff(const SKSE::ModCallbackEvent* a_event);
};

