#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class handle_SL_Survival
{
public:

	static void handle_sl_survival_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect);

};
