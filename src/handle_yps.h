#pragma once

#include <string>   
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

extern bool actually_wearing_heels_according_to_yps_thoughts;

enum class YpsHeelTrainingStatus
{
	unknown = -1,
	untrained_feet = 0,
	high_heel_novice = 1,
	flexible_feet = 2,
	high_heel_walker = 3,
	arched_feet = 4,
	bondage_feet = 5
};

class handle_yps
{
public:
	static void handle_yps_fashion_detection_stuff();
	static void handle_yps_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect);
	static YpsHeelTrainingStatus get_current_heels_training_status();
	static bool try_handle_yps_mod_stuff(const SKSE::ModCallbackEvent* a_event);
	static void reset_hair_stage_tracking();
	static void reset_hair_dye_tracking();
	static void reset_fashion_tracking();
};
