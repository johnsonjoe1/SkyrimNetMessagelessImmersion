#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "handle_armor_activation.h"
#include "handle_furniture_activation.h"
#include "handle_mod_broadcasts.h"
#include "handle_active_magic_effect_changes.h"
#include "handle_debug_notifications.h"
#include "handle_iNeed.h"
#include "handle_yps.h"
#include "handle_fame.h"
#include "handle_config_ini_file.h"
#include "handle_player_dirt.h"
#include "handle_worn_equipment_change.h"
#include "misc.h"
#include "papyrus_interface.h"
#include <algorithm>
#include <atomic>
#include <unordered_set>
#include <optional>
#include <chrono>
#include <string>
#include <thread>

namespace logger = SKSE::log;

namespace
{
	void LogStartupInformation(const SKSE::LoadInterface* a_skse)
	{
		const auto* plugin = SKSE::PluginDeclaration::GetSingleton();
		const auto runtime = a_skse->RuntimeVersion();
		const auto skseVersion = REL::Version::unpack(a_skse->SKSEVersion());
		const auto* runtimeType = REL::Module::IsVR() ? "VR" : REL::Module::IsAE() ? "Anniversary Edition" : "Special Edition";

#if defined(_M_X64)
		constexpr auto processArchitecture = "x64";
#elif defined(_M_IX86)
		constexpr auto processArchitecture = "x86";
#elif defined(_M_ARM64)
		constexpr auto processArchitecture = "ARM64";
#else
		constexpr auto processArchitecture = "unknown";
#endif

#if defined(NDEBUG)
		constexpr auto buildType = "Release";
#else
		constexpr auto buildType = "Debug";
#endif

		logger::info("==================== STARTUP INFORMATION ====================");
		logger::info("Plugin: {}", plugin->GetName());
		logger::info("SNMI release: {}", plugin->GetVersion().string("."));
		logger::info("Skyrim: {} {}", runtimeType, runtime.string("."));
		logger::info("SKSE: {}", skseVersion.string("."));
		if (const auto* skyrimNet = a_skse->GetPluginInfo("SkyrimNet")) {
			logger::info("SkyrimNet: {}", REL::Version::unpack(skyrimNet->version).string("."));
		} else {
			logger::info("SkyrimNet: version unavailable (plugin not registered yet)");
		}
		logger::info("Machine: Windows {}", processArchitecture);
		logger::info("Build: {} (compiled {} {})", buildType, __DATE__, __TIME__);
		logger::info("=============================================================");
	}
}

/*  TODO-LIST   
**  Handle the changes in YPS-Buffs:  (Maybe also remember the clothing-contribution (manually) from last time.  Mabe also the makeup/nails/hair from last time.)
**  Reign in the SLSF-Messageboxes by making them conditional on a constant flag.

MUCH LATER:  ** Try to find out the (other) actor of an SL scene and then comment at the beginning of the scene, maybe even based on the tags of the scene.

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

   --  Magic effect handling still missing:  BellSoundEffect  (from the pear and chain plug device)
   --   Base name: ChainSoundEffect | Base ptr: 0x165f448a080 | Base-FormID: 1101BB51 | Base-Form Type: 18   (This means: MGEF) 
        base-Effect EDID: zadx_HR_ChainSoundMagicEffect | Source ptr: 0x165f4163040  |  Caster: Lillith 
   --  Magic effect handling still missing:  Orgasm Exhaustion (from UD vibrating devices)

   --  MOD-Event-Handling:   MOD EVENT:  Name: ''BM-LPO_ViolationFound''  StrArg: ''''  NumArg: 0
   --  MOD-Event-Handling:   MOD EVENT:  Name: ''BM-LPO_BountyStart''  StrArg: ''''  NumArg: 0
   --  MOD-Event-Handling:   MOD EVENT:  Name: BM-LPO_BountyEnd  StrArg:   NumArg: 0
   --  MOD-Event-Handling:   MOD EVENT:  Name: BM-LPO_ViolationCheck  StrArg:   NumArg: 0

   **  Add the Apropos2 pain status effects and changes

   **  Add an internal debug hotkey to trigger general status overview concerning heavy effects currently in place and easily forgotten.

*/

namespace
{
	std::atomic_bool periodicChecksEnabled{ false };
	std::atomic_bool periodicTaskQueued{ false };
	std::chrono::steady_clock::time_point nextPeriodicCheck;
	std::jthread periodicSchedulerThread;

	void RunPeriodicChecksIfDue();

	void StartPeriodicScheduler()
	{
		if (periodicSchedulerThread.joinable()) {
			return;
		}

		periodicSchedulerThread = std::jthread([](std::stop_token a_stopToken) {
			while (!a_stopToken.stop_requested()) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				if (a_stopToken.stop_requested() || !periodicChecksEnabled.load()) {
					continue;
				}

				bool expected = false;
				if (!periodicTaskQueued.compare_exchange_strong(expected, true)) {
					continue;
				}

				if (auto* taskInterface = SKSE::GetTaskInterface()) {
					taskInterface->AddTask([]() {
						periodicTaskQueued.store(false);
						RunPeriodicChecksIfDue();
					});
				} else {
					periodicTaskQueued.store(false);
				}
			}
		});

		logger::info("Started the periodic-check scheduler.");
	}

	void SuspendPeriodicChecks()
	{
		periodicChecksEnabled.store(false);
		logger::info("Suspended periodic checks for game loading.");
	}

	void ResumePeriodicChecks()
	{
		// Run the first pass promptly, while the existing post-load thought-output
		// guard is active, so every handler establishes a baseline for this game.
		nextPeriodicCheck = std::chrono::steady_clock::now();
		periodicChecksEnabled.store(true);
		logger::info("Enabled periodic checks for the loaded game.");
	}
}

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
			// At first I thought we could use this for something useful, but it's much too random it seems.  "Stupid Dog" conversations and such, not really helping.
			// LillithOnlyBox("closestConversation IS NOT NULL ANY MORE!!!!");
		}
		if (data.aiConversationRunning) {
			LillithOnlyBox("aiConversationRunning IS NOT NULL ANY MORE!!!!");
		}
	} else {
		logger::info(
			"CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK//CHECK// NO PLAYER??? NO PLAYER???NO PLAYER???NO PLAYER???NO PLAYER???NO PLAYER???");
	}
}

namespace
{
	void RunPeriodicChecksIfDue()
	{
		if (!periodicChecksEnabled.load() || !RE::PlayerCharacter::GetSingleton()) {
			return;
		}

		if (auto* ui = RE::UI::GetSingleton(); ui && ui->GameIsPaused()) {
			return;
		}

		const auto now = std::chrono::steady_clock::now();
		if (now < nextPeriodicCheck) {
			return;
		}

		logger::info("\n***********************************************************\n***** RunPeriodicChecksIfDue():  5 seconds elapsed:  TIME TO DO PERIODIC CHECKS *****\n***********************************************************");

		const auto interval = std::chrono::seconds(std::max(1, SNMI::GetSettings().updateInterval));
		nextPeriodicCheck = now + interval;

		logger::info("Starting periodic checks (interval: {} seconds).", interval.count());
		handle_AND_modesty::handle_AND_modesty_and_nakedness_stuff();
		handle_iNeed::handle_iNeed_hunger_thirst_and_fatigue_stuff();
		handle_yps::handle_yps_fashion_detection_stuff();
		handle_fame::handle_SLSF_Reloaded_fame_stuff();
		handle_check_for_close_conversations();
		handle_player_dirt::handle_player_dirt_changes();
		handle_timeout_for_stale_scenes();
		logger::info("\n******************************************************************************\n***** RunPeriodicChecksIfDue():  FINISHED DOING PERIODIC CHECKS *****\n***** Callbacks and events, that happen driven by other mods are handled separately *****\n******************************************************************************");
	}
}


//  Here comes the code for hooking into the active effect application and removal, i.e. the list of currently active effects.
class ChangesToTheActiveMagicEffectListEventHandler : public RE::BSTEventSink<RE::TESActiveEffectApplyRemoveEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent( const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::BSTEventSource<RE::TESActiveEffectApplyRemoveEvent>*
    ) override
    {
		handle_changes_in_active_magic_effects(a_event);


        return RE::BSEventNotifyControl::kContinue;
    }
	private:

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
        
		install_debug_notification_hook();

		SKSE::log::info("[SkyrimNetMessagelessImmersion] STARTUP BEFORE THE LIKELY CRASH done.");

		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0004 SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		// That has happend before, right?  SetupLog();
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0002 Log setup just passed.");
		// spdlog::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		SKSE::log::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0003 First log message should be written now.");

		// Now we register the event handler for the Tanning Rack, Benches and all the other crap.
		RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESActivateEvent>(&g_activateHandler);
		handle_worn_equipment_change::register_event_handler();

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
		StartPeriodicScheduler();

		break;

	case SKSE::MessagingInterface::kPostLoad:
		// This is an SKSE/plugin lifecycle event; no playable game is ready yet.
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		SuspendPeriodicChecks();
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
		handle_yps::reset_hair_stage_tracking();
		handle_yps::reset_hair_dye_tracking();
		handle_yps::reset_fashion_tracking();
		
		SNMIPapyrus::suppress_next_Apropos2_state_updates();
		SNMIPapyrus::previous_yps_AddictionLevel = -99.0f;
		SNMIPapyrus::previous_yps_AddictionBuff = -99.0f;
		handle_AND_modesty::reset_previous_rank_to_current_rank();
		handle_iNeed::try_to_reset_iNeed_stuff_after_game_load_or_start();
		handle_player_dirt::try_to_reset_player_dirt_after_game_load_or_start();
		refresh_currently_worn_item_records();
		historic_worn_item_records = currently_worn_item_records;  // We eliminate any fake changes due to game load or new game
		ResumePeriodicChecks();
		break;
	case SKSE::MessagingInterface::kNewGame:
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
		handle_yps::reset_hair_stage_tracking();
		handle_yps::reset_hair_dye_tracking();
		handle_yps::reset_fashion_tracking();
		handle_AND_modesty::reset_previous_rank_to_current_rank();
		handle_iNeed::try_to_reset_iNeed_stuff_after_game_load_or_start();
		handle_player_dirt::try_to_reset_player_dirt_after_game_load_or_start();
		refresh_currently_worn_item_records();
		historic_worn_item_records = currently_worn_item_records;  // We eliminate any fake changes due to game load or new game
		ResumePeriodicChecks();
		break;
	};
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();
	LogStartupInformation(skse);

    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	// We NEED to register the papyrus interfaces here, before the Virtual-Machine is running.
	// Otherwise we get an error in the papayrus.log.0:  [05/21/2026 - 07:26:51PM] error: Unbound native function "SetMilkLevel" called
	SKSE::GetPapyrusInterface()->Register(SNMIPapyrus::Register);

	// Try to load the (dummy) config.ini file, so that we can read the settings from it.  
	SNMI::LoadSettings();

    return true;
}
