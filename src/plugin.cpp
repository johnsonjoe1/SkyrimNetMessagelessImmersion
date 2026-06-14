#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "handle_armor_activation.h"
#include "handle_furniture_activation.h"
#include "handle_mod_broadcasts.h"
#include "handle_iNeed.h"
#include "handle_yps.h"
#include "handle_fame.h"
#include "misc.h"
#include "papyrus_interface.h"
#include <unordered_set>
#include <optional>
#include <chrono>
#include <string>

namespace logger = SKSE::log;

static auto last_periodic_check_for_changes = std::chrono::steady_clock::now();

float previous_dirt_value = 100000;  // some impossible value, so that no message occurs (unless dirt value 0, which wouldn't likely be the case in mid-game)

// static auto last_speech_timestamp = std::chrono::steady_clock::now();


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
std::array<std::string, 2> list_of_food_contracted_sicknesses = {
    "Stomach Rot",
    "Food Poisoning"
};
std::string my_active_effect_description_string = "Nothing yet!";


// ****************************************************************************************************************
// This is the handling of Active Magic Effects and everything related to it.
class MyVisitor :
    public RE::MagicTarget::ForEachActiveEffectVisitor
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



//  Here comes the code for hooking into the active effect application and removal, i.e. the list of currently active effects.

class ChangesToTheActiveMagicEffectListEventHandler : public RE::BSTEventSink<RE::TESActiveEffectApplyRemoveEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent( const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::BSTEventSource<RE::TESActiveEffectApplyRemoveEvent>*
    ) override
    {

        // Protect from null pointer access, just in case.
		if (!a_event) {
			LillithOnlyBox("WOW! Null EVENT POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's see what this event is about.  Who is the actor and what is the effect?		
		auto* targetRef = a_event->target.get();
		if (!targetRef) {
			LillithOnlyBox("WOW! Null TARGET POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		RE::Actor* actor;
		actor = targetRef->As<RE::Actor>();
		if (!actor) {
			LillithOnlyBox("WOW! There is NO ACTOR in this ACTIVE EVENT CHANGE!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		// ✔ only care about player
		if (!actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
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
			return RE::BSEventNotifyControl::kContinue;
		}
		auto* base = effect->GetBaseObject();
		if (!base)
		{
			LillithOnlyBox("WOW! THE BASE OBJECT IS NULL!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}

		auto our_form_id = base->GetFormID();
		auto base_name = base->GetName();
		auto caster = effect->caster.get();
		auto* source = effect->spell;
		// Let's inspect the FormID and hte Form behind the effect, to see if we can identify it.  
		auto* form = RE::TESForm::LookupByID(our_form_id);
		
		// THIS SECTION OF THE CODE SHOULD BE CALLED VERY FREQUENTLY IN THE COURSE OF MAGIC EFFECTS, handled and even unhandled magic effects.
		// So we put periodic checks here with AT LEAST 5 seconds pause in between.  This should be faily consistent though.
		//
		
		auto now = std::chrono::steady_clock::now();
		if (now - last_periodic_check_for_changes >= std::chrono::seconds(5)) {
			last_periodic_check_for_changes = now;
			logger::info("***************************************************************************5 seconds elapsed:  TIME TO DO PERIODIC CHECKS***********************************************************");
			handle_AND_modesty::handle_AND_modesty_and_nakedness_stuff();
			handle_iNeed::handle_iNeed_hunger_thirst_and_fatigue_stuff();
			handle_yps::handle_yps_fashion_detection_stuff();
			handle_fame::handle_SLSF_Reloaded_fame_stuff();
		}


		if (is_known_irrelevant_magic_effect(base_name))
		{
			logger::info("SKIPPING HANDLING OF IRRELEVANT MAGIC EFFECT: {}", base_name);
			return RE::BSEventNotifyControl::kContinue; 
		}

		// We moved the YPS-Movement-Speed-Stuff to the separate YPS module
		if (base) {
			if (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0) {
				if ( (std::strcmp(source->GetName(), "High Heel Novice") == 0) || (std::strcmp(source->GetName(), "Untrained Feet") == 0) ) {
					handle_yps::handle_yps_magic_effect_stuff(a_event, effect);  // const RE::TESActiveEffectApplyRemoveEvent* a_event,
					return RE::BSEventNotifyControl::kContinue;
				}
			}
		}

		// Let's also track the drunk-stumble-script:  It means the stumble-and-fall animation is playing, 
		// so we might as well say so.
		if ( (std::strcmp(base_name, "Drunk Stumble Script") == 0) && (a_event->isApplied) ){  	// Drunk Stumble Script
			// Here you can add your custom logic for when the stumble-and-fall animation is playing.
			DumpThoughts::throw_out_TTS_thought_message("YOU, the player, are so drunk, that you just lost balance and just stumbled and fell over you own feet from all the alcohol.  How does that make you feel?  What are you thinking now? ");
			return RE::BSEventNotifyControl::kContinue;
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
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track Unforgiving Devices Struggle Exhaustion here:  FIRST THE APPLICATION OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "Exhaustion") == 0)  ) && ( (std::strcmp(source->GetName(), "Struggle exhaustion") == 0)  ) )
		{
			if (a_event->isApplied)
			{
				SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION APPLICATION!");
				DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just tried getting out of your locking bondage devices for a whole while. You may have made some progress, but nevertheless now you are too exhausted to continue.  Say as much in your response.")); //  + standard_thought_instruction;
				return RE::BSEventNotifyControl::kContinue;
			} 
			else  // i.e.  (!a_event->isApplied)
			{
				SKSE::log::info("Event handler for UD STRUGGLE EXHAUSTION REMOVAL!");
				DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just were trying to get out of your locking bondage devices for a whole while. You may have made some progress, but in any case, that activity had made you exhausted to the point where you couldn't continue any more.  But now time has passed and you're feeling better and you're good to go and maybe could continue trying.  Say as much in your response.")); //  + standard_thought_instruction;
				return RE::BSEventNotifyControl::kContinue;
			}
		}
		// Let's try to track UD/US Black-Goo-Application-Effect here:  FIRST THE APPLICATION OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "Device Manifest") == 0)  ) && (a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for BLACK-GOO-APPLICATION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("Event handler for BLACK-GOO-APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("Some substance called black goo just came in contact with you, and, to your horror, it manifested into a bondage device, thus trapping you as the victim now locked into said device.  What are you thinking in the face of this situation? ")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track UD/DD slowdown-effect from bondage boots:  :  NOW THE REMOVAL OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "SpeedMult Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "BootSlow-Enchant") == 0)  ))
		{
			if (a_event->isApplied)
			{
				SKSE::log::info("Event handler for UD BONDAGE BOOTS SLOWDOWN APPLICATION!");
				DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just got locking bondage boots equipped onto your feet and you cannot take them off any more. But the important point is:  You cannot walk or run so fast any more with these heels equipped onto your feet! You will be slowed down for the whole time while wearing them (thus less able to run away from dangerious things)! Say as much in your response.")); //  + standard_thought_instruction;
				return RE::BSEventNotifyControl::kContinue;
			} 
			else // i.e.  if (!a_event->isApplied) )
			{
				SKSE::log::info("Event handler for UD BONDAGE BOOTS SLOWDOWN REMOVAL!");
				DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, had your feet locked into bondage boots the whole time and couldn't get them off. This has slowed you down the whole time. But now you got rid of the locking bondage devices on your feet. But the important point is:  This means you can finally move much faster again!  (And you won't trip over your feet any more.)  Say as much in your response.")); //  + standard_thought_instruction;
				return RE::BSEventNotifyControl::kContinue;
			}
		}



		// We have from another mod:
		// PlayerDirt = Game.GetFormFromFile(0x000DA8, "Bathing in Skyrim.esp") as GlobalVariable
		// DirtString = " Dirt: " + ((Bis.GetPlayerDirt() * 100.0) as Int) + "%"
		//
		// So this should be a value between 0 and 1, and 1 meaning 100% dirty, and 0 meaning completely clean.
		// This should allow for direct native access to the same from C++:
		//	
		auto* playerDirt = RE::TESDataHandler::GetSingleton() ->LookupForm<RE::TESGlobal>(0x000DA8, "Bathing in Skyrim.esp");
		if (playerDirt) {
			float dirtValue = playerDirt->value;
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Player dirtiness: {} (previous value: {} )", dirtValue, previous_dirt_value);
			
			if ((previous_dirt_value < 0.6f) && (dirtValue >= 0.6f)) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are getting really dirty now! This is not just a little bit, but really dirty so that you are not as charming when talking to people and maybe you also could get sick more easily.  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!"));
				SKSE::log::info("Note:  Dirtyness-level-update thought 1 was delivered.");
			} else if ((previous_dirt_value >= 0.01f) && (dirtValue < 0.01f)) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are finally fresh and clean and no longer dirty! People won't dislike you anymore for being dirty! Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!", static_cast<int>(dirtValue * 100)));
				SKSE::log::info("Note:  Dirtyness-level-update thought 2 was delivered.");
			}
			previous_dirt_value = dirtValue;  // Update the previous dirt value for the
		} else {
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Player dirtiness global variable not found.");
		}


		auto* player = RE::PlayerCharacter::GetSingleton();

		if (player) {
			auto& data = player->GetPlayerRuntimeData();
			logger::info(
				"CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//closestConversation={} runningConversation={}",
				data.closestConversation != nullptr,
				data.aiConversationRunning != nullptr);
			if (data.closestConversation) {
				//  NOTE:  This message box never fires, because that happens actually inside a dialoge-UI with talk options and all that, at least I think that's why.
				LillithOnlyBox("closestConversation IS NOT NULL ANY MORE!!!!");
			}
			if (data.aiConversationRunning) {
				LillithOnlyBox("aiConversationRunning IS NOT NULL ANY MORE!!!!");
			}
		} else {
			logger::info(
				"CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK// NO PLAYER??? NO PLAYER???NO PLAYER???NO PLAYER???NO PLAYER???NO PLAYER???");
		}


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

        return RE::BSEventNotifyControl::kContinue;
    }
	private:



		
	bool is_known_irrelevant_magic_effect(std::string base_name)
	{
		static const std::array<std::string, 12> irrelevant_effect_list = {
			"RaceMenuHH Scale Effect"   , 
			"Consume Food Portion"   , 
			"Automate Hunger Script"  ,
			"SOS_Addon_PHF_Recolor" ,
			"Maintenance" ,
			"SCO_CellChangeDetectMgef" ,
			"SCO_CellChangeBegin",
			"Cell Tracking Effect",
			"UIWheelMenu_LoadMenu",
			"UIWheelMenu_SetOption",
			"UIWheelMenu_CloseMenu",
			"UIWheelMenu_ChooseOption"
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
};

class DialogueHook
{
public:
    static void Install()
    {

		logger::info("SNMI:  CHECK//CHECK//CHECH//CHECK//CHECK//CHECK//CHECH//CHECK//CHECK//CHECK//CHECH//CHECK Installing DialogueHook.");
        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_Actor[0] };

        _SetDialogueWithPlayer =
            vtbl.write_vfunc(0x041, SetDialogueWithPlayer);

		logger::info("SNMI:  CHECK//CHECK//CHECH//CHECK//CHECK//CHECK//CHECH//CHECK//CHECK//CHECK//CHECH//CHECK DialogueHook installed.");

    }

private:
    static bool SetDialogueWithPlayer(
        RE::Actor* a_actor,
        bool a_flag,
        bool a_forceGreet,
        RE::TESTopicInfo* a_topic)
    {
        if (a_actor) {
            logger::info(
                "Actor {} dialogue forceGreet={} topic={:08X}",
                a_actor->GetName(),
                a_forceGreet,
                a_topic ? a_topic->GetFormID() : 0);
			LillithOnlyBox("SetDialogueWithPlayer TRIGGERED!!!!");
			logger::info(
				"DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK//DOUBLECHECK:  Did the SetDialogueWithPlayer really trigger?  ");
        }

        return _SetDialogueWithPlayer(
            a_actor,
            a_flag,
            a_forceGreet,
            a_topic);
    }

    inline static REL::Relocation<
        decltype(SetDialogueWithPlayer)> _SetDialogueWithPlayer;
};


class ModEventHandler : public RE::BSTEventSink<SKSE::ModCallbackEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
        override
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }

		handle_mod_event_broadcasts(a_event);

	

        return RE::BSEventNotifyControl::kContinue;
    }

private:
	
};


//  Here comes the code for hooking into the furniture usage events, or even all usage events, but for now we focus on furniture.
class ActivateEventHandler : public RE::BSTEventSink<RE::TESActivateEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(const RE::TESActivateEvent* event, RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!event)
            return RE::BSEventNotifyControl::kContinue;

        auto activatedRef = event->objectActivated.get();
        auto activatorRef = event->actionRef.get();

        if (!activatedRef || !activatorRef)
            return RE::BSEventNotifyControl::kContinue;

        // only care about player
        if (activatorRef != RE::PlayerCharacter::GetSingleton())
            return RE::BSEventNotifyControl::kContinue;

        auto base = activatedRef->GetBaseObject();
        if (!base)
            return RE::BSEventNotifyControl::kContinue;

        SKSE::log::info("=================== New activation Event noticed ===================.");
        SKSE::log::info("Player activated: {}",
            base->GetName()
        );

        // For the moment, we ignore everything, that is not furniture.
        auto formType = base->GetFormType();
        if (formType == RE::FormType::Furniture)
        {
			handle_furniture_item_activation(base);
			// Return control, no question
            return RE::BSEventNotifyControl::kContinue;
        } else if (formType == RE::FormType::Armor) {

			handle_armor_item_activation(base);

			// Return control, no question
            return RE::BSEventNotifyControl::kContinue;
		} else
		{
            SKSE::log::info("[SkyrimNetMessagelessImmersion] That activated object does not seem to be Furniture and also not an Armor item, so we ignore it.");
            return RE::BSEventNotifyControl::kContinue;
        }

		
        return RE::BSEventNotifyControl::kContinue;
    };
	private:

};

//  Instantiate the code for hooking into Furniture, i.e. Blacksmith Forge, Workbenches, Tanning Rack and also Milk Pumps.
static ActivateEventHandler g_activateHandler;

//  Instantiate the code for hooking into the Active Effect Changes.
static ChangesToTheActiveMagicEffectListEventHandler g_ChangesToTheActiveMagicEffectListEventHandler;

//  Instantiate the code for hooking into the mod event listener.
static ModEventHandler g_mod_event_handler;

auto* source = RE::ScriptEventSourceHolder::GetSingleton();

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
        

		SKSE::log::info("[SkyrimNetMessagelessImmersion] STARTUP BEFORE THE LIKELY CRASH done.");

		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0004 SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		// That has happend before, right?  SetupLog();
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0002 Log setup just passed.");
		// spdlog::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		SKSE::log::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0003 First log message should be written now.");

		// Now we register the event handler for the Tanning Rack, Benches and all the other crap.
		RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&g_activateHandler);

		if (source) {
			source->AddEventSink<RE::TESActiveEffectApplyRemoveEvent>(&g_ChangesToTheActiveMagicEffectListEventHandler);
			SKSE::log::info("[SkyrimNetMessagelessImmersion] 0005: Registered ACTIVE-MAGIC-EFFECT-CHANGE event sink");
		} else {
			RE::DebugMessageBox("FAILED TO GET SOURCE FOR THE EVENT LISTENER REGISTRATION!!!!");
		};


		// Register also out mod-event-listener, that just catched intentionally broadcasted mod events.
		// auto* mod_event_source = SKSE::GetModCallbackEventSource();
		// mod_event_source->AddEventSink(&g_mod_event_handler);
		SKSE::GetModCallbackEventSource()->AddEventSink(&g_mod_event_handler);

		// Try something new, register for dialogue, so we can form a better player-thought timing.
		DialogueHook::Install();

		break;

	case SKSE::MessagingInterface::kPostLoad:
		// DANGER HERE:  The player name may not be availabe.  This might crash!!	
		//  DumpThoughts::reset_last_game_load_or_reload_timestamp();	
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		// DANGER HERE:  The player name may not be availabe.  This might crash!!	
		//  DumpThoughts::reset_last_game_load_or_reload_timestamp();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
		handle_AND_modesty::reset_previous_rank_to_current_rank();
        break;
	case SKSE::MessagingInterface::kNewGame:
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
		handle_AND_modesty::reset_previous_rank_to_current_rank();
		break;
	};
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();

    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	// We NEED to register the papyrus interfaces here, before the Virtual-Machine is running.
	// Otherwise we get an error in the papayrus.log.0:  [05/21/2026 - 07:26:51PM] error: Unbound native function "SetMilkLevel" called
	SKSE::GetPapyrusInterface()->Register(SNMIPapyrus::Register);

    return true;
}