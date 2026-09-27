#include "handle_DDUD.h"

#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"

#include <cstring>
#include <string>

namespace
{
	bool try_handle_hood_effect(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* a_effect)
	{
		auto* base = a_effect->GetBaseObject();
		auto* source = a_effect->spell;
		if (!base || !source || std::strcmp(source->GetName(), "Hood Script") != 0) {
			return false;
		}

		if (std::strcmp(base->GetName(), "Blindfold Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood doesn't even let you see anything, so that you are completely blindfolded. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage hood, and that hood was keeping you completely blindfolded, but now you can see again. Say as much in your response, and be sure to make it clear, that you speak about the hood that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Blindfold-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		if (std::strcmp(base->GetName(), "Gag Food Removal Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood doesn't even let you eat or drink anything. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage hood, and that hood was keeping you from eating or drinking anything, but now you can eat and drink again. Say as much in your response, and be sure to make it clear, that you speak about the hood that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-Food-Removal-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		if (std::strcmp(base->GetName(), "Gag Script") == 0) {
			const std::string thought = a_event->isApplied ?
				"YOU, the player, just got looked into a hood, and that hood gags you completely so that you cannot utter a single word. Say as much in your response, and be sure to make it clear, that you speak about the hood that you are wearing now." :
				"YOU, the player, just escaped of of a locking bondage gag, and that gag was keeping you completely gagged, but now you can speak again. Say as much in your response, and be sure to make it clear, that you speak about the gag that you were wearing just moments ago.";
			DumpThoughts::throw_out_TTS_thought_message(std::string("Active Effect:Gag-through-Hood: ") + (a_event->isApplied ? "APPLIED-THOUGHT: " : "RELEASE-THOUGHT: ") + thought);
			LillithOnlyBox(thought);
			return true;
		}

		return std::strcmp(base->GetName(), "Muffling Script") == 0;
	}
}

bool handle_DDUD::handle_DDUD_struggle_exhaustion_effect(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	auto* base = a_effect->GetBaseObject();
	auto* source = a_effect->spell;
	if (!base || !source || std::strcmp(base->GetName(), "Exhaustion") != 0 ||
		std::strcmp(source->GetName(), "Struggle exhaustion") != 0) {
		return false;
	}

	if (a_event->isApplied) {
		SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION APPLICATION!");
		DumpThoughts::throw_out_TTS_thought_message(
			"YOU, the player, just tried getting out of your locking bondage devices for a whole while. You may have made some progress, but nevertheless now you are too exhausted to continue. Say as much in your response.");
	} else {
		SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION REMOVAL!");
		DumpThoughts::throw_out_TTS_thought_message(
			"YOU, the player, just were trying to get out of your locking bondage devices for a whole while. You may have made some progress, but in any case, that activity had made you exhausted to the point where you couldn't continue any more. But now time has passed and you're feeling better and you're good to go and maybe could continue trying. Say as much in your response.");
	}




    
	return true;
}

bool handle_DDUD::handle_DDUD_hood_magic_effect_stuff(
	const RE::TESActiveEffectApplyRemoveEvent* a_event,
	RE::ActiveEffect* a_effect)
{
	if (!a_event || !a_effect) {
		return false;
	}

	return try_handle_hood_effect(a_event, a_effect);
}