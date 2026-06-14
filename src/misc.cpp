#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include <string_view>
#include <unordered_set>

std::string  final_lillith_message;
// void LillithOnlyBox(const char* a_message)

void LillithOnlyBox(std::string_view a_message)
{
	/*  ORIGINAL:
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(("SNMI:  An unhandled mod-event was discovered: " + debug_message).c_str());
	} else {
		SKSE::log::info("SNMI:  An unhandled mod-event was discovered: {}", debug_message);
	}  
	*/
	final_lillith_message = std::string("SNMI: LILLITH: ");
	final_lillith_message += a_message;
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(final_lillith_message.c_str());  // Show the message in a message box if the player's name is Lillith.
	} else {
		SKSE::log::info("LILLITH-MESSAGEBOX-REDIRECTED-TO-LOG: {}", a_message);  // Otherwise, log the message to the console.
	}  
}


bool is_known_useless_event(std::string event_name)
{
	static const std::unordered_set<std::string> ignored_mod_events = {
		"SKICP_configManagerReady",
		"Apropos2GameLoaded",
		"Apropos2ConfigClose",
		"SNMI_JustPumpMyStringToPlayerThought",        // treat our own events with a log entry only.
		"SNMI_Pump_IMPORANT_PlayerThought",            // treat our own events with a log entry only.
		"SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought",   // treat our own events with a log entry only.
		"SKIWF_hudModeChanged", 
		"SKIWF_widgetLoaded", 
		"SKIWF_widgetManagerReady", 
		"SKIWF_iWantWidgetsReset", 
		"SKIWF_iWantStatusBarsReady", 
		"iWantStatusBarsReady", 
		"iWantWidgetsReset", 
		"SKICP_modSelected",   // this is broadcast when the player selects a mod in the SKI Configuration Menu.
		"SKICP_pageSelected",  // this is broadcast when the player selects a page of a mod configuration in the SKI Configuration Menu.
		"SKICP_optionHighlighted",   // this is broadcast when the player highlights a configuration option for a mod in the SKI Configuration Menu.
		"SKICP_optionSelected",   // this is broadcast when the player selects a configuration option for a mod in the SKI Configuration Menu.
		"SKICP_messageDialogClosed",   // this is broadcast when the player closes a message dialog in the SKI Configuration Menu.
		"SKICP_menuSelected",
		"SKICP_menuAccepted",
		"SKICP_inputSelected",
		"SKICP_inputAccepted",
		"SKICP_keymapChanged",   // this is broadcast when the player changes a keymap in the SKI Configuration Menu.
		"SKIWF_widgetError",            // this is broadcast when a widget error occurs.
		"RSM_CategoriesInitialized",   // this is some technical event from RaceMenu that we don't care about.
		"RSM_Initialized",             // this is some technical event from RaceMenu that we don't care about.
		"RSM_SliderChange",
		"RSM_Reinitialized",
		"RSM_RequestTintSave",
		"RSM_RequestTintLoad",
		"RSM_HairColorChange",
		"RSM_ShadersInvalidated",
		"zadRegisterEvents",   			// This is from zadLibs probably and just a technical event anyway.
		"GagSoundsRegistered",			// This is from zadLibs probably and just a technical event anyway.
		"SLA_Int_PlayerLoadsGame",
		"sla_Int_PlayerLoadsGame",
		"sla_UpdateComplete",
		"SN_StatusUpdated", 
		"_SN_StatusUpdated", 
		"_SN_UIConfigured",
		//"SkyrimNet_SpeechStarted",
		//"SkyrimNet_SpeechCompleted",
		//"SkyrimNet_SpeechComplete",
		//"SkyrimNet_AudioStarted",
		//"SkyrimNet_AudioEnded",
		"SkyrimNet_MemoryCreated",  // No need to respond to this, as it's internal memory creation and not relevant to direct game status.
		"UD_AfterUIReload", 			
		"UD_QuestKeywordUpdate", 
		"UD_GenericKeyUpdate", 
		"UD_PatchUpdate",
		"UIListMenu_LoadMenu",
		"UIListMenu_CloseMenu",
		"UIListMenu_SelectItemText",   // this may be useful later, because it indicated player is trying lockpicking now
		"UIListMenu_SelectItem",
		// "UD_SentientDialogue",  // Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		"RSM_LoadPlugins",
		"SeverActions_CellLoaded",
		"SeverActions_FamiliarityTimestamp",
		"SeverActions_ReputationAssess",
		"SeverActions_AmbientBanterReady",
		"SeverActions_ForcedCombatEnded",   // No need to respond to this, I guess?
		"SeverActions_NewTeammateDetected",
		"ReSchlongify",
		"MME_MilkCycleComplete",
		"BeeingFemale",   //  We ignore this for now, maybe later we can do something with it.
		"CBPCPlayerCollisionWithFemaleEvent",
		"PlayerChangedCells",
		"Obody_ApplyMorph",
		"_SN_PlayerConsumes",  // MOD EVENT:  Name: _SN_PlayerConsumes  StrArg: IsEating  NumArg: 0
		"PlayerOrgasmEnd",
		"dhlp-Resume",   // This is technical Devious Helplessness operational stuff, to continue mod processes.
		"dhlp-Suspend",   // This is technical Devious Helplessness operational stuff, to suspend mod processes.
		"dhlp-maintenance",   // This is technical Devious Helplessness operational stuff, for maintenance purposes.
		"SSL_PREPARE_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_LOCK_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStarting",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStart",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStart_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationStarting_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_READY_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageStart",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageStart_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SL_SetSpeed",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SL_EndScene",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SL_AdvanceScene",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"SSL_CLEAR_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd_",                            //  This might be from The-Ancient-Profession.
		"StageStart_",                          //  This might be from The-Ancient-Profession.
		"AnimationStarting_TAPPlayerFreelance", //  This might be from The-Ancient-Profession.
		"AnimationStart_TAPPlayerFreelance",    //  This might be from The-Ancient-Profession.
		"AnimationEnding_TAPPlayerFreelance",   //  This might be from The-Ancient-Profession.
		"AnimationEnd_TAPPlayerFreelance",      //  This might be from The-Ancient-Profession.
		"StageStart_TAPPlayerFreelance",        //  This might be from The-Ancient-Profession.
		"StageEnd_TAPPlayerFreelance",          //  This might be from The-Ancient-Profession.
		"Helpless_RemoveSpell",  // Unknown what this is
		"CaptiveDefeatInit"  // This is called every time a new cell is entered and merely a technical event,  probably for CaptivePlayer.
	};		
	if (ignored_mod_events.contains(event_name)) {
		return true;
		// logger::info("Found!");
	}
	return false;
}
