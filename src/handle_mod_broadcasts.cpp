#include "log.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "handle_DDUD.h"
#include "handle_yps.h"
#include "handle_jailrape.h"
#include "handle_helplessness.h"
#include "handle_captive_defeat.h"
#include "handle_bodysearch.h"
#include "handle_bimbos.h"
#include "handle_licenses_player_oppression.h"
#include "handle_worn_equipment_change.h"
#include "handle_SLAC.h"
#include "player_thought_history.h"
#include "DumpThoughts.h"
#include <optional>
#include <string_view>
#include <unordered_set>

namespace logger = SKSE::log;

static auto last_random_run_up_and_spank_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
static auto last_tap_player_freelance_stage_start_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

namespace
{
	constexpr RE::FormID deviousFollowersGoldControlQuestFormID = 0x00112478;
	constexpr std::string_view deviousFollowersPlugin = "DeviousFollowers.esp";
	constexpr std::string_view deviousFollowersGoldControlScript = "_DFGoldConQScript";
	constexpr auto dialogueCloseGracePeriod = std::chrono::seconds(3);

	struct DeviousFollowersDialogueSnapshot
	{
		bool goldControlEnabled;
		std::optional<std::chrono::steady_clock::time_point> dialogueClosedAt;
	};

	std::optional<DeviousFollowersDialogueSnapshot> deviousFollowersDialogueSnapshot;

	std::optional<bool> get_devious_followers_gold_control_enabled()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!dataHandler || !vm) {
			return std::nullopt;
		}

		auto* quest = dataHandler->LookupForm<RE::TESQuest>(
			deviousFollowersGoldControlQuestFormID,
			deviousFollowersPlugin);
		auto* handlePolicy = vm->GetObjectHandlePolicy();
		if (!quest || !handlePolicy) {
			return std::nullopt;
		}

		const auto handle = handlePolicy->GetHandleForObject(quest->GetFormType(), quest);
		RE::BSTSmartPointer<RE::BSScript::Object> scriptObject;
		if (!vm->FindBoundObject(handle, deviousFollowersGoldControlScript.data(), scriptObject) || !scriptObject) {
			return std::nullopt;
		}

		RE::BSScript::Variable enabled;
		if (!vm->GetPropertyValue(scriptObject, "Enabled", enabled) || !enabled.IsBool()) {
			return std::nullopt;
		}

		return enabled.GetBool();
	}

	void handle_devious_followers_scene_end()
	{
		if (!deviousFollowersDialogueSnapshot) {
			logger::info("DF-SceneEnd received without a Devious Followers dialogue snapshot.");
			return;
		}

		if (deviousFollowersDialogueSnapshot->dialogueClosedAt &&
			std::chrono::steady_clock::now() - *deviousFollowersDialogueSnapshot->dialogueClosedAt > dialogueCloseGracePeriod) {
			logger::info("Ignoring DF-SceneEnd because the dialogue snapshot is stale.");
			deviousFollowersDialogueSnapshot.reset();
			return;
		}

		const auto goldControlEnabledNow = get_devious_followers_gold_control_enabled();
		if (!goldControlEnabledNow) {
			logger::info("Could not read Devious Followers gold-control state at DF-SceneEnd.");
			deviousFollowersDialogueSnapshot.reset();
			return;
		}

		const bool goldControlWasEnabled = deviousFollowersDialogueSnapshot->goldControlEnabled;
		deviousFollowersDialogueSnapshot.reset();
		logger::info(
			"Compared Devious Followers gold-control state across dialogue: {} -> {}",
			goldControlWasEnabled,
			*goldControlEnabledNow);
		if (!goldControlWasEnabled && *goldControlEnabledNow) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You have just finished a conversation in which your devious follower talked you into accepting his control of your gold. From now on, your follower decides how much gold you may carry, takes any excess to reduce your debt, and adds to your debt when you need more. Respond in first person to this new loss of financial freedom. Express your outrage or frustration, but also make sure you mention your follower's name AS WELL AS discribe that he is going to control how much gold you may carry, much like a parent handling an immature child. Maybe express your frustration on how you let yourself be talked into this deal in a weak moment.");
			logger::info("Devious Followers gold control changed from disabled to enabled during dialogue; emitted a player thought.");
		}
	}
}

void handle_dialogue_menu_event(const RE::MenuOpenCloseEvent* a_event)
{
	if (!a_event || a_event->menuName != RE::DialogueMenu::MENU_NAME) {
		return;
	}

	if (a_event->opening) {
		const auto goldControlEnabled = get_devious_followers_gold_control_enabled();
		if (goldControlEnabled) {
			deviousFollowersDialogueSnapshot = DeviousFollowersDialogueSnapshot{ *goldControlEnabled, std::nullopt };
			logger::info("Captured Devious Followers gold-control state at dialogue start: {}", *goldControlEnabled);
		} else {
			deviousFollowersDialogueSnapshot.reset();
		}
	} else if (deviousFollowersDialogueSnapshot) {
		deviousFollowersDialogueSnapshot->dialogueClosedAt = std::chrono::steady_clock::now();
	}
}

void reset_devious_followers_dialogue_tracking()
{
	deviousFollowersDialogueSnapshot.reset();
}

bool shortcircuit_Sever_events(std::string_view event_name)
{
	return event_name.starts_with("Sever");
}

bool is_known_SUPERIRRELEVANT_mod_event(std::string event_name) {
		static const std::unordered_set<std::string> ignored_mod_events = {
		"SKIWF_widgetLoaded",
		"CBPCPlayerCollisionWithFemaleEvent",   // This event can be triggered by a fall as well as just the idle animation while in an armbinder, so:  NO CHANCE TO MAKE ANYTHING USEFUL FROM THAT, unfortunately.  And it can be super-frequent as well.
		"Obody_ApplyMorph",
		"SKICP_configManagerReady",
		"SKICP_modSelected",         // this is broadcast when the player selects a mod in the SKI Configuration Menu.
		"SKICP_pageSelected",        // this is broadcast when the player selects a page of a mod configuration in the SKI Configuration Menu.
		"SKICP_optionHighlighted",   // this is broadcast when the player highlights a configuration option for a mod in the SKI Configuration Menu.
		"SKICP_optionSelected",      // this is broadcast when the player selects a configuration option for a mod in the SKI Configuration Menu.
		"SKICP_messageDialogClosed", // this is broadcast when the player closes a message dialog in the SKI Configuration Menu.
		"SKICP_menuSelected",
		"SKICP_menuAccepted",
		"SKICP_inputSelected",
		"SKICP_inputAccepted",
		"SKICP_keymapChanged",          // this is broadcast when the player changes a keymap in the SKI Configuration Menu.
		"SKICP_sliderSelected",         // more mod events that happen when changing something in the MCM.
		"SKICP_sliderAccepted",         // more mod events that happen when changing something in the MCM.
		"SKICP_dialogCanceled",         // more mod events that happen when changing something in the MCM.		
		"SKIWF_widgetError",            // this is broadcast when a widget error occurs.
		"SKIWF_hudModeChanged", 
		"SKIWF_widgetLoaded", 
		"SKIWF_widgetManagerReady", 
		"SKIWF_iWantWidgetsReset", 
		"SKIWF_iWantStatusBarsReady", 			
		"RSM_CategoriesInitialized",   // this is some technical event from RaceMenu that we don't care about.
		"RSM_Initialized",             // this is some technical event from RaceMenu that we don't care about.
		"RSM_SliderChange",
		"RSM_Reinitialized",
		"RSM_RequestTintSave",
		"RSM_RequestTintLoad",
		"RSM_HairColorChange",
		"RSM_ShadersInvalidated",

		"FW_OMEARefresh",  // No clue what this is, but it doesn't sound very useful for our purposes.
	};		
	if (ignored_mod_events.contains(event_name)) {
		return true;
	}
	return false;
}

bool is_known_useless_event_that_can_be_completely_shortcircuited(std::string event_name)
{
	static const std::unordered_set<std::string> ignored_mod_events = {
		
		"Apropos2GameLoaded",
		"Apropos2ConfigClose",

		// "SNMI_JustPumpMyStringToPlayerThought",             // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_IMPORANT_PlayerThought",                 // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought",        // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_AS_LITTERAL_AS_POSSIBLE_PlayerThought",  // we can't short-circuit that any more, because it should reset background thought cooldowns
		"SNMI_PlayerActivatedSomething",   //  This is our own event, to be picked up by SkyrimNet, so we don't need to respond to that.

		"iWantStatusBarsReady", 
		"iWantWidgetsReset", 
		"iWantWidgetsDDReset",		
		"iWantWidgetsPing",
		"iWantStatusBarsIconStatusChange",
		"ORS_LinkedWidgetUpdate",    // no clue what this is.
		"zadRegisterEvents",   			// This is from zadLibs probably and just a technical event anyway.
		"GagSoundsRegistered",			// This is from zadLibs probably and just a technical event anyway.
		"SLA_Int_PlayerLoadsGame",
		"sla_Int_PlayerLoadsGame",
		"sla_UpdateComplete",
		"SN_StatusUpdated", 
		"_SN_StatusUpdated", 
		"_SN_UIConfigured",
		"_SLS_IntCoverShutdown",    // Not sure what this is.  Maybe the Sexlab-Survival mods Enforcers seeing you.  Too complicated for now.  But maybe later.
		"_SLS_IntWeaponReadied",    // Sexlab-Survival mod triggering this when drawing a weapon.  Nothing for us.
		"_SLS_LicenceStateUpdateEvent",  // Sexlab-Survival mod triggering this at seemingly random times, e.g. in Lauras Shop.
		"_SLS_HighlightItemsStop",
		"_BC_UpdateBackPackWeight",  // This is from SL Survival as well, but not worth dealign with now probably.
		
		//"SkyrimNet_SpeechStarted",
		//"SkyrimNet_SpeechCompleted",
		//"SkyrimNet_SpeechComplete",
		//"SkyrimNet_AudioStarted",
		//"SkyrimNet_AudioEnded",
		"SkyrimNet_MemoryCreated",  // No need to respond to this, as it's internal memory creation and not relevant to direct game status.^
		"SkyrimNet_MoodChanged",  // No need to respond to this, as it's native to SkyrimNet anyway and probably already handled by SkyrimNet itself.
		"SkyrimNet_DiaryCreated",  // This is internal.  No player thoughts.
		"SkyrimNet_TimelineResolved",

		"UIWheelMenu_LoadMenu",      //  This is the wheel menu from SkyrimNet.  We won't do anything with that.
		"UIWheelMenu_SetOption",     //  This is the wheel menu from SkyrimNet.  We won't do anything with that.
		"UIWheelMenu_CloseMenu",     //  This is the wheel menu from SkyrimNet.  We won't do anything with that.
		"UIWheelMenu_ChooseOption",  //  This is the wheel menu from SkyrimNet.  We won't do anything with that.
		"UD_AfterUIReload", 			
		"UD_QuestKeywordUpdate", 
		"UD_GenericKeyUpdate", 
		"UD_PatchUpdate",
		"DeviceVibrateEffectStart",  //  Problaby from UD, but this is already commented on via the Naito-Plugin it seems, so we do nothing here.
		"DeviceVibrateEffectStop",  //  Problaby from UD, but this is already commented on via the Naito-Plugin it seems, so we do nothing here.
		"UIListMenu_LoadMenu",
		"UIListMenu_CloseMenu",
		"UIListMenu_SelectItemText",   // this may be useful later, because it indicated player is trying lockpicking now
		"UIListMenu_SelectItem",
		// "UD_SentientDialogue",  // Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1

		"_SLS_Int_PlayerLoadsGame",  // Sexlab-Survival has detected a reload, nothing else.
		"RSM_LoadPlugins",

		"SLOA_PlayerArousalUpdated",   // This update message is nice, but there is already a player response for that it seems.
		"SLOA_NPCArousalUpdated",      // This update message is nice, but there is no need to respond now.

		"BM-LPO_ViolationCheck",  // This is from Licenses - Player Oppression mod, but this is just the check, nothing for us to work with.  The actual violation might be something noteworthy.

		"ReSchlongify",
		"MME_MilkCycleComplete",
		"BeeingFemale",   //  We ignore this for now, maybe later we can do something with it.
		"CBPCPlayerCollisionWithFemaleEvent",
		"CBPCPlayerGenitalCollisionWithFemaleEvent",
		"PlayerChangedCells",


		"_SN_PlayerConsumes",  // MOD EVENT:  Name: _SN_PlayerConsumes  StrArg: IsEating  NumArg: 0
		
		
		"PlayerOrgasmEnd",

		"dhlp-Resume",   // This is technical Devious Helplessness operational stuff, to continue mod processes.
		"dhlp-Suspend",   // This is technical Devious Helplessness operational stuff, to suspend mod processes.
		"dhlp-maintenance",   // This is technical Devious Helplessness operational stuff, for maintenance purposes.

		"Helpless_FollowerStart",             // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"StageStart_HelplessFollower",        // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"StageEnd_HelplessFollower",          // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"AnimationEnding_HelplessFollower",   // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"AnimationEnd_HelplessFollower",   // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.		
		"StageEnd_HelplessFollower",          // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"Helpless_FollowerRedress",           // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"AnimationStart_HelplessFollower",    // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.
		"AnimationStarting_HelplessFollower",    // This is technical Devious Helplessness / creature stuff, but for followers and currently out of scope.

		"StageEnd_Helpless",    // This is from MOD:  Devious Helplessness.  Stage-end will not be used, so it can be ignored.
		"Helpless_inventorycheck",   // This is also from Devious Helplessness, but it's just a possibility:  sometimes there is a steal event, but with creatures for example this gets triggered also, but there is nothing of this sort.  Therefore:  We do nothing with just this information.


		"AnimationStart_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"StageEnd_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"StageStart_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationEnding_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationEnd_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationChange_HelplessCreature", // hmmm, Devious Helplessness or Aroused Creatures??

		"OrgasmStart_HelplessFollower",  //	those two are both followers, I think.  MOD EVENT:  Name: OrgasmStart_HelplessFollower  StrArg: 1  NumArg: 0
		"OrgasmStart",                   //	those two are both followers, I think.  MOD EVENT:  Name: OrgasmStart  StrArg: 1  NumArg: 0	
		// Technical mod events from Sexlab P+.  There can be up to 15 threads, but I guess those are edge cases that we don't need to handle for now.  
		"SSL_PREPARE_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_PREPARE_Thread1",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_PREPARE_Thread2",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_PREPARE_Thread3",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_PREPARE_Thread4",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		
		"SSL_LOCK_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_LOCK_Thread1",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_LOCK_Thread2",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_LOCK_Thread3",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_LOCK_Thread4",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		
		"SSL_READY_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_READY_Thread1",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_READY_Thread2",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_READY_Thread3",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_READY_Thread4",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		
		"SSL_CLEAR_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_CLEAR_Thread1",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_CLEAR_Thread2",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_CLEAR_Thread3",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_CLEAR_Thread4",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.

		"AnimationStarting",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStart",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStart_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStarting_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.

		"StageStart",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageStart_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.

		"SL_SetSpeed",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SL_EndScene",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SL_AdvanceScene",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd_",                            //  This might be from The-Ancient-Profession.
		"StageStart_",                          //  This might be from The-Ancient-Profession.

		// "AnimationStarting_TAPPlayerFreelance", //  This might be from The-Ancient-Profession.  We need this one for a comment at animation overall start.
		"AnimationStart_TAPPlayerFreelance",    //  This might be from The-Ancient-Profession.
		// "AnimationEnding_TAPPlayerFreelance",   //  This might be from The-Ancient-Profession.  We need this one for a comment at animation overall end.
		"AnimationEnd_TAPPlayerFreelance",      //  This might be from The-Ancient-Profession.
		"StageStart_TAPPlayerFreelance",        //  This might be from The-Ancient-Profession.
		"StageEnd_TAPPlayerFreelance",          //  This might be from The-Ancient-Profession.

		//  "AnimationStarting_BattleFuck",   //  This is from the BattleFuck mod.  This is the one event, that we respond to.
		"AnimationStart_BattleFuck",   //  This is from the BattleFuck mod.  This is 4 seconds after the other, so we ignore this one and respond to the other one.
		"StageStart_BattleFuck", // This is from the BattleFuck mod.  Since this is about undressing itself, we won't stop clothing changes from this one.
		"StageEnd_BattleFuck",   // This is from the BattleFuck mod.  Since this is about undressing itself, we won't stop clothing changes from this one.
		"AnimationEnd_BattleFuck",   // This is from the BattleFuck mod.  nothing to do here.
		//  "AnimationEnding_BattleFuck", // This is from the BattleFuck mod.  We absolutely should comment on that.

		"AnimationStart_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationStarting_CreatureSummoner", // This is from the Creature Summoner mod.
		"StageStart_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationChange", // This is from the Creature Summoner mod.
		"AnimationChange_CreatureSummoner", // This is from the Creature Summoner mod.
		

		"AnimationEnding_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationEnd_CreatureSummoner", // This is from the Creature Summoner mod
		"StageEnd_CreatureSummoner", // This is from the Creature Summoner mod

		"ActorChangeStart",                  //  This *might* be relevant, if that has some extra detail about the current SL scene and changes there, but it's just not a priority now.

		"PlayDBVOTopic",  // This is from the DragonBornVoiceOver Mod, but we don't need to respond to it, as this is already diaglogue.

		"Helpless_RemoveSpell",  // Unknown what this is
		"CaptiveDefeatInit"  // This is called every time a new cell is entered and merely a technical event,  probably for CaptivePlayer.
	};		

	if (ignored_mod_events.contains(event_name)) {
		return true;
	}

	return false;
}

void toggle_in_a_scene_or_not_based_on_mod_events(const SKSE::ModCallbackEvent* a_event) 
{
	if (!a_event) {
		return;
	}

	static const std::unordered_set<std::string_view> scene_start_events = {
		// "AnimationStarting",   // This can also happen in BattleFuck, where undressing is an important point and SHOULD be commented via player-thoughts.
		// "AnimationStart",   // This can also happen in BattleFuck, where undressing is an important point and SHOULD be commented via player-thoughts.
		"AnimationStart_MatchMaker",
		"AnimationStarting_MatchMaker",
		"AnimationStarting_TAPPlayerFreelance",
		"AnimationStart_TAPPlayerFreelance",
		"AnimationStart_CreatureSummoner",
		"AnimationStarting_CreatureSummoner",
		"StageStart_CreatureSummoner",
		"AnimationChange",
		"AnimationChange_CreatureSummoner",
		// "AnimationStart_BodySearch" is intentionally excluded: body search itself is about clothing, especially in the second part.
		"StageStart_TAPPlayerFreelance",
		"StageStart_",
		"SL_AdvanceScene",
		"AnimationStarting_HelplessCreature",
		"StageStart_HelplessCreature",

		"AnimationStarting_Helpless",
		"StageStart_Helpless",
		"AnimationStarting_JailRapePC",
		"StageStart_JailRapePC",
		"AnimationChange_JailRapePC",
	};

	static const std::unordered_set<std::string_view> scene_end_events = {
		"AnimationEnding",
		"AnimationEnd",
		"AnimationEnding_",     // sometimes no originator is mentioned
		"AnimationEnd_",		// sometimes no originator is mentioned
		"AnimationEnding_TAPPlayerFreelance",
		"AnimationEnd_TAPPlayerFreelance",
		"AnimationEnding_CreatureSummoner",
		"AnimationEnd_CreatureSummoner",
		"AnimationEnd_MatchMaker",
		"AnimationEnding_MatchMaker",
		"AnimationEnding_HelplessCreature",   //  This is from Aroused Creatures (I think)
		"AnimationEnd_HelplessCreature",      //  This is from Aroused Creatures (I think)
		"AnimationEnd_Helpless",    // This is from Devious Helplessness.
		"AnimationEnding_JailRapePC",
		"AnimationEnd_JailRapePC",

	};

	const std::string_view event_name = a_event->eventName;
	if (handle_SLAC::update_scene_status_from_mod_event(a_event)) {
		return;
	}

	const bool is_scene_start = scene_start_events.contains(event_name);
	const bool is_scene_end = scene_end_events.contains(event_name);
	if (!is_scene_start && !is_scene_end) {
		return;
	}

	auto* player = RE::PlayerCharacter::GetSingleton();
	const char* player_name = player ? player->GetName() : nullptr;
	const bool player_in_scene = player_name && !a_event->strArg.empty() &&
		std::strcmp(a_event->strArg.c_str(), player_name) == 0;
	logger::info(
		"NEW SWITCH TO CHECK FOR PLAYER-INVOLVEMENT BEFORE DECIDING ON PLAYER-SCENE-PARTICIPATION-FLAG: Scene participant check: event={} strArg='{}' player='{}' player_in_scene={}",
		a_event->eventName.c_str(),
		a_event->strArg.c_str(),
		player_name ? player_name : "<unknown>",
		player_in_scene);
	if (!player_in_scene) {
		return;
	}

	if (is_scene_start) {
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("Mod-Event-Based DISABLING OF CLOTHING-CHANGE-COMMENTS: {}  StrArg: {} ", a_event->eventName.c_str(), a_event->strArg.c_str());  
		set_current_animation_status("in_a_scene", event_name);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if (is_scene_end) {
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("Mod-Event-Based RE-ENABLING OF CLOTHING-CHANGE-COMMENTS: {}\n{}\n.", a_event->eventName.c_str(), a_event->strArg.c_str());  
		set_current_animation_status("not_in_a_scene", event_name);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
}

void handle_mod_event_broadcasts(const SKSE::ModCallbackEvent* a_event)
{
	if (a_event->eventName == "DF-SceneEnd") {
		handle_devious_followers_scene_end();
	}

	if (shortcircuit_Sever_events(a_event->eventName.c_str())) {
		return;
	}

	if ( is_known_SUPERIRRELEVANT_mod_event(a_event->eventName.c_str())) {
		// This mod event is so frequent it clutters up the log even if we just dedicate one line to it, so we will just dismiss this one silently.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	toggle_in_a_scene_or_not_based_on_mod_events(a_event);
	

	if (is_known_useless_event_that_can_be_completely_shortcircuited(a_event->eventName.c_str()) ||
		handle_jailrape::is_known_irrelevant_event(a_event->eventName.c_str()) ||
		handle_SLAC::is_known_irrelevant_event(a_event->eventName.c_str()))
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("SKIPPING HANDLING OF IRRELEVANT MOD EVENT: Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	if ( (std::strcmp(a_event->eventName.c_str() , "SNMI_JustPumpMyStringToPlayerThought") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_IMPORANT_PlayerThought") == 0) ||
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought") == 0) ||
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_AS_LITTERAL_AS_POSSIBLE_PlayerThought") == 0) ) 
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info(".\n.\n.\nWe really push out a thought now.  From: {}\n{}\n.", a_event->eventName.c_str(), a_event->strArg.c_str());  
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// We log all other mod events, because they might be interesting for us to react to and turn into immersive player thoughts
	logger::info("MOD EVENT:  Name: ''{}''  StrArg: ''{}''  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
	std::string debug_message = std::format("MOD EVENT:  Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg );


	if (handle_DDUD::handle_DDUD_sentient_dialogue_event(a_event)) {
		return;
	}
	if (handle_DDUD::handle_DDUD_device_equipped_event(a_event)) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if (handle_DDUD::handle_DDUD_device_removed_event(a_event)) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if (handle_DDUD::handle_DDUD_device_events(a_event)) {
		return;
	}
	if (handle_DDUD::handle_DDUD_skyrimnet_event(a_event)) {
		return;
	}
	if (handle_DDUD::handle_DDUD_hotkey_captured_and_stopped_event(a_event)) {
		return;
	}


	if (handle_yps::try_handle_yps_mod_stuff(a_event)) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if (handle_bimbos::try_handle_mod_event(a_event)) {
		return;
	}

	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can the rape you right in from of everybody.  There may be bystanders who come to watch the spectacle as you are potentially getting raped.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped and raped and make it clear from your response, that a rape is about to happen to you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// BattleFuck includes the new spectator's display name in this event's string argument.
	if ( (std::strcmp(a_event->eventName.c_str() , "_BF_Onlookers_AddNotification") == 0)  ) {
		std::string thought_message = std::format(
			"YOU, the player, realize that another onlooker has arrived, but instead of helping you, that person has stopped to watch you get molested and used.  Express what you feel and think about that betrayal or humiliation.  BattleFuck described the new spectator as follows: '{}'.  If that description contains the spectator's name, use the name, but do not invent one.",
			a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		LillithOnlyBox("_BF_Onlookers_AddNotification:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// MOD EVENT:  Name:  "AnimationEnding_BattleFuck" :  this is the end of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, survived a sexual assault.  Maybe you got completely undressed by the attacker and maybe he even fucked you and came inside of you, while everybody else stood by and watched, but regardless of that, the assault is over now and you can get dressed again.  Say so and let us know what you are feeling and thinking in that moment.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	



	// MOD EVENT:  Generic orgasm start (and end)  
	// [2026-05-21 21:44:52.579] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmStart  StrArg:   NumArg: 0  
	// [2026-05-21 21:45:00.613] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmEnd  StrArg:   NumArg: 0
	if ( (std::strcmp(a_event->eventName.c_str() , "PlayerOrgasmStart") == 0)  ) {
		// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		std::string  thought_message = std::format("Regardless whether you like it or not, from all the stimulation, you, the player, are now suddenly having an orgasm! Let us know this via your response. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	





	// MOD EVENT:  From SpankThatAss, we have the following event (running up and spanking, in contrast to bump-spanks, which seem not to trigger any mod event unfortunately)
	if ( (std::strcmp(a_event->eventName.c_str() , "_STA_RandomRunUpAndSpankComplete") == 0)  ) {
		// Name: _STA_RandomRunUpAndSpankComplete  StrArg:   NumArg: 0
		if (!cooldown_has_passed(last_random_run_up_and_spank_thought_timestamp, 60)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: _STA_RandomRunUpAndSpankComplete because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_random_run_up_and_spank_thought_timestamp).count());
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		std::string  thought_message = std::format("Since you were busy focussing on crafting, someone used the opportunity and just ran up behind you and spanked your ass with full force while you were distracted! Let us know your response to that, and make sure you implicitly explain that your ass was just slapped hard in your response as well. If you can guess who it was from the context, feel free to mention the culprit as well. You can even tell them to stop, if you want.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_random_run_up_and_spank_thought_timestamp = std::chrono::steady_clock::now();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}		
	// MOD EVENT:  From SpankThatAss, we have the following event:  spanking of any kind, and then we have this resistance loss by one, but we don't do anything on that for now)
	if ( (std::strcmp(a_event->eventName.c_str() , "DF-ResistanceLoss") == 0)  ) {
		// Name: DF-ResistanceLoss  StrArg:   NumArg: 1
		// std::string  thought_message = std::format("Someone just spanked your ass with full force or groped your tits outright! Let us know your response. ");
		// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}		
	if ( (std::strcmp(a_event->eventName.c_str() , "DF-ResistanceLossWithSeverity") == 0) ) {			
		std::string  thought_message = std::format("The NPCs in this game are rather mean and mean-spirited towards you and they seem to be putting you down all the time.  They are just so stupid and annoying that it's wearing you down, sucking your mental energy from you.  Maybe they want to see you fail, so that they can control and dominate you and do what they want with you when you have no power to resist any more.  In any case, they are slowly wearing you out and you may not be able to take it any more at some point.  Tell us how you feel about that.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("DF-ResistanceLossWithSeverity:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	


	if (handle_bodysearch::try_handle_mod_event(a_event)) {
		return;
	}

	
	if (handle_SLAC::try_handle_mod_event(a_event)) {
		return;
	}
	
	// MOD EVENT:  From The Ancient Profession mod, we have the following event:  AnimationStarting_TAPPlayerFreelance
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_TAPPlayerFreelance") == 0)  ) {
		std::string  thought_message = std::format("You just managed to successfully prostitute yourself to a man and were paid the usual price of this profession.  You are now starting a sexual encounter with him, like any normal prostitute would.  Let us know your response to that, and make sure you implicitly explain that you are letting him fuck you and use you for his pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	       
	// MOD EVENT:  From The Ancient Profession mod, we have the following event:  AnimationStarting_TAPPlayerFreelance
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_TAPPlayerFreelance") == 0)  ) {
		if (!cooldown_has_passed(last_tap_player_freelance_stage_start_thought_timestamp, 40)) {
			return;
		}
		std::string  thought_message = std::format("You are fucking a client as a sex worker.  But it seems the client wants yet another sex position, so you go along with it as agreed for that price and let him use you as he wishes.  Let us know your response to that, and make sure you implicitly explain that you are letting him fuck you and use you for his pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_tap_player_freelance_stage_start_thought_timestamp = std::chrono::steady_clock::now();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  From The Ancient Profession mod, we have the following event:  AnimationEnding_TAPPlayerFreelance
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_TAPPlayerFreelance") == 0)  ) {
		std::string  thought_message = std::format("You have just finished a sexual encounter with a client as part of your freelance sex work.  Reflect on the experience and let us know your thoughts on it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	if (handle_helplessness::try_handle_mod_event(a_event)) {
		return;
	}

	if (handle_jailrape::try_handle_mod_event(a_event)) {
		return;
	}

	if (handle_licenses_player_oppression::try_handle_mod_event(a_event)) {
		return;
	}





	if ( (std::strcmp(a_event->eventName.c_str() , "_SN_WaterRefill") == 0) ) {			
		std::string  thought_message = std::format("You just used some water well or similar source to fill up your waterskins.  The supply should last for quite a while.  Tell us how you feel about that.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("iNeed - Food, Water and Sleep - Continued:  _SN_WaterRefill:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	

	if (handle_captive_defeat::try_handle_mod_event(a_event)) {
		return;
	}


	// MOD EVENT:  IF there was other SkyrimNetSpeech or thoughts, we restart our pause tracking, to not overflow the BACKGROUND TTS channel with too much content for the listener.  There should also be a little bit of pause and quiet here and there.
	if ( (std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechComplete") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechCompleted") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechStarted") == 0)  ||
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioStarted") == 0) ||
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioEnded") == 0)  ) {			

		PlayerThoughtHistory::TryRecordSkyrimNetSpeech(a_event->eventName.c_str(), a_event->strArg.c_str());
		
		auto now = std::chrono::steady_clock::now();
		// auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_speech_timestamp);
		auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - DumpThoughts::GetLastSpeechTimestamp());

		SKSE::log::info("=====NO-SPEECH-TIMER for the BACKGROUND CHANNEL WAS RESET BY SKYRIMNET ModEvent after {} seconds. ", runtime.count());
		// last_speech_timestamp=std::chrono::steady_clock::now();
		DumpThoughts::reset_last_speech_timestamp();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	
	// IF the MOD-EVENT really WASNT HANDLED BY THIS POINT, IT IS MAYBE SOMETHING NEW, AND THEREFORE WE MAKE A MESSAGEBOX-ANNOUNCEMENT OF it.
	LillithOnlyBox("An unhandled mod-event was discovered: " + debug_message);
	logger::info("An unhandled mod-event was discovered: {}" , debug_message );

}
