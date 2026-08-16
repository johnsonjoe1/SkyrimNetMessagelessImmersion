#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include "handle_yps.h"
#include "handle_SL_Survival.h"
#include <string_view>
#include <unordered_set>
#include <vector>

namespace logger = SKSE::log;

static auto last_drool_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_muzzle_gag_ding_a_ling_sound_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

std::array<std::string, 2> list_of_food_contracted_sicknesses = {
    "Stomach Rot",
    "Food Poisoning"
};

std::array<std::string, 10> list_of_enemy_contracted_sicknesses = {
    "Ataxia",
    "Bone Break Fever",
    "Brain Rot",
    "Brown Rot",
    "Droops",
	"Greenspore",
	"Rattles",
	"Rockjoint",
	"Gutworm",
	"Witbane"
};

int IsAFoodBasedDisease(std::string_view keyword)
{
	for (std::size_t i = 0; i < list_of_food_contracted_sicknesses.size(); ++i)
	{
		if (keyword == list_of_food_contracted_sicknesses[i])
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

std::string_view ExtractCreatureNameFromEffectName(std::string_view effect_name)
{
	static constexpr std::string_view prefix = "Creature";
	static constexpr std::string_view suffix = "Effect";

	if (effect_name.size() <= (prefix.size() + suffix.size()))
	{
		return {};
	}

	if (effect_name.compare(0, prefix.size(), prefix) != 0)
	{
		return {};
	}

	if (effect_name.compare(effect_name.size() - suffix.size(), suffix.size(), suffix) != 0)
	{
		return {};
	}

	return effect_name.substr(prefix.size(), effect_name.size() - prefix.size() - suffix.size());
}
 
bool is_known_irrelevant_magic_effect(std::string base_name)
{
	static const std::vector<std::string> irrelevant_effect_list = {
		"RaceMenuHH Scale Effect"   , 
		"Consume Food Portion"   , 
		"Automate Hunger Script"  ,
		"Automate Thirst Script"  ,
		"SOS_Addon_PHF_Recolor" ,
		"SOS Actor Magic Effect" ,
		"Maintenance" ,
		"SCO_CellChangeDetectMgef" ,
		"SCO_CellChangeBegin",
		"Cell Tracking Effect",
		"Watch Cell",                 //  This one is from Slave Tats
		"Detect Cell Change Effect",  //  This one is from sztkUtil
		"UIWheelMenu_LoadMenu",
		"UIWheelMenu_SetOption",
		"UIWheelMenu_CloseMenu",
		"UIWheelMenu_ChooseOption",
		"CC NPCBimboCheckerCloakEffect",
		"lvskLoveSicknessVisibleEffect",  // This one is the pink-heart-eyes from the LoveSickness mod.  What are we to do with that?
		"Heart Eyes Effect",              // This is from lovesick mod, but it only affects eye textures and has no further relevance.
		"Euphoria",                       // This is from lovesick mod, but we only handle lovesick state 0 and 1 so far.
		"MilkRNDEffect",             // This is from MME, but I don't understand it.  Maybe it is some milk that was auto-drank from iNeed, but I don't know.
		"Adrenaline Script",         // This one is from iNeed, but I don't remember doing anything at that point.  Maybe it was auto-eating something with an effect.  But I don't know.
		"_STA_TearsCooldownMgef",    // This is interesting, because it's from spank-that-ass tears effect, but it seems to be too numerous and frequent to really be useful for anything at present
		"_STA_DialogOutputMgef",     // This is some spank-that-ass sex scene comments, but I guess SkyrimNet will do much better on it's own than those crude old comments
		"_STA_DroolCooldownMgef",     // This is interesting, because it's from spank-that-ass tears effect, but it seems to be too numerous and frequent to really be useful for anything at present
		"_SLS_WeaponReadyMgef",
		"_SLS_WeaponUnreadyMgef",
		"_SLS_CombatBeginMgef",
		"_SLS_CombatEndMgef",

		"BM_ME_DetectLocChange",  // The mod Licenses-Player Oppression checking for location changes, can be ignored.
		"BM_ME_DetectLocCity",  // The mod Licenses-Player Oppression checking for location changes, can be ignored.
		"BM_ME_PeriodicCheck",  // The mod Licenses-Player Oppression periodically doing something I presume.

		"BM_ME_HostArmorLicense",      // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostBikiniExemption",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostBikiniLicense",     // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostClothingLicense",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostCollarExemption",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostCraftingLicense",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostCurfewExemption",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostInsurance",         // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostMagicLicense",      // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostTradingLicense",    // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostTravelPermit",      // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostWeaponLicense",     // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.
		"BM_ME_HostWhoreLicense",      // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.

		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Slow",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Medium",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Fast",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   "Drool",   // This is from UD/DD/ZAD and from some gag, and COULD be used later.


		"Quest Start Routine",  // This is from Mod Sleep-in-Lingerie, and startup/initialization of the Mod.

		"Standing Moving Detector Effect"   // This is also from SL Survival and runs all the time, like every 2 to 5 seconds, so useless for our purposes here.
	};		
	for (std::size_t i = 0; i < irrelevant_effect_list.size(); ++i)
	{
		if (base_name == irrelevant_effect_list[i])
		{
			return true;
		}
	}
	return false;
};


// ****************************************************************************************************************
// This is the handling of Active Magic Effects and everything related to it.
class MyVisitor : public RE::MagicTarget::ForEachActiveEffectVisitor
{
public:
    RE::BSContainer::ForEachResult Accept(RE::ActiveEffect* effect) override
    {
        if (effect) {
            auto* base = effect->GetBaseObject();
            if (base) {
                // logger::info("Effect: {}", base->GetName());
				logger::info(
					"Effect ptr={} base={}",
					(void*)effect,
					base ? base->GetName() : "NULL"
				);
            }
        }
        return RE::BSContainer::ForEachResult::kContinue;
    }
};

class UIDMatchVisitor : public RE::MagicTarget::ForEachActiveEffectVisitor
{
public:
    UIDMatchVisitor(std::uint16_t uid) : targetUID(uid), found(nullptr)
    {}
	
    RE::BSContainer::ForEachResult Accept(RE::ActiveEffect* effect) override
    {
        if (!effect) {
            return RE::BSContainer::ForEachResult::kContinue;
        }
        // 🔑 THIS is the key comparison
        if (effect->usUniqueID == targetUID)
        {
            found = effect;
            auto* base = effect->GetBaseObject();
            if (base) {
				// logger::info("FROM UIDMatchVisitor we find:  Effect ptr={} base={}", (void*)effect, base ? base->GetName() : "NULL" );
            }
            return RE::BSContainer::ForEachResult::kStop;
        }
        return RE::BSContainer::ForEachResult::kContinue;
    }
    RE::ActiveEffect* GetResult() const
    {
        return found;
    }
private:
    std::uint16_t targetUID;
    RE::ActiveEffect* found;
};

void handle_changes_in_active_magic_effects( const RE::TESActiveEffectApplyRemoveEvent* a_event)
{
	// Protect from null pointer access, just in case.
	if (!a_event) {
		LillithOnlyBox("WOW! Null EVENT POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// Let's see what this event is about.  Who is the actor and what is the effect?		
	auto* targetRef = a_event->target.get();
	if (!targetRef) {
		LillithOnlyBox("WOW! Null TARGET POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	RE::Actor* actor;
	actor = targetRef->As<RE::Actor>();
	if (!actor) {
		LillithOnlyBox("WOW! There is NO ACTOR in this ACTIVE EVENT CHANGE!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// ✔ only care about player
	if (!actor->IsPlayerRef()) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	auto* magicTarget = actor->GetMagicTarget();
	auto* ref = a_event->target.get();

	// MyVisitor visitor;  // old Version had really no parameter
	UIDMatchVisitor visitor(a_event->activeEffectUniqueID);  // new version, we want to find the effect with the same UID in the current list of active effects.
	magicTarget->VisitEffects(visitor);
	auto* effect = visitor.GetResult();

	if (!effect)		
	{
		logger::info("No matching ActiveEffect found for UID {}", a_event->activeEffectUniqueID);
		LillithOnlyBox("WOW! AFTER RUNNING VisitEffects, we got NO MATCHING ACTIVE EFFECT FOUND for the current UID!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	auto* base = effect->GetBaseObject();
	if (!base)
	{
		LillithOnlyBox("WOW! THE BASE OBJECT IS NULL!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	auto our_form_id = base->GetFormID();
	auto base_name = base->GetName();
	auto caster = effect->caster.get();
	auto* source = effect->spell;
	// Let's inspect the FormID and hte Form behind the effect, to see if we can identify it.  
	auto* form = RE::TESForm::LookupByID(our_form_id);
	



	if (is_known_irrelevant_magic_effect(base_name))
	{
		logger::info("SKIPPING HANDLING OF IRRELEVANT MAGIC EFFECT: {}", base_name);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// We moved the SL-Survival-Stuff to the separate SL-Survival module
	if (base) {
		SKSE::log::info("CHECKING FOR _SLS_STUFF! Currently investigating: {}", base_name);
		if ( 
			(std::strcmp(base_name, "Barefoot") == 0)  || 
			(std::strcmp(base_name, "Bikini Curse") == 0) || 
			(std::strcmp(base_name, "_SLS_BikCurseShortBreathMgef") == 0) ) {

				LillithOnlyBox(std::format("CHECKING FOR _SLS_STUFF! Finally found something: {} and IsApplied= {}", base_name, a_event->isApplied));
				handle_SL_Survival::handle_sl_survival_magic_effect_stuff(a_event, effect);  // const RE::TESActiveEffectApplyRemoveEvent* a_event,
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
	}

	// We moved the YPS-Movement-Speed-Stuff to the separate YPS module
	if (base) {
		if (std::strcmp(base_name, "Movement Speed Penalty") == 0) {
			if ( (std::strcmp(source->GetName(), "High Heel Novice") == 0) || (std::strcmp(source->GetName(), "Untrained Feet") == 0) 
				|| (std::strcmp(source->GetName(), "Flexible Feet") == 0) || (std::strcmp(source->GetName(), "High Heel Walker") == 0) 
				|| (std::strcmp(source->GetName(), "Arched Feet") == 0) || (std::strcmp(source->GetName(), "Bondage Feet") == 0) )
			{
				handle_yps::handle_yps_magic_effect_stuff(a_event, effect);  // const RE::TESActiveEffectApplyRemoveEvent* a_event,
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
			}
		}
	}
	/* 	
	HeelTrainingStatusName[0] = "Untrained Feet"
	HeelTrainingStatusName[1] = "High Heel Novice"
	HeelTrainingStatusName[2] = "Flexible Feet"
	HeelTrainingStatusName[3] = "High Heel Walker"
	HeelTrainingStatusName[4] = "Arched Feet"
	HeelTrainingStatusName[5] = "Bondage Feet"  */

	// Let's also track the drunk-stumble-script:  It means the stumble-and-fall animation is playing, 
	// so we might as well say so.
	if ( (std::strcmp(base_name, "Drunk Stumble Script") == 0) && (a_event->isApplied) ){  	// Drunk Stumble Script
		// Here you can add your custom logic for when the stumble-and-fall animation is playing.
		DumpThoughts::throw_out_TTS_thought_message("YOU, the player, are so drunk, that you just lost balance and just stumbled and fell over you own feet from all the alcohol.  How does that make you feel?  What are you thinking now? ");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	} else {
		// logger::info("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx NOT THE DRUNK STUMBLE SCRIPT!");
	}
	// Let's try to track Stomach Rot here, which is a common effect that is applied when the player eats something rotten.  
	// It has a very specific magnitude and duration, so it should be easy to identify.
	if (base && ( IsAFoodBasedDisease(base_name) != -1) && (a_event->isApplied) )
	{
		std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for FOOD-BASED-DISEASE! ", base_name);
		LillithOnlyBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
		SKSE::log::info("Event handler for FOOD-BASED-DISEASE!");
		DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just ate something!  As a total surprise, you now notice, that you may have just contracted the so-called disease '{}' from it!  You need to announce the potential infection in your response, so that the actual player is informed.  You may do that implicitly, in the form of regret, surprise anger or shock.  It is a potentially dangerous condition. Be sure to mention the name of the disease '{}' in your response.  ", base_name, base_name)); //  + standard_thought_instruction;
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// The cure to rock joint desease looks like the comment below.  Lets say something.
	if (base && ( strcmp(base_name, "Rock Joint") == 0) && (! a_event->isApplied) )
	{
		std::string stomach_rot_status = std::format("CURE OF {} DISEASE DETECTED! ", base_name);
		LillithOnlyBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
		SKSE::log::info("Event handler for Rock Joint Disease!");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, just got cured of your {} Disease!  What a relief.  Your body has recoverd so quickly from the cure!  You need to announce great relief and successful cure!  You may do that implicitly, in the form of relief and gratitude.  Be sure to mention the name of the disease '{}' in your response.  ", base_name, base_name)); //  + standard_thought_instruction;
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
/*
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:336] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:337] Effect REMOVED on Lillith | UID=17
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:340] Base name: Rock Joint | Base ptr: 0x1a4f4647900 | Base-FormID: 1E00F0AC | Base-Form Type: 18   (This means: MGEF) 
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:341] base-Effect EDID: RND_DiseaseRockjoint | Source ptr: 0x1a4df640a00  |  Caster: None 
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:345] Magnitude: -1 | Duration: 0
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:348] Source name: Rock Joint | Source FormID: B8782 | Source EDID: DiseaseRockjoint 
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:354] Form LookupByID 1E00F0AC found: Rock Joint
*/


	// Let's try to track Unforgiving Devices Struggle Exhaustion here:  FIRST THE APPLICATION OF THE EFFECT.
	if (base && ( (std::strcmp(base_name, "Exhaustion") == 0)  ) && ( (std::strcmp(source->GetName(), "Struggle exhaustion") == 0)  ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just tried getting out of your locking bondage devices for a whole while. You may have made some progress, but nevertheless now you are too exhausted to continue.  Say as much in your response.")); //  + standard_thought_instruction;
		} 
		else  // i.e.  (!a_event->isApplied)
		{
			SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just were trying to get out of your locking bondage devices for a whole while. You may have made some progress, but in any case, that activity had made you exhausted to the point where you couldn't continue any more.  But now time has passed and you're feeling better and you're good to go and maybe could continue trying.  Say as much in your response.")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// Let's try to track UD/US Black-Goo-Application-Effect here:  FIRST THE APPLICATION OF THE EFFECT.
	if (base && ( (std::strcmp(base_name, "Device Manifest") == 0)  )  )
	{
		if (a_event->isApplied) 
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for BLACK-GOO-APPLICATION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("Event handler for BLACK-GOO-APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("Some substance called black goo just came in contact with you, and, to your horror, it manifested into a bondage device, thus trapping you as the victim now locked into said device.  What are you thinking in the face of this situation? ")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// Let's try to track UD/DD slowdown-effect from bondage boots: 
	if (base && ( (std::strcmp(base_name, "SpeedMult Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "BootSlow-Enchant") == 0)  ))
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for UD BONDAGE BOOTS SLOWDOWN APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just got locking bondage boots equipped onto your feet and you cannot take them off any more. But the important point is:  You cannot walk or run so fast any more with these heels equipped onto your feet! You will be slowed down for the whole time while wearing them (thus less able to run away from dangerious things)! Say as much in your response.")); //  + standard_thought_instruction;
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			SKSE::log::info("Event handler for UD BONDAGE BOOTS SLOWDOWN REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, had your feet locked into bondage boots the whole time and couldn't get them off. This has slowed you down the whole time. But now you got rid of the locking bondage devices on your feet. But the important point is:  This means you can finally move much faster again!  (And you won't trip over your feet any more.)  Say as much in your response.")); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}







	// Handle CreatureSummoner effects:
	// For any effect like CreatureBoarEffect, CreatureChaurusEffect, CreatureAshhopperEffect we handle it by producting a descriptive thought message containing that creature name.
	const auto creature_name = ExtractCreatureNameFromEffectName(base_name ? base_name : "");
	if (!creature_name.empty() && a_event->isApplied)
	{
		SKSE::log::info("Event handler for CREATURE {} EFFECT APPLICATION!", creature_name);
		DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just summoned a {}.  And the {} is a male {} and it seems to be horny too, looking for mates, and that includes you too!  Say as much in your response and make it clear that you speak about your freshly summoned {} in your response.", creature_name, creature_name, creature_name, creature_name)); //  + standard_thought_instruction;
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	
	if (base && ( (std::strcmp(base_name, "Irresistible Attraction") == 0)  ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for IRRESISTIBLE ATTRACTION EFFECT APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just became irresistibly attractive from some magic you used.  Others may be drawn to you and fuck you, or if not, you will just do it for yourself, because you are just to irresistile also to yourself!  Say as much in your response.")); //  + standard_thought_instruction;
		} 
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	if (base && ( (std::strcmp(base_name, "Teleport") == 0) && (std::strcmp(base->GetFormEditorID(), "aaaWCTeleportSpellEffect") == 0) ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for AAA WC TELEPORT SPELL effect application!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just used a teleport spell.  This one should get you right to the display hall, where all your Waifu Cards are collected.  Say as much in your response.")); //  + standard_thought_instruction;
		} 
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


	if (base && ( (std::strcmp(base_name, "Restraint Trap") == 0) ) )   // We already know, that this is about the player at this point, so no need to double-check!
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for RESTRAINT TRAP effect application!");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, were just hit by a bondage trap.  This is a major catastrophe.  The Trap has hit you and now you will be bound into restrictive bondage gear by the magic of the restraints trap!  In your response, you should cry out in desperation, that you have been hit by a bondage trap and will now suffer heavy bondage.  This event is so important, that you can elaborate in many words about your desperation here.")); //  + standard_thought_instruction;
		} 
		//  The REMOVAL happens immediately afterwards and doesn't need to be mentioned again.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

/*[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:419] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:420] Effect APPLIED on Lillith | UID=42
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:423] Base name: Drool | Base ptr: 0x1d0424ac2c0 | Base-FormID: 2415A4F2 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:424] base-Effect EDID: UD_ZAZDrool_ME | Source ptr: 0x1d042c4f200  |  Caster: Lillith 
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:428] Magnitude: 3 | Duration: 125
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:431] Source name: Drool | Source FormID: 2415A4F4 | Source EDID: UD_ZAZDroolSpell 
[2026-08-09 13:37:08.084] [log] [info] [handle_active_magic_effect_changes.cpp:437] Form LookupByID 2415A4F2 found: Drool*/
	if (base && ( (std::strcmp(base_name, "Drool") == 0) ) )   // We already know, that this is about the player at this point, so no need to double-check!
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Drool effect application!");

			// We implement a cooldown here, just to be safe.
			if (cooldown_has_passed(last_drool_thought_timestamp, 180))  // 180 seconds = 3 minutes cooldown
			{
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, are now drooling.  This is so humiliating.  It's probably because of the gag you are wearing or something similar.  In your response, you should cry out in desperation, that you are drooling uncontrollably.  This event is so important, that you can elaborate in many words about your desperation here.")); //  + standard_thought_instruction;				
				last_drool_thought_timestamp = std::chrono::steady_clock::now();
			} else {
				SKSE::log::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> But the cooldown in Event handler for Drool effect application hasn't passed yet!");
				return;
			}
		} 
		//  The REMOVAL happens immediately afterwards and doesn't need to be mentioned again.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

/*[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:419] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:420] Effect APPLIED on Lillith | UID=33
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:423] Base name: Muzzle Gag Ding-a-Ling Sounds Slow | Base ptr: 0x1d03e5c0d40 | Base-FormID: 110586B9 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:424] base-Effect EDID: zadx_SndMuzzleGagDingaLingSlowMgef | Source ptr: 0x1d03e1c1f00  |  Caster: Lillith 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:428] Magnitude: 0 | Duration: 0
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:431] Source name: Muzzle Gag Script | Source FormID: 110586B4 | Source EDID: zad_enchGagDingaLing 
[2026-08-09 13:38:20.753] [log] [info] [handle_active_magic_effect_changes.cpp:437] Form LookupByID 110586B9 found: Muzzle Gag Ding-a-Ling Sounds Slow*/
	if (base && ( (std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Slow") == 0) 
	|| (std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Medium") == 0) 
	|| (std::strcmp(base_name, "Muzzle Gag Ding-a-Ling Sounds Fast") == 0) ) )   // We already know, that this is about the player at this point, so no need to double-check!
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Muzzle Gag Ding-a-Ling Sounds effect application!");
			// We implement a cooldown here, just to be safe.
			if (cooldown_has_passed(last_muzzle_gag_ding_a_ling_sound_timestamp, 60*5))  //  5 minutes cooldown
			{
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, are now dangling a little bell, that is attached to your muzzle gag and keeps dingling little bell sounds whenever you move.  This is so humiliating.  It's all just because of the gag you are wearing.  In your response, you should cry out in desperation, that you are feeling so humiliated with that little bell, that you are unable to remove or muffle due to your bondage.  This event is so important, that you can elaborate in many words about your desperation here.")); //  + standard_thought_instruction;				
				last_muzzle_gag_ding_a_ling_sound_timestamp = std::chrono::steady_clock::now();
			} else {
				SKSE::log::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> But the cooldown in Event handler for Muzzle Gag Ding-a-Ling Sounds effect application hasn't passed yet!");
				return;
			}
		} 
		//  The REMOVAL happens immediately afterwards and doesn't need to be mentioned again.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

/*-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:419] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:420] Effect REMOVED on Lillith | UID=39
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:423] Base name: Covered In Cum | Base ptr: 0x1d03d86cb80 | Base-FormID: 9041477 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:424] base-Effect EDID: SexLabCumVaginalEffect | Source ptr: 0x1d03d723600  |  Caster: Lillith 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:428] Magnitude: 0 | Duration: 0
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:431] Source name: Sexual Encounter | Source FormID: 9041478 | Source EDID: SexLabCumVaginalSpell 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:437] Form LookupByID 9041477 found: Covered In Cum*/
	if (base && ( (std::strcmp(base_name, "Covered In Cum") == 0) && (std::strcmp(base->GetFormEditorID(), "SexLabCumVaginalEffect") == 0) ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Covered In Cum/SexLabCumVaginalEffect effect application!");
			std::string final_thought_string = std::format("Due to your sexual encounter, YOU, the player, now have fresh cum dripping from your vagina.  This is so humiliating and disgusting. Say as much in your response and be sure to mention that his sperm is now oozing out of your vagina.  This event is so important, that you can elaborate in many words about your desperation here.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		} else {
			SKSE::log::info("Event handler for Covered In Cum/SexLabCumVaginalEffect effect removal!");
			std::string final_thought_string = std::format("Due to your sexual encounter, YOU, the player, had fresh cum dripping from your vagina, up until now.  This was so humiliating and disgusting.  But now the cum dripping has stopped.  It probably all oozed out now.  Say as much in your response and be sure to mention that his sperm is stopped oozing out of your vagina now.  It is a little bit of a relief.  This event is so important, that you can elaborate in many words about your desperation here.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	/*[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:511] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:512] Effect APPLIED on Lillith | UID=31
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:515] Base name: Tears | Base ptr: 0x1643d92cb80 | Base-FormID: 2415A4EF | Base-Form Type: 18   (This means: MGEF) 
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:516] base-Effect EDID: UD_ZAZTears_ME | Source ptr: 0x1643e03f300  |  Caster: Lillith 
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:520] Magnitude: 3 | Duration: 50
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:523] Source name: Tears | Source FormID: 2415A4F0 | Source EDID: UD_ZAZTearsSpell 
[2026-08-14 16:40:34.172] [log] [info] [handle_active_magic_effect_changes.cpp:529] Form LookupByID 2415A4EF found: Tears
*/
	if (base && ( (std::strcmp(base_name, "Tears") == 0) && (std::strcmp(base->GetFormEditorID(), "UD_ZAZTearsSpell") == 0) ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Tears effect application!");
			std::string final_thought_string = std::format("After the emotional abuse you just felt, you feel something watery drip down your cheeks.  You ask yourself:  Oh my god, could this be tears?  Am I crying?  Say as much in your response and be sure to mention that you think it's tears rolling down your cheeks.  This event is so important, that you can elaborate in many words about your desperation here.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		} else {
			// When the tears have stopped, that isn't such a big issue to make an annoucement from that.  So we do nothing in this case.
			SKSE::log::info("Event handler for Tears effect removal!");
			std::string final_thought_string = std::format("After the emotional abuse you just felt, the tears on your cheeks have stopped.  You take a moment to compose yourself.  Say as much in your response and be sure to mention that the tears have stopped.  This event is so important, that you can elaborate in many words about your desperation here.");
			// LillithOnlyBox(final_thought_string);
			// DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


	//  CODE-MARKER:  THIS IS THE ENTRY POINT FOR MORE ACTIVE MAGIC EFFECTS TO BE HANDLED.

	




	logger::info("========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============");		
	SKSE::log::info("Effect {} on {} | UID={}", a_event->isApplied ? "APPLIED" : "REMOVED", actor->GetName(), a_event->activeEffectUniqueID);
	// logger::info("Effect ptr: {}", (void*)effect);
	// logger::info("UID: {}", a_event->activeEffectUniqueID);
	logger::info("Base name: {} | Base ptr: {} | Base-FormID: {:X} | Base-Form Type: {}   (This means: {}) ", base_name, (void*)base, our_form_id, (int)base->GetFormType(),   RE::FormTypeToString(base->GetFormType() ) );
	logger::info("base-Effect EDID: {} | Source ptr: {}  |  Caster: {} ", base->GetFormEditorID(), (void*)source, caster ? caster->GetName() : "None");
	// logger::info("Source pointer: {}", (void*)effect->spell);
	// logger::info("Source ptr: {}  |  Caster: {} ", (void*)source, caster ? caster->GetName() : "None");
	// Optional but very useful if available in your build:
	logger::info("Magnitude: {} | Duration: {}", effect->magnitude, effect->duration);
	// logger::info("Elapsed: {}", effect->elapsedTime);
	if (source) {
		logger::info("Source name: {} | Source FormID: {:X} | Source EDID: {} ", source->GetName(), source->GetFormID(), source->GetFormEditorID());
	} else {
		logger::info("No source spell for this effect.");
	}
	if (form)
	{
		logger::info("Form LookupByID {:X} found: {}", our_form_id, form->GetName());
	}
	else
	{
		logger::info("Form with ID {:X} not found.", our_form_id);
	}
	SKSE::log::info(".");
	SKSE::log::info(".");
	SKSE::log::info("ABOVE IS A POTENTIALLY UNHANDLED MAGIC EFFECT??? CHECK THE BASE NAME AND SOURCE NAME TO SEE IF IT'S SOMETHING YOU WANT TO REACT TO, OR IF IT'S SOME RANDOM EFFECT THAT YOU DON'T CARE ABOUT.  IF IT'S THE LATTER, THEN YOU PROBABLY WANT TO ADD A NEW IF-STATEMENT FOR THIS EFFECT IN THIS HANDLER, SO THAT IT DOESN'T GET LOGGED IN SUCH DETAIL ANY MORE, BECAUSE THAT WOULD BE ANNOYING.  CHECK THE BASE NAME AND SOURCE NAME TO SEE WHAT EFFECT THIS IS ABOUT.  IF IT'S AN EFFECT YOU CARE ABOUT, THEN CONSIDER ADDING A CUSTOM MESSAGE FOR IT IN THIS HANDLER, SO THAT YOUR TTS CAN REACT TO IT IN A MEANINGFUL WAY! ");
}
