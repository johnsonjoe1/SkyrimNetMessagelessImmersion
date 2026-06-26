#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "handle_armor_activation.h"
#include "handle_furniture_activation.h"
#include "handle_mod_broadcasts.h"
#include "handle_active_magic_effect_changes.h"
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

/*  TODO-LIST   
** Many more UD-Mod-Events for different devices being applied could be commented.
DONE 0.5.2:   ** Many more Creature-Summoner-Creatures could be commented upon summoning them.
AFTER THAT:  ** Try to find out the (other) actor of an SL scene and then comment at the beginning of the scene, maybe even based on the tags of the scene.
** Build the periodic check status-thought-message:
   --  YPS Thought:  Take the latest one queued from the YPS thought mod event and bring it forward
   --  YPS Shoes Penalty
   --  Thirst-Max-Reached
   --  Fatigue-Max-Reached
   --  Hunger-Max-Reached
   --  Milk-Max-Reached
   --  Dirtyness
   --  Fame:  Take the highest of all fame values and create a thought
   --  Sickness:  Do we have a status there?  Maybe from active magic effect hook?
   --  Nakedness/Flashing:  

  

*/




static auto last_periodic_check_for_changes = std::chrono::steady_clock::now();

float previous_dirt_value = 100000;  // some impossible value, so that no message occurs (unless dirt value 0, which wouldn't likely be the case in mid-game)

// static auto last_speech_timestamp = std::chrono::steady_clock::now();


void handle_check_for_close_conversations()
{
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
}






void handle_player_dirt()
{
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
			SKSE::log::info("Note:  Dirtiness-level-update thought 1 was delivered.");
		} else if ((previous_dirt_value > 0.2f) && (dirtValue <= 0.2f)  && (dirtValue >= 0.19f)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("Finally some of the dirt on your body has washed off.  It's not as clean as washing with soap, but you are still more fresh and clean than before!  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!", static_cast<int>(dirtValue * 100)));
			SKSE::log::info("Note:  Update according to empirical values:  some swimming brings dirtiness down to 0.2 absolute it seems.");
		} else if ((previous_dirt_value > 0.01f) && (dirtValue <= 0.01f) ) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("Finally you are completely clean, like only a nice bath with soap can achieve!  Say as much in your response and let us know how that makes you feel!  And make it clear that you speak about your dirtiness in your response!", static_cast<int>(dirtValue * 100)));
			SKSE::log::info("Note:  Dirtiness-level-update thought 2 was delivered.");
		}
		previous_dirt_value = dirtValue;  // Update the previous dirt value for the
	} else {
		logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Player dirtiness global variable not found.");
	}
}
//  Here comes the code for hooking into the active effect application and removal, i.e. the list of currently active effects.

class ChangesToTheActiveMagicEffectListEventHandler : public RE::BSTEventSink<RE::TESActiveEffectApplyRemoveEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent( const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::BSTEventSource<RE::TESActiveEffectApplyRemoveEvent>*
    ) override
    {

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
			handle_check_for_close_conversations();
			handle_player_dirt();
			
			handle_timeout_for_stale_scenes();  // just a periodic check, that we don't think we are in a SL scene forever (i.e. no more than 90 seconds after the previous SL_stage_advance)
		}

		handle_changes_in_active_magic_effects(a_event);


        return RE::BSEventNotifyControl::kContinue;
    }
	private:

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
            SKSE::log::info("ActivateEventHandler : That activated object does not seem to be Furniture and also not an Armor item, so we ignore it.  But for the reference, the numeric code was:  {} and the name was:  {}", static_cast<std::uint32_t>(formType), base->GetName());
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