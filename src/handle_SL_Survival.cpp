#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "misc.h"
#include "handle_SL_Survival.h"
#include <string.h>

namespace logger = SKSE::log;



void check_armor_for_bikini_property()
{
	// For our purposes, the actor is always the player character.
	RE::Actor* actor = RE::PlayerCharacter::GetSingleton();
    if (!actor) {
        return;
    }


	// Loop through inventory and list everything
    auto inventory = actor->GetInventory();
    logger::info("Currently worn items:");
    for (const auto& [item, entry] : inventory)
    {
        if (!entry.second->IsWorn()) {
            continue;
        }

		auto armor = item->As<RE::TESObjectARMO>();
        if (!armor) {
            continue;
        } 
		
		auto slots = armor->GetSlotMask();
        auto form = item;
        logger::info("  {} (FormID {:08X})  on slot 0x{:08X}",
            form->GetName(),
            form->GetFormID(),
            static_cast<std::uint32_t>(slots));
		PrintSlots(static_cast<std::uint32_t>(slots));

		for (std::uint32_t i = 0; i < armor->numKeywords; i++)
		{
			auto* keyword = armor->keywords[i];
			if (!keyword) {
				continue;
			}
			logger::info("      Keyword: {} ({:08X})", 				keyword->GetFormEditorID(), 				keyword->GetFormID());

			if ( 
				(strcmp(keyword->GetFormEditorID(), "_SLS_BikiniArmor") == 0)
			) {
				
				logger::info("      ======================>Found AND_AssCurtain keyword, setting global_ass_curtain_flag to true.");
			}
		}
    }
}






void handle_SL_Survival::handle_sl_survival_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect)
{
	SKSE::log::info("CHECKING FOR _SLS_STUFF!");
	logger::info("*********************SL-Survival-MAGIC-EFFECT-HANDLER STARTED!!!**********************************");
	// We got a magic effect.  We check once again everything, even though it has been checked in the calling function.

	auto* base = effect->GetBaseObject();
	auto* source = effect->spell;
	auto base_name = base->GetName();

	SKSE::log::info("The MAGIC EFFECT Source->GetName() of the SL-Survival magic effect gave:  {} with magnitude:  {}", source->GetName(), effect->magnitude);

	// Let's try to track SL Survival barefoot-slowdown-effect: 
	if (base && ( (std::strcmp(base->GetName(), "Barefoot") == 0)  ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for SL Survival Barefoot effect APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, are suddenly barefoot and the ground is full of little stones and sharp edges.  You can't run so quickly like that, in fact you can walk only slowly while being barefoot and it's painful for your sensible feet.  Say as much in your response.")); //  + standard_thought_instruction;
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			SKSE::log::info("Event handler for SL Survival Barefoot effect REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, were barefoot the whole time and had to walk slowly over the sharp and uncomfortable ground. But now you have shoes on again. This means you can finally move much faster again!  What a relief!  Say as much in your response.")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// Let's try to track SL Bikini Curse application/removal due to change of clothing: 
	if (base && ( (std::strcmp(base->GetName(), "Bikini Curse") == 0)  ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for SL Bikini Curse effect APPLICATION!");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, are suddenly being drained of all your stamina and have to stop and catch your breath all the time, because you are affected by the Bikini Curse.  The Bikini curse means, that whenever you wear anything that isn't bikini armour or regular clothes, you will be drained of all your stamina and will be out of breath all the time.  Say as much in your response.")); //  + standard_thought_instruction;
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			SKSE::log::info("Event handler for SL Bikini Curse effect REMOVAL!");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, were affected by the Bikini Curse the whole time and had to stop and catch your breath all the time. But now you are no longer affected by the curse. This means you can finally move and act normally again!  What a relief!  Say as much in your response.")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// Now try to track SL Bikini Curse out-of-breath-sequence: 
	if (base && ( (std::strcmp(base->GetName(), "_SLS_BikCurseShortBreathMgef") == 0)  ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for SL Bikini Curse out-of-breath-sequence APPLICATION!");
			// DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, are suddenly out of breath and have to stop.  All because you are affected by the Bikini curse that you are wearing something that isn't a bikini or heels that aren't high enough.  Say as much in your response.")); //  + standard_thought_instruction;
			// std::string sentence_with_non_bikini_items = get_bikini_curse_offending_sentence();
			check_armor_for_bikini_property();
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, are suddenly out of breath and have to stop.  All because you are affected by the Bikini curse.  The bikini curse makes it so that you are out of breath, whenever you are either wearing something that isn't a bikini or you are wearing heels that aren't high enough.  Say as much in your response and mention the 'bikini curse'.")); //  + standard_thought_instruction;
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			SKSE::log::info("Event handler for SL Bikini Curse out-of-breath-sequence REMOVAL!");  // the removal doesn't get it's own thought-statement, because that would be too much too quickly
			// DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, were affected by the Bikini Curse the whole time and had to stop and catch your breath all the time. But now you are no longer affected by the curse. This means you can finally move and act normally again!  What a relief!  Say as much in your response.")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	logger::info("*********************SL-SURVIVAL-MAGIC-EFFECT-HANDLER FINISHED!!!**********************************");
}
