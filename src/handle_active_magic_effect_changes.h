#pragma once
#include <string>   //  ChatGPT suggested this might be needed?????

enum class AtaxiaStage
{
	none = -1,
	stage0 = 0,
	stage1 = 1,
	stage2 = 2
};

AtaxiaStage get_current_ataxia_stage();
void handle_changes_in_active_magic_effects( const RE::TESActiveEffectApplyRemoveEvent* a_event);
