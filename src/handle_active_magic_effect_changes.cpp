#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include "handle_DDUD.h"
#include "handle_alchemy_magic_effects.h"
#include "handle_yps.h"
#include "handle_SL_Survival.h"
#include "handle_active_magic_effect_changes.h"
#include <algorithm>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace logger = SKSE::log;

static auto last_drool_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_disease_cure_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_cum_effect_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_cum_effect_removal_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_entered_water_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_exited_water_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_firebolt_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_swimming_effect_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

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

std::array<std::string, 12> list_of_all_sicknesses = {
    "Ataxia",
    "Bone Break Fever",
    "Brain Rot",
    "Brown Rot",
    "Droops",
	"Greenspore",
	"Rattles",
	"Rockjoint",
	"Gutworm",
	"Witbane",
    "Stomach Rot",
    "Food Poisoning"	
};

AtaxiaStage get_current_ataxia_stage()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return AtaxiaStage::none;
	}

	auto has_spell = [player](std::string_view editorID) {
		auto* spell = RE::TESForm::LookupByEditorID<RE::SpellItem>(editorID);
		return spell && player->HasSpell(spell);
	};

	if (has_spell("RND_DiseaseAtaxiaStage2")) {
		return AtaxiaStage::stage2;
	}
	if (has_spell("RND_DiseaseAtaxiaStage1")) {
		return AtaxiaStage::stage1;
	}
	if (has_spell("RND_DiseaseAtaxiaStage0")) {
		return AtaxiaStage::stage0;
	}

	return AtaxiaStage::none;
}


std::vector<std::string> get_current_other_sickness_thoughts()
{
	std::vector<std::string> thoughts;
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return thoughts;
	}

	struct SevereDisease
	{
		std::string_view spellEditorID;
		std::string_view name;
		std::string_view symptoms;
	};
	static constexpr std::array<SevereDisease, 5> severeDiseases{{
		{ "RND_DiseaseBoneBreakFeverStage2", "Bone Break Fever", "The disease reduces your carrying capacity, slows your movement, and makes your attacks deal less damage." },
		{ "RND_DiseaseBrainRotStage2", "Brain Rot", "The disease reduces your Magicka and makes your spells much less effective." },
		{ "RND_DiseaseRattlesStage2", "Rattles", "The disease slows the regeneration of your Stamina and Magicka." },
		{ "RND_DiseaseRockjointStage2", "Rockjoint", "The disease reduces your Stamina and carrying capacity." },
		{ "RND_DiseaseWitbaneStage2", "Witbane", "The disease reduces your Magicka and makes learning new skills slower." }
	}};
	for (const auto& disease : severeDiseases) {
		auto* spell = RE::TESForm::LookupByEditorID<RE::SpellItem>(disease.spellEditorID);
		if (spell && player->HasSpell(spell)) {
			thoughts.push_back(std::format("You are suffering from the most severe stage of {}. {} You need a cure for this disease. Say so in your response and explicitly name {} as the cause of these symptoms.", disease.name, disease.symptoms, disease.name));
		}
	}

	// These sicknesses have no progressive stage spells in RND.
	class SicknessVisitor : public RE::MagicTarget::ForEachActiveEffectVisitor
	{
	public:
		RE::BSContainer::ForEachResult Accept(RE::ActiveEffect* effect) override
		{
			if (!effect || effect->flags.any(RE::ActiveEffect::Flag::kInactive, RE::ActiveEffect::Flag::kDispelled)) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			auto* base = effect->GetBaseObject();
			if (!base) {
				return RE::BSContainer::ForEachResult::kContinue;
			}
			const std::string_view name = base->GetName();
			if (name == "Brown Rot" || name == "Droops" || name == "Greenspore" || name == "Gutworm" || name == "Stomach Rot" || name == "Food Poisoning") {
				names.emplace(name);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		}

		std::unordered_set<std::string> names;
	};
	SicknessVisitor visitor;
	if (auto* target = player->GetMagicTarget()) {
		target->VisitEffects(visitor);
	}
	for (const auto& name : list_of_all_sicknesses) {
		if (visitor.names.contains(name)) {
			thoughts.push_back(std::format("You are currently suffering from the disease {} and still need a cure. Describe how you feel about being ill and needing treatment. Explicitly mention {} so the reason for your thought is clear.", name, name));
		}
	}
	return thoughts;
}

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

		"DS Cell Tracker",			   // This is Devious Strike cell tracker and has no implications for us.

		"BM_ME_PeriodicCheck",         // The mod Licenses-Player Oppression periodically doing something I presume.
		"BM_ME_DetectLocChange",       // The mod Licenses-Player Oppression checking for location changes, can be ignored.
		"BM_ME_DetectLocCity",         // The mod Licenses-Player Oppression checking for location changes, can be ignored.
		"BM_ME_DetectLocTown",         // The mod Licenses-Player Oppression checking for location changes, can be ignored.
		"BM_ME_DetectStateWorkbench",  // The mod Licenses-Player Oppression checking for workbench state changes, can be ignored.
		"BM_ME_DetectItemWeaponOut",   // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.		
		"BM_ME_DetectStateJail",       // The mod Licenses-Player Oppression checking for jail state changes, can be ignored.
		"BM_ME_DetectItemMagicOut",    // The mod Licenses-Player:  Seems to be regular checks again, which we can't do anything with, really.

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


		"BF SetInvulnerable Effect",            // This is *PROBABLY* from Battlefuck, but I'm not sure
		"BF Call Follower For Help Effect",     // This is *PROBABLY* from Battlefuck, but I'm not sure
		"BF Combat Player Effect",              // This is *PROBABLY* from Battlefuck, but I'm not sure
		"BF Stop Combat Effect",                // This is *PROBABLY* from Battlefuck, but I'm not sure

		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Slow",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Medium",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   ""Muzzle Gag Ding-a-Ling Sounds Fast",  // This is from UD/DD/ZAD and probably triggers very time the bell from the muzzle-gag sounds.  It is too often outright, but with a cooldown, we could add some thoughts here to, about the annoying cute sound.
		// NOW HANDLED:   "Drool",   // This is from UD/DD/ZAD and from some gag, and COULD be used later.


		"Quest Start Routine",     // This is from Mod Sleep-in-Lingerie, and startup/initialization of the Mod.

		"Standing Moving Detector Effect",   // This is also from SL Survival and runs all the time, like every 2 to 5 seconds, so useless for our purposes here.

		"Align To Furniture Effect"   // This is from captive player, and is probably more an internal thing, so no need to do anything with that.
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

	// For the removal of a disease, we can simply handle them all in the same way.  At least for now.
	if (base && std::find(list_of_all_sicknesses.begin(), list_of_all_sicknesses.end(), base_name) != list_of_all_sicknesses.end() && !a_event->isApplied)
	{
		// NOTE:  We add a cooldown here, because stage2, stage1 and stage0 are all removed separately, so we would trigger 3 messages, if
		//        the disease was at RND (realistic-needs-and-diseases-mod) stage2 already.  A finer handling in the future might mention the
		//        specific stage that was removed, but for now we just want to avoid spamming the player with multiple messages in quick succession.
		if (!cooldown_has_passed(last_disease_cure_thought_timestamp, 10))
		{
			return;
		}

		std::string stomach_rot_status = std::format("CURE OF {} DISEASE DETECTED! ", base_name);
		LillithOnlyBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
		SKSE::log::info("Event handler for ALL DISEASED BEING REMOVED, i.e CURED!");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, just got cured of your {} Disease!  What a relief.  Your body has recoverd so quickly from the cure!  You need to announce great relief and successful cure!  You may do that implicitly, in the form of relief and gratitude.  Be sure to mention the name of the disease '{}' in your response.  ", base_name, base_name)); //  + standard_thought_instruction;
		last_disease_cure_thought_timestamp = std::chrono::steady_clock::now();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

/*[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:337] Effect REMOVED on Lillith | UID=17
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:340] Base name: Rock Joint | Base ptr: 0x1a4f4647900 | Base-FormID: 1E00F0AC | Base-Form Type: 18.
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:341] base-Effect EDID: RND_DiseaseRockjoint | Source ptr: 0x1a4df640a00  |  Caster: None 
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:345] Magnitude: -1 | Duration: 0
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:348] Source name: Rock Joint | Source FormID: B8782 | Source EDID: DiseaseRockjoint 
[2026-06-28 21:04:22.010] [log] [info] [handle_active_magic_effect_changes.cpp:354] Form LookupByID 1E00F0AC found: Rock Joint
...
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:757] Effect APPLIED on Non-Lillith | UID=38
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:760] Base name: Ataxia | Base ptr: 0x2a5f1608d40 | Base-FormID: 1E010672 | Base-Form Type: 18.
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:761] base-Effect EDID: RND_DiseaseAtaxiaStage1Effect | Source ptr: 0x2a5e7609a00  |  Caster: Non-Lillith 
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:765] Magnitude: -25 | Duration: 0
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:768] Source name: Ataxia | Source FormID: 1E00A530 | Source EDID: RND_DiseaseAtaxiaStage1 
[2026-09-06 15:21:07.321] [log] [info] [handle_active_magic_effect_changes.cpp:774] Form LookupByID 1E010672 found: Ataxia
*/
	// Let's try to track ALL combat-related diseases here.  
	if (base && std::find(list_of_enemy_contracted_sicknesses.begin(), list_of_enemy_contracted_sicknesses.end(), base_name) != list_of_enemy_contracted_sicknesses.end() && a_event->isApplied)
	{
		std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for ENEMY-CONTRACTED-DISEASE! ", base_name);
		LillithOnlyBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
		SKSE::log::info("Event handler for ENEMY-CONTRACTED-DISEASE!");

		if (std::string_view(base->GetFormEditorID()).find("Stage2") != std::string_view::npos)
		{
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, have now reached the most severe stage of '{}' disease!  The effects are overwhelming, and you are feeling extremely sick.  You already feel the heaviest symptoms.  This could maybe end your life, if you don't manage to get treated in time.  Say so in your response, and make sure you mention the name of the disease '{}' as well as make clear fact that this *is* a disease!  You need to announce the potential infection in your response, so that the actual player is informed.  This is so important, that you can use more words than usual for that.", base_name, base_name));
			RE::DebugMessageBox("DISEASE HANDLER STAGE 2!!");
			if (std::strcmp(base_name, "Ataxia") == 0) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("The disease has reached the worst state. That means from now on, you cannot move so fast and it also means you do less damage to enemies.  Say so in your response and make sure you mention both effects of the disease."));
			}	
		} else if (std::string_view(base->GetFormEditorID()).find("Stage1") != std::string_view::npos)
		{
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, have now been infected with '{}' disease for quite a while.  And the effects of the disease are now suddenly getting worse!  You are now feeling much worse from the sickening effect.  You already feel heavy symptoms.  Say so in your response, and make sure you mention the name of the disease '{}' as well as make clear fact that this *is* a disease!  You need to announce the potential infection in your response, so that the actual player is informed.  This is so important, that you can use more words than usual for that.", base_name, base_name));
			RE::DebugMessageBox("DISEASE HANDLER STAGE 1!!");	
			if (std::strcmp(base_name, "Ataxia") == 0) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("The disease is making it not only harder for you to do lockpicking and  pickpocketing, but at this stage of the disease, it also becomes harder to sneak and to carry so much weight.  Say so in your response."));
			}				
		} else 
		{
			// This must be the "mild" stage.  This ISNT MODIFIED BY THE ADD-ONS and sometimes just base vanilla, so it seems there can be cases where the expected 'stage0' doesn't even appear in the effect description.  Therefore we handle this via the "else" clause.				
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, just got infected with '{}' disease!  This must be something you contracted during combat with an infected creature.  You are already starting to feel the sickening effect.  You already feel the symptoms.  Say so in your response, and make sure you mention the name of the disease '{}' as well as make clear fact that this *is* a disease!  You need to announce the potential infection in your response, so that the actual player is informed.  This is so important, that you can use more words than usual for that.", base_name, base_name));
			RE::DebugMessageBox("DISEASE HANDLER STAGE 0!!");   
			if (std::strcmp(base_name, "Ataxia") == 0) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("The disease is making it harder for you to do lockpicking and pickpocketing.  Say so in your response."));
			}
		}

		// NOTE:  FOR THE MOMENT WE LET THIS EVENT RUN, SO THAT WE SEE MORE PARATEMERS FROM IT IN THE LOG.
		// return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// list_of_enemy_contracted_sicknesses

	if (handle_DDUD::handle_DDUD_struggle_exhaustion_effect(a_event, effect)) {
		return;
	}

	if (handle_DDUD::handle_DDUD_hood_magic_effect_stuff(a_event, effect)) {
		return;
	}


	if (handle_DDUD::handle_DDUD_gag_magic_effect_stuff(a_event, effect)) {
		return;
	}

	if (handle_DDUD::handle_DDUD_restrictive_corset_effect(a_event, effect)) {
		return;
	}

	if (handle_DDUD::handle_DDUD_chain_sound_effect(a_event, effect)) {
		return;
	}

	if (handle_DDUD::handle_DDUD_muzzle_gag_ding_a_ling_effect(a_event, effect)) {
		return;
	}

	if (handle_alchemy_magic_effects::handle_magic_effect(a_event, effect)) {
		return;
	}






/*[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:536] Effect REMOVED on Lillith | UID=30
[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:539] Base name: Whipped | Base ptr: 0x20bc785fc80 | Base-FormID: 2C0518F7 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:540] base-Effect EDID: _ddCaptivePLWhippedEffect | Source ptr: 0x20bc7701000  |  Caster: Lillith 
[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:544] Magnitude: 100 | Duration: 5
[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:547] Source name: Whipped | Source FormID: 2C0518F6 | Source EDID: _ddCaptivePLWhipped 
[2026-08-16 17:56:42.354] [log] [info] [handle_active_magic_effect_changes.cpp:553] Form LookupByID 2C0518F7 found: Whipped*/
	if (base && ( (std::strcmp(base_name, "Whipped") == 0)  ) && ( (std::strcmp(source->GetName(), "Whipped") == 0)  ))
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Whipped APPLICATION!");
			std::string final_thought_string = std::format("YOU, the player, are getting whipped while bound and helpless from the captive player mod.  Express the horrible pain of being whipped while helplessly tied up and unable to escape.  Since this is a major source of suffering, you can elaborate in more words about this."); //  + standard_thought_instruction;
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: Whipped: THOUGHT: " + final_thought_string);
			LillithOnlyBox(final_thought_string);
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			// Do nothing here, since we only explain the effect when it is applied, that this is a special propery of the current corset.
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
/*[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:536] Effect APPLIED on Lillith | UID=30
[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:539] Base name: Drugged | Base ptr: 0x20bc76fa240 | Base-FormID: 2C00082C | Base-Form Type: 18   (This means: MGEF) 
[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:540] base-Effect EDID: _ddCaptivePLDruggedEffect | Source ptr: 0x20e3ed63b80  |  Caster: Lillith 
[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:544] Magnitude: 0 | Duration: 1200
[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:547] Source name: Arousal Drug | Source FormID: 2C00082A | Source EDID: _ddArousalDrug 
[2026-08-16 17:49:20.090] [log] [info] [handle_active_magic_effect_changes.cpp:553] Form LookupByID 2C00082C found: Drugged */
	if (base && ( (std::strcmp(base_name, "Drugged") == 0)  ) && ( (std::strcmp(source->GetName(), "Arousal Drug") == 0)  ))
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Drugged APPLICATION!");
			std::string final_thought_string = std::format("YOU, the player character, just got drugged with some arousal drug or aphrodisiac while bound and helpless in a prison scene from the captive player mod.  Explain to the player, that you have just been drugged with an arousal drug.  Also express the effects of the drug you are feeling, while helplessly tied up and unable to escape.  Since this is a major source of suffering, you can elaborate in more words about this."); //  + standard_thought_instruction;
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: Drugged: THOUGHT: " + final_thought_string);
			LillithOnlyBox(final_thought_string);
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			// Do nothing here, since we only explain the effect when it is applied, that this is a special propery of the current corset.
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
/*[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:536] Effect APPLIED on Lillith | UID=40
[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:539] Base name: Low Health Effect | Base ptr: 0x20bc785f040 | Base-FormID: 2C07505C | Base-Form Type: 18   (This means: MGEF) 
[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:540] base-Effect EDID: _ddCaptivePLLowHealthEffect | Source ptr: 0x20bc7701e00  |  Caster: Lillith 
[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:544] Magnitude: 100 | Duration: 100
[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:547] Source name: Low Health | Source FormID: 2C07505B | Source EDID: _ddCaptivePLLowHealth 
[2026-08-16 17:55:39.017] [log] [info] [handle_active_magic_effect_changes.cpp:553] Form LookupByID 2C07505C found: Low Health Effect*/
	if (base && ( (std::strcmp(base_name, "Low Health Effect") == 0)  ) && ( (std::strcmp(source->GetName(), "Low Health") == 0)  ))
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Low Health Effect APPLICATION!");
			std::string final_thought_string = std::format("YOU, the player character, just got the Low Health Effect applied from the captive player mod.  That means you are going down like almost in a knockout, which gives your opponents a chance to find you and capture you.  Explain to the player, that your health is critically low and that you opponents might capture you if you don't manage to recover quickly soon.  Since this is a major source of suffering, you can elaborate in more words about this."); //  + standard_thought_instruction;
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: Low Health Effect: THOUGHT: " + final_thought_string);
			LillithOnlyBox(final_thought_string);
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			// Do nothing here, since we only explain the effect when it is applied, that this is a special propery of the current corset.
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

/*
[2026-09-30 21:56:10.622] [log] [info] [handle_active_magic_effect_changes.cpp:873] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-30 21:56:10.622] [log] [info] [handle_active_magic_effect_changes.cpp:874] Effect APPLIED on Lillith | UID=40
[2026-09-30 21:56:10.623] [log] [info] [handle_active_magic_effect_changes.cpp:877] Base name: Resist Disease | Base ptr: 0x23c6ec6acc0 | Base-FormID: FBFF5 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-30 21:56:10.623] [log] [info] [handle_active_magic_effect_changes.cpp:878] base-Effect EDID:  | Source ptr: 0x23c6ebb3900  |  Caster: Lillith 
[2026-09-30 21:56:10.623] [log] [info] [handle_active_magic_effect_changes.cpp:882] Magnitude: 25 | Duration: 28800
[2026-09-30 21:56:10.623] [log] [info] [handle_active_magic_effect_changes.cpp:885] Source name: Blessing of Talos | Source FormID: FB99A | Source EDID:  
[2026-09-30 21:56:10.623] [log] [info] [handle_active_magic_effect_changes.cpp:891] Form LookupByID FBFF5 found: Resist Disease
*/
	if (base && ( (std::strcmp(base_name, "Resist Disease") == 0)  ) && ( (std::strcmp(source->GetName(), "Blessing of Talos") == 0)  ))
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Resist Disease via Blessing of Thalos APPLICATION!");
			std::string final_thought_string = std::format("YOU, the player character, just received the blessing of Thalos. This gives you improved resistance to disease for the day (technically for 8 hours).  Respond in character and let the player know how you feel about that, but be sure to mention you improved disease resistance thanks to the blessing, because otherwise the player won't know what you are talking about."); //  + standard_thought_instruction;
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: Resist Disease via Blessing of Thalos: THOUGHT: " + final_thought_string);
			LillithOnlyBox(final_thought_string);
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			// Do nothing here, since we only explain the effect when it is applied, that this is a special propery of the current corset.
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	/*
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:873] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:874] Effect REMOVED on Lillith | UID=36
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:877] Base name: Get Condition: Swimming | Base ptr: 0x239a1818480 | Base-FormID: FE019033 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:878] base-Effect EDID:  | Source ptr: 0x23c59d42e00  |  Caster: Lillith 
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:882] Magnitude: 0 | Duration: 0
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:885] Source name: Get Dirty Over Time - Clean | Source FormID: FE01903A | Source EDID:  
[2026-09-30 22:14:56.866] [log] [info] [handle_active_magic_effect_changes.cpp:891] Form LookupByID FE019033 found: Get Condition: Swimming
	*/
	if (base && ( (std::strcmp(base_name, "Get Condition: Swimming") == 0)  ) && ( (std::strcmp(source->GetName(), "Get Dirty Over Time - Clean") == 0)  ))
	{
		if (a_event->isApplied)
		{
			if (!cooldown_has_passed(last_swimming_effect_thought_timestamp, 5*60)) {
				logger::info("Skipping swimming thought because its 5-minute cooldown has not elapsed.");
				return;
			}

			SKSE::log::info("Event handler for Get Condition: Swimming via Get Dirty Over Time - Clean APPLICATION!");
			std::string final_thought_string = std::format("YOU, the player character, just started a little swim, which is helping to remove some of the dirt you have been aquiring. Respond in character and mention your relief about the little swim, how good it feels and how nice it is, that this will remove even more of the dirt and filth you have been aquiring."); //  + standard_thought_instruction;
			DumpThoughts::throw_out_TTS_thought_message("Active Effect: Get Condition: Swimming via Get Dirty Over Time - Clean: THOUGHT: " + final_thought_string);
			LillithOnlyBox(final_thought_string);
			last_swimming_effect_thought_timestamp = std::chrono::steady_clock::now();
		} 
		else // i.e.  if (!a_event->isApplied) )
		{
			// Do nothing here, since we only explain the effect when it is applied, that this is a special propery of the current corset.
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


		/*  This is probably just a standard effect after sleeping a lot
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:845] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:846] Effect APPLIED on Lillith | UID=28
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:849] Base name: Not Tired | Base ptr: 0x192042c7e40 | Base-FormID: 695D2184 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:850] base-Effect EDID:  | Source ptr: 0x192041ba680  |  Caster: Lillith 
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:854] Magnitude: 0 | Duration: 1440
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:857] Source name: Not Tired | Source FormID: 695D21AA | Source EDID:  
[2026-09-14 21:52:01.405] [log] [info] [handle_active_magic_effect_changes.cpp:863] Form LookupByID 695D2184 found: Not Tired  */
	if (base && ( (std::strcmp(base_name, "Not Tired") == 0) || std::strcmp(base_name, "Slept 8 hours") == 0 )  )
	{
		if (a_event->isApplied) 
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for Not Tired-EFFECT-APPLICATION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("Event handler for Not Tired effect application!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("You just received the Not Tired effect.  Probably because you just slept long and good.  How do you feel about this change in your state?  Be sure to mention, that you are not tired any more.")); //  + standard_thought_instruction;
		}
		// No thought on removal of that, because there are more interesting stages of tiredness when real fatigue sets in.
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
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("YOU, the player, are now drooling.  It's probably because of the gag you are wearing or something similar.  In your response, you should mention, that you are drooling uncontrollably.  This event is so important, that you can elaborate in many words about your situation here.  Your response should match your overall character and can be positive or negative.")); //  + standard_thought_instruction;				
				last_drool_thought_timestamp = std::chrono::steady_clock::now();
			} else {
				SKSE::log::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> But the cooldown in Event handler for Drool effect application hasn't passed yet!");
				return;
			}
		} 
		//  The REMOVAL happens immediately afterwards and doesn't need to be mentioned again.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


/*
========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:420] Effect REMOVED on Lillith | UID=39
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:423] Base name: Covered In Cum | Base ptr: 0x1d03d86cb80 | Base-FormID: 9041477 | Base-Form Type: 18   (This means: MGEF) 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:424] base-Effect EDID: SexLabCumVaginalEffect | Source ptr: 0x1d03d723600  |  Caster: Lillith 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:428] Magnitude: 0 | Duration: 0
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:431] Source name: Sexual Encounter | Source FormID: 9041478 | Source EDID: SexLabCumVaginalSpell 
[2026-08-09 13:37:47.489] [log] [info] [handle_active_magic_effect_changes.cpp:437] Form LookupByID 9041477 found: Covered In Cum
.
========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1039] Effect APPLIED on Lillith | UID=12
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1042] Base name: SexLab Cum Effect (Main) | Base ptr: 0x23beb1add00 | Base-FormID: 90CD86D | Base-Form Type: 18   (This means: MGEF) 
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1043] base-Effect EDID:  | Source ptr: 0x23beb097a00  |  Caster: Lillith 
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1047] Magnitude: 0 | Duration: 0
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1050] Source name: Sexual Encounter | Source FormID: 90CD86F | Source EDID:  
[2026-09-25 12:47:18.258] [log] [info] [handle_active_magic_effect_changes.cpp:1056] Form LookupByID 90CD86D found: SexLab Cum Effect (Main)
.
========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1039] Effect APPLIED on Lillith | UID=35
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1042] Base name: Covered In Cum | Base ptr: 0x23beb1ae240 | Base-FormID: 90434D0 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1043] base-Effect EDID:  | Source ptr: 0x23beb097b00  |  Caster: Lillith 
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1047] Magnitude: 0 | Duration: 0
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1050] Source name: Sexual Encounter | Source FormID: 90434D5 | Source EDID:  
[2026-09-25 12:47:18.274] [log] [info] [handle_active_magic_effect_changes.cpp:1056] Form LookupByID 90434D0 found: Covered In Cum

*/
	// if (base && ( (std::strcmp(base_name, "Covered In Cum") == 0) && (std::strcmp(base->GetFormEditorID(), "SexLabCumVaginalEffect") == 0) ) )
	if (base && ( (std::strcmp(base_name, "Covered In Cum") == 0) || (std::strcmp(base_name, "SexLab Cum Effect (Main)") == 0) ) )
	{
		// We implement just one handler for all of these cum-effect-applied and cum-effect-removed events.
		if (a_event->isApplied)
		{
			if (!cooldown_has_passed(last_cum_effect_thought_timestamp, 60))
			{
				return;
			}
			SKSE::log::info("Event handler for Covered In Cum/SexLabCumVaginalEffect effect application!");
			std::string final_thought_string = std::format("Due to your sexual encounter, YOU, the player, now have fresh cum dripping from your body.  Say as much in your response and be sure to mention that his sperm is now oozing from your body.  This event is so important, that you can elaborate in many words about your situation here.  Your response should match your overall character and can be positive or negative.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
			last_cum_effect_thought_timestamp = std::chrono::steady_clock::now();
		} else {
			if (!cooldown_has_passed(last_cum_effect_removal_thought_timestamp, 60))
			{
				return;
			}
			SKSE::log::info("Event handler for Covered In Cum/SexLabCumVaginalEffect effect removal!");
			std::string final_thought_string = std::format("Due to your sexual encounter, YOU, the player, had fresh cum dripping from your body, up until now.  But now the cum dripping has stopped.  It probably all oozed out now.  Say as much in your response and be sure to mention that his sperm stopped oozing from your body now.  This event is so important, that you can elaborate in many words about your situation here.  Your response should match your overall character and can be positive or negative.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
			last_cum_effect_removal_thought_timestamp = std::chrono::steady_clock::now();
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
			std::string final_thought_string = std::format("After the emotional abuse you just felt, you feel something watery drip down your cheeks.  You ask yourself:  Oh my god, could this be tears?  Am I crying?  Say as much in your response and be sure to mention that you think it's tears rolling down your cheeks.  This event is so important, that you can elaborate in many words about your situation here.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		} else {
			// When the tears have stopped, that isn't such a big issue to make an annoucement from that.  So we do nothing in this case.
			SKSE::log::info("Event handler for Tears effect removal!");
			std::string final_thought_string = std::format("After the emotional abuse you just felt, the tears on your cheeks have stopped.  You take a moment to compose yourself.  Say as much in your response and be sure to mention that the tears have stopped.  This event is so important, that you can elaborate in many words about your situation here.");
			// LillithOnlyBox(final_thought_string);
			// DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	/*
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1071] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1072] Effect APPLIED on Lillith | UID=1
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1075] Base name: Is In Water Script | Base ptr: 0x272d014e400 | Base-FormID: 2B08AC45 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1076] base-Effect EDID:  | Source ptr: 0x272d0238400  |  Caster: Lillith 
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1080] Magnitude: 0 | Duration: 0
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1083] Source name: iNeed | Source FormID: 2B000D63 | Source EDID:  
[2026-09-26 09:17:19.628] [log] [info] [handle_active_magic_effect_changes.cpp:1089] Form LookupByID 2B08AC45 found: Is In Water Script
	*/	
	if (base && ( (std::strcmp(base_name, "Is In Water Script") == 0) ) )
	{
		if (a_event->isApplied)
		{
			if (!cooldown_has_passed(last_entered_water_thought_timestamp, 60*5))
			{
				return;
			}
			SKSE::log::info("Event handler for Is In Water Script effect application!");
			std::string final_thought_string = std::format("You have entered the water.  It may be quite cold and fresh or warm.  Respond in character and say something and be sure to mention that you are now in the water.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
			last_entered_water_thought_timestamp = std::chrono::steady_clock::now();
		} else {
			if (!cooldown_has_passed(last_exited_water_thought_timestamp, 60*5))
			{
				return;
			}
			if (!cooldown_has_passed(last_entered_water_thought_timestamp, 60))
			{
				// If it was only a very brief dip into the water, we don't want to trigger the separate exit thought from that.
				return;
			}
			SKSE::log::info("Event handler for Is In Water Script effect removal!");
			std::string final_thought_string = std::format("You have exited the water.  It may have been quite cold and fresh or warm.  Respond in character and say something and be sure to mention that you are now out of the water.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;	
			last_exited_water_thought_timestamp = std::chrono::steady_clock::now();
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	



	
	if (base && ( (std::strcmp(base_name, "Nullify Magicka") == 0) ) )
	{
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Nullify Magicka effect application!");
			std::string final_thought_string = std::format("You have been cursed with a Nullify Magicka effect.  Your magicka has been nullified.  You feel powerless and unable to cast spells and unable to shout as well.  This curse is so strong, it renders you completly unable to use magic.  That is the effect of the curse, because you do not have a magic License.  Respond in character and say something and be sure to mention that you are now under the Nullify Magicka effect, because you have no license to use magic.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		} else {
			SKSE::log::info("Event handler for Nullify Magicka effect removal!");
			std::string final_thought_string = std::format("The Nullify Magicka effect is now removed.  Your magicka is no longer nullified and you can cast spells and shout again.  Respond in character and say something and be sure to mention that you are now free from the suppression of your magical abilities.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;	
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


/*
[2026-10-04 17:25:01.250] [log] [info] [handle_active_magic_effect_changes.cpp:923] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-10-04 17:25:01.250] [log] [info] [handle_active_magic_effect_changes.cpp:924] Effect APPLIED on Lillith | UID=28
[2026-10-04 17:25:01.250] [log] [info] [handle_active_magic_effect_changes.cpp:927] Base name: Firebolt | Base ptr: 0x251eb919b40 | Base-FormID: 12F03 | Base-Form Type: 18   (This means: MGEF) 
[2026-10-04 17:25:01.250] [log] [info] [handle_active_magic_effect_changes.cpp:928] base-Effect EDID:  | Source ptr: 0x251eb8b5300  |  Caster: Forsworn Looter 
[2026-10-04 17:25:01.251] [log] [info] [handle_active_magic_effect_changes.cpp:932] Magnitude: -31.25 | Duration: 1
[2026-10-04 17:25:01.251] [log] [info] [handle_active_magic_effect_changes.cpp:935] Source name: Firebolt | Source FormID: C969B | Source EDID:  
[2026-10-04 17:25:01.251] [log] [info] [handle_active_magic_effect_changes.cpp:941] Form LookupByID 12F03 found: Firebolt
*/	
	if (base && ( (std::strcmp(base_name, "Firebolt") == 0) ) )
	{
		if (a_event->isApplied)
		{
			if (!cooldown_has_passed(last_firebolt_thought_timestamp, 90)) {
				logger::info("Skipping Firebolt thought because its 90-second cooldown has not elapsed.");
				return;
			}

			SKSE::log::info("Event handler for Firebolt effect application!");
			std::string final_thought_string = std::format("You have been hit by a Firebolt spell.  The impact sears your skin and you feel the intense heat and magical energy coursing through you.  The firebolt came from: {}.  Respond in character and say something and be sure to mention that you have been struck by a Firebolt.", caster->GetName());
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
			last_firebolt_thought_timestamp = std::chrono::steady_clock::now();
		} else {
			// no comment or reaction on removal of firebolt hit effect
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	/*
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:954] ========== Found A SO-FAR UNHANDLED effect, that is actually about the Player.  Let's go into more details below! =============
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:955] Effect REMOVED on Lillith | UID=44
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:958] Base name: Orgasm Exhaustion | Base ptr: 0x1f89ed7fc80 | Base-FormID: 241553CF | Base-Form Type: 18   (This means: MGEF) 
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:959] base-Effect EDID:  | Source ptr: 0x1f89f785100  |  Caster: Lillith 
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:963] Magnitude: -1 | Duration: 185
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:966] Source name: Orgasm exhaustion | Source FormID: 2411A34C | Source EDID:  
[2026-10-04 21:32:12.201] [log] [info] [handle_active_magic_effect_changes.cpp:972] Form LookupByID 241553CF found: Orgasm Exhaustion	
	*/
	if (base && ( (std::strcmp(base_name, "Orgasm Exhaustion") == 0) ) )
	{
		// The Orgasm Exhaustion can be applied multiople times in parallel and will then have applied and removed independently for each instance.
		// Also usually when that is applied, lots of other things are going on.  And the effect is short and not very strong or important.  
		// Best we leave that for now and don't do anything with it.
		if (a_event->isApplied)
		{
			SKSE::log::info("Event handler for Orgasm Exhaustion effect application!");
			std::string final_thought_string = std::format("You are experiencing Orgasm Exhaustion.  This effect leaves you drained and unable to engage in further sexual activity for a while.  The source of this effect is: {}.  Respond in character and describe your current state and feelings.", caster->GetName());
			// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		} else {
			// LillithOnlyBox("NOTE:  Orgasm Exhaustion removed!!");
			// LillithOnlyBox("NOTE:  Orgasm Exhaustion removed!!");
		}
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	


// *************************************************
// *** HERE WE PUT SOME EXTRA NOTIFICATIONS FOR UNHANDLED MAGIC EFFECTS THAT WE DON'T UNDERSTAND AND WANT MORE POPUP MESSAGES FOR, TO BETTER DETECT THEM AND THEN UNDERSTAND THEM ***
// ********************************************************
/*  
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=35
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Training | Base ptr: 0x18a9b81a940 | Base-FormID: 1003C907 | Base-Form Type: 18   (This means: MGEF) 
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18a9b492f80  |  Caster: Lillith 
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name:  | Source FormID: 1003C3A2 | Source EDID:  
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID 1003C907 found: Training
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:888] .
[2026-09-21 22:40:07.282] [log] [info] [handle_active_magic_effect_changes.cpp:889] .
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:864] 
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:865] Effect APPLIED on Lillith | UID=36
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:868] Base name: Nullify Magicka | Base ptr: 0x18abcd58480 | Base-FormID: FE0608FB | Base-Form Type: 18 
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:869] base-Effect EDID:  | Source ptr: 0x18abcce2500  |  Caster: Lillith 
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:873] Magnitude: 0 | Duration: 0
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:876] Source name: Nullify Magic Enchantment | Source FormID: FE0608FC | Source EDID:  
[2026-09-21 22:35:25.721] [log] [info] [handle_active_magic_effect_changes.cpp:882] Form LookupByID FE0608FB found: Nullify Magicka
*/
	if (base && ( (std::strcmp(base_name, "Training") == 0) && (std::strcmp(base->GetFormEditorID(), "Nullify Magicka") == 0) ) )
	{
		std::string final_thought_string = std::format("STRANGE BUT INTERESTING MAGIC EFFECT:  {} WAS JUST APPLIED= {}  The effect will be in the log as UNHANDLED, so you can look up all the details there.", base_name, a_event->isApplied ? "APPLIED" : "REMOVED");
		LillithOnlyBox(final_thought_string);
		LillithOnlyBox(final_thought_string);
		LillithOnlyBox(final_thought_string);
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
