#include "log.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace logger = SKSE::log;

static auto last_device_equipped_thought_timestamp = std::chrono::steady_clock::now();
static auto last_device_removed_thought_timestamp = std::chrono::steady_clock::now();
static auto last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
namespace
{
	bool try_handle_device_equipped_event(const SKSE::ModCallbackEvent* a_event)
	{
		static const std::unordered_map<std::string_view, std::string_view> equipped_device_thoughts = {

			//  Head things here:
			{"DeviceEquippedGag", "YOU, the player, just got a gag placed in your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedHeavy Gag", "YOU, the player, just got a heavy gag placed in your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedTape", "YOU, the player, just got a tape gag placed on your mouth.  This gag prevents you from speaking or making any significant noise.  It is securely fastened so that you cannot simply spit it out.   "},
			{"DeviceEquippedMask", "YOU, the player, just got a locking face-mask or hood placed on your face.  This mask may prevent you from seeing or speaking clearly.  It is securely fastened so that you cannot simply remove it.   "},
			{"DeviceEquippedblindfold", "YOU, the player, just got a blindfold placed over your eyes.  This blindfold prevents you from seeing anything.  It is securely fastened so that you cannot simply remove it.   "},
			{"DeviceEquippedHeavy Blindfold", "YOU, the player, just got a heavy blindfold placed over your eyes.  This blindfold prevents you from seeing anything.  It is securely fastened so that you cannot simply remove it.   "},

			// Collar-and-Yoke-things here:
			{"DeviceEquippedCollar", "YOU, the player, just got a collar locked onto your neck.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedyoke", "YOU, the player, just got locked into an iron bondage yoke. Such a yoke is an iron bondage device, that locks around your neck and wrists, trapping your wrists in a position next to your shoulders, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			
			// Hand things here:
			{"DeviceEquippedGloves", "YOU, the player, just got locked into bondage gloves.  Such gloves prevent the use of your fingers, making it impossible to perform delicate tasks. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedBondage Mittens", "YOU, the player, just got locked into bondage gloves.  Such gloves prevent the use of your fingers, making it impossible to perform delicate tasks. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedArm Cuffs", "YOU, the player, just got locked into arm cuffs.  The arm cuffs wrap around your wrists.  In addition, these cuffs can be connected together, e.g. behind your back, to further immobilize and restrain you, but this hasn't happened yet.  These devices got locked onto you and now you cannot get of out them.   "},
			{"DeviceEquippedElbowbinder", "YOU, the player, just got locked into an elbow binder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRare Armbinder", "YOU, the player, just got locked into an armbinder, even the rare sort.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedarmbinder", "YOU, the player, just got locked into an armbinder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRogueBinder", "YOU, the player, just got locked into a rogue armbinder.  The binder holds your arms and hands in tight sleeves behind your back, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			
			// Torso things here:
			{"DeviceEquippedChastity Bra", "YOU, the player, just got a chastity bra locked onto your body.  This device is like a normal bra, but it is sturdy and prevents stimulation including self-stimulation and all access to your breasts.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedStraitJacket", "YOU, the player, just got locked into a strait jacket.  The jacket holds your arms and hands in tight sleeves bound around your torso, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedRope Harness", "YOU, the player, just got locked into a rope harness.  The rope harness wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedSuit", "YOU, the player, just got locked into a catsuit of sorts.  The suit wraps around your torso and limbs. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedHarness", "YOU, the player, just got locked into a harness.  The harness wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedChain Harness Wrist Shackles", "YOU, the player, just got a chain harness wrist shackles locked onto your wrists.  These shackles prevent you from moving your arms freely.  They are securely fastened so that you cannot simply remove them.   "},
			{"DeviceEquippedBoxbinder", "YOU, the player, just got locked into a box binder.  The box binder holds your arms and hands in a bag behind your torso, so that you are helpless and completely at the mercy of others. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedcorset", "YOU, the player, just got locked into a corsett.  The corsett wraps around your torso and constricts it a bit. This device got locked onto you and now you cannot get of out it.   "},
			


			// Crotch things here:
			{"DeviceEquippedClitoris Piercing", "YOU, the player, just got a clitoris piercing locked onto your clitoris.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedClitoral Piercing", "YOU, the player, just got a clitoris piercing locked onto your clitoris.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedNipple Piercings", "YOU, the player, just got a nipple piercing locked onto your nipples.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedPiercing", "YOU, the player, just got a nipple piercing locked onto your nipples.  Such a piercing may start to vibrate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedVaginal Plug", "YOU, the player, just got a vaginal plug locked into your vagina.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedVaginal Pear Plug", "YOU, the player, just got a vaginal pear plug locked into your vagina.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedAnal Plug", "YOU, the player, just got a anal plug locked into your anus.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedPlug", "YOU, the player, just got a anal plug locked into your anus.  Such a plug may start to inflate and deflate at the most inconvenient times and can be removed only be picking the lock.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedAnal Pear Plug", "YOU, the player, just got an Anal Pear Plug plugged into your ass.  This plug is plugged into the anus, and then it expands inside, so that you cannot remove it any more.  And it is locked in this state, locked onto your body and now you cannot get if out without somehow opening the lock first.   "},
			{"DeviceEquippedChastity Belt", "YOU, the player, just got a chastity belt locked onto your body.  This device is like a normal panties, but it is absolutely sturdy and prevents stimulation including self-stimulation and masturbation all access to your pussy.  On the one hand it keeps you from having intercourse with a man and thus also might prevent rape unless the attacker has the key, on the other hand you can't access your own sex this way, which might be very frustrating when you are very horny.  This device got locked onto you and now you cannot get if off.   "},
			{"DeviceEquippedBelt", "YOU, the player, just got a chastity belt locked onto your body.  This device is like a normal panties, but it is absolutely sturdy and prevents stimulation including self-stimulation and masturbation all access to your pussy.  On the one hand it keeps you from having intercourse with a man and thus also might prevent rape unless the attacker has the key, on the other hand you can't access your own sex this way, which might be very frustrating when you are very horny.  This device got locked onto you and now you cannot get if off.   "},

			// Feet things here:
			{"DeviceEquippedBoots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedPumps", "YOU, the player, just got locked into bondage pumps and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedBallet Boots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedIron Ballet Boots", "YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  They may have high heels and they may be severely restricting the speed at which you can move.   "},
			{"DeviceEquippedPony Boots", "YOU, the player, just got locked into pony boots.  These pony boots wrap around your legs shape them like a horse leg, with hooves and horseshoe and all. In addition to that and to further de-humanize the wearer, the pony boots are making a sound like a walking pony whereever you go. This device got locked onto you and now you cannot get of out it.   "},
			{"DeviceEquippedLeg Cuffs", "YOU, the player, just got locked into leg cuffs.  The leg cuffs wrap around your ankles.  In addition, these cuffs can be connected together, to further immobilize and restrain you and force you to make only tiny hobble steps, but this hasn't happened yet.  These devices got locked onto you and now you cannot get of out them.   "},
			{"DeviceEquippedRubber Socks", "YOU, the player, just got locked into locking rubber socks.  The rubber socks wrap around your feet.  These devices got locked onto you and now you cannot get of out them.   "},			

		};

		const std::string_view event_name = a_event->eventName.c_str();
		const auto it = equipped_device_thoughts.find(event_name);
		if (it == equipped_device_thoughts.end()) {
			return false;
		}

		if (a_event->strArg.c_str() == RE::PlayerCharacter::GetSingleton()->GetName()) {

			//  We POTENTIALLY throw out a thought message about the devices being equipped.  But we don't want to flood the queue.
			auto now = std::chrono::steady_clock::now();
			auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_device_equipped_thought_timestamp);
			LillithOnlyBox(std::format("Device-Equipped-Thought-Runtime-since-last-event: {}", runtime.count()));

			const int minimum_time_since_last_device_equipped_thought = 20;  // in seconds
			if (runtime.count() >= minimum_time_since_last_device_equipped_thought) {
				std::string final_thought_string = std::format("{}.  Say that in your response.", it->second);
				LillithOnlyBox(final_thought_string);
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string);
				last_device_equipped_thought_timestamp = now;
			}
		}

		return true;
	}

	bool try_handle_device_removed_event(const SKSE::ModCallbackEvent* a_event)
	{
		static const std::unordered_map<std::string_view, std::string_view> removed_device_thoughts = {
			//  Feet things here:
			{"DeviceRemovedBoots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedBallet Boots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedIron Ballet Boots", "YOU, the player, just got released from your bondage boots and you feet are finally free from them.  They may have had high heels and they may have been severely restricting the speed at which you could move, but now you are rid of them.   "},
			{"DeviceRemovedPony Boots", "YOU, the player, just got released from your pony boots.  Those boots had shaped your legs like horse legs and added hoof-like movement sounds, but these effects no longer apply now that they are removed.   "},
			
			
			{"DeviceRemovedyoke", "YOU, the player, just got released from your iron bondage yoke.  Your neck and wrists are no longer locked in that restrictive yoke posture, so those restraints no longer apply.   "},
			{"DeviceRemovedGloves", "YOU, the player, just got unlocked from gloves.  The gloves wrap around your hands like boxing gloves but with no thumb, so that you were unable to use your fingers for anything.  These devices got unlocked from your hands and now you can use your fingers again.   "},
			{"DeviceRemovedBondage Mittens", "YOU, the player, just got released from your bondage mittens.  Your fingers are no longer restrained by those mittens, so those restrictions no longer apply.   "},
			{"DeviceRemovedClitoris Piercing", "YOU, the player, just got your clitoris piercing removed.  That piercing can cause intense stimulation and inconvenient vibration, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedNipple Piercings", "YOU, the player, just got your nipple piercings removed.  Those piercings can cause intense stimulation and inconvenient vibration, but those effects no longer apply now that they are gone.   "},
			// NOTE:  These two again come with two different spellings.
			{"DeviceRemovedVaginal Plug", "YOU, the player, just got your vaginal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedPlugVaginal", "YOU, the player, just got your vaginal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			// Two different spellings again:
			{"DeviceRemovedAnal Plug", "YOU, the player, just got your anal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			{"DeviceRemovedPlugAnal", "YOU, the player, just got your anal plug removed.  That plug had imposed restrictive and intrusive stimulation effects, but those effects no longer apply now that it is gone.   "},
			
			
			
			{"DeviceRemovedCollar", "YOU, the player, just got your collar removed.  The feeling of being locked and restrained by that collar no longer applies now that it is gone.   "},
			{"DeviceRemovedChastity Bra", "YOU, the player, just got your chastity bra removed.  Access and stimulation restrictions on your breasts no longer apply now that the device is gone.   "},
			{"DeviceRemovedChastity Belt", "YOU, the player, just got your chastity belt removed.  The access and stimulation restrictions from that belt no longer apply now that it is gone.   "},
			{"DeviceRemovedBelt", "YOU, the player, just got your chastity belt removed.  The access and stimulation restrictions from that belt no longer apply now that it is gone.   "},
			
			
			{"DeviceRemovedStraitJacket", "YOU, the player, just got released from your strait jacket.  Your arms and hands are no longer bound against your torso, so those restraints no longer apply.   "},
			{"DeviceRemovedElbowbinder", "YOU, the player, just got released from your elbow binder.  Your arms are no longer forced behind your back in that restrictive position, so those restraints no longer apply.   "},
			{"DeviceRemovedRope Harness", "YOU, the player, just got released from your rope harness.  The constricting pressure around your torso no longer applies now that it is removed.   "},
			{"DeviceRemovedHarness", "YOU, the player, just got released from your harness.  The constricting and restrictive pressure from that harness no longer applies now that it is removed.   "},
			// NOTE:  These two again come with two different spellings.
			{"DeviceRemovedArmCuffs", "YOU, the player, just got your arm cuffs removed.  Your wrists are no longer cuffed and restrained, so those restrictions no longer apply.   "},
			{"DeviceRemovedArm Cuffs", "YOU, the player, just got your arm cuffs removed.  Your wrists are no longer cuffed and restrained, so those restrictions no longer apply.   "},
			{"DeviceRemovedWristRestraint", "YOU, the player, just got your wrist restraint removed.  Your wrists are no longer restrained, and you can move them freely now.   "},
			{"DeviceRemovedLeg Cuffs", "YOU, the player, just got your leg cuffs removed.  Your ankles are no longer cuffed and your movement is no longer restricted by them.   "},
			{"DeviceRemovedAnal Pear Plug", "YOU, the player, just got your anal pear plug removed.  The internal pressure and restrictive lock-in effects no longer apply now that it is gone.   "},
			{"DeviceRemovedGag", "YOU, the player, just got your gag removed.  Your mouth is no longer restrained, and you can speak and breathe freely again.   "},
			{"DeviceRemovedblindfold", "YOU, the player, just got your blindfold removed.  Your vision is no longer obstructed, so that sensory restriction no longer applies.   "},
			{"DeviceRemovedChain Harness Wrist Shackles", "YOU, the player, just got your chain harness wrist shackles removed.  Your wrists are no longer locked by those shackles, so those restraints no longer apply.   "},
		};

		const std::string_view event_name = a_event->eventName.c_str();
		const auto it = removed_device_thoughts.find(event_name);
		if (it == removed_device_thoughts.end()) {
			return false;
		}

		if (a_event->strArg.c_str() == RE::PlayerCharacter::GetSingleton()->GetName() && !it->second.empty()) {

			//  We POTENTIALLY throw out a thought message about the devices being removed.  But we don't want to flood the queue.
			auto now = std::chrono::steady_clock::now();
			auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_device_removed_thought_timestamp);
			const int minimum_time_since_last_device_removed_thought = 20;  // in seconds
			if (runtime.count() >= minimum_time_since_last_device_removed_thought) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::string(it->second));
				last_device_removed_thought_timestamp = now;
			}
		}

		return true;
	}
}

bool is_known_SUPERIRRELEVANT_mod_event(std::string event_name) {
		static const std::unordered_set<std::string> ignored_mod_events = {
		"SKIWF_widgetLoaded",
		"SeverActions_CellLoaded",
		"CBPCPlayerCollisionWithFemaleEvent"
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
		
		"CC_ModBimboCorruption",   // not now, maybe we deal with this later.

		// "SNMI_JustPumpMyStringToPlayerThought",             // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_IMPORANT_PlayerThought",                 // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought",        // we can't short-circuit that any more, because it should reset background thought cooldowns
		// "SNMI_Pump_AS_LITTERAL_AS_POSSIBLE_PlayerThought",  // we can't short-circuit that any more, because it should reset background thought cooldowns
		"SNMI_PlayerActivatedSomething",   //  This is our own event, to be picked up by SkyrimNet, so we don't need to respond to that.

		"SKICP_configManagerReady",
		"SKIWF_hudModeChanged", 
		"SKIWF_widgetLoaded", 
		"SKIWF_widgetManagerReady", 
		"SKIWF_iWantWidgetsReset", 
		"SKIWF_iWantStatusBarsReady", 
		"iWantStatusBarsReady", 
		"iWantWidgetsReset", 
		"ORS_LinkedWidgetUpdate",    // no clue what this is.
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
		"SKIWF_widgetError",            // this is broadcast when a widget error occurs.
		"SKICP_sliderSelected",         // more mod events that happen when changing something in the MCM.
		"SKICP_sliderAccepted",         // more mod events that happen when changing something in the MCM.
		"SKICP_dialogCanceled",         // more mod events that happen when changing something in the MCM.
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

		//  Mod events from SeverActions are directly integrated into SkyrimNet anyway, so no need to build a bridge for those in any way.
		"SeverActions_CellLoaded",
		"SeverActions_FamiliarityTimestamp",
		"SeverActions_ReputationAssess",
		"SeverActions_AmbientBanterReady",
		"SeverActions_ForcedCombatEnded",   // No need to respond to this, I guess?
		"SeverActions_NewTeammateDetected",
		"SeverActionsNative_FurnitureCleanup",
		"SeverActions_OrphanCleanup",
		"SeverActions_TeammateRemoved",
		"SeversHearth_CampTick",
		"SeverActions_LLM_RelAssess",
		"SeverActions_BrawlChallengeChoice",
		"SeverBrawl_Started",
		"SeverBrawl_Ended",
		"SeverActions_LLM_RepAssess",
		"SeverActions_VanillaFollowTopic",
		"SeverActions_ExternalWaitAdopt",
		"SeverActions_CombatSpoils",
		"SeverActions_CampMemberDied",
		"SeverActions_SetUIScale",
		"SeverActions_PrismaUI_RequestData",
		"SeverActions_BrawlChallengeExpired",
		"SeverActionsNative_SandboxCleanup",

		"SLOA_PlayerArousalUpdated",   // This update message is nice, but there is already a player response for that it seems.
		"SLOA_NPCArousalUpdated",      // This update message is nice, but there is no need to respond now.

		"BM-LPO_ViolationCheck",  // This is from Licenses - Player Oppression mod, but this is just the check, nothing for us to work with.  The actual violation might be something noteworthy.

		"ReSchlongify",
		"MME_MilkCycleComplete",
		"BeeingFemale",   //  We ignore this for now, maybe later we can do something with it.
		"CBPCPlayerCollisionWithFemaleEvent",
		"CBPCPlayerGenitalCollisionWithFemaleEvent",
		"PlayerChangedCells",
		"Obody_ApplyMorph",

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


		"AnimationStart_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"StageEnd_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"StageStart_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationEnding_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationEnd_HelplessCreature",   // This is technical Devious Helplessness / creature stuff, and doesn't warrant a separate comment.
		"AnimationChange_HelplessCreature", // hmmm, Devious Helplessness or Aroused Creatures??

		"OrgasmStart_HelplessFollower",  //	those two are both followers, I think.  MOD EVENT:  Name: OrgasmStart_HelplessFollower  StrArg: 1  NumArg: 0
		"OrgasmStart",                   //	those two are both followers, I think.  MOD EVENT:  Name: OrgasmStart  StrArg: 1  NumArg: 0	
		"OrgasmStart_slacEngagement",   //  Hmmm, not now, I guess.
		
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
		"SSL_CLEAR_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnding_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"AnimationEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
		"StageEnd_",                            //  This might be from The-Ancient-Profession.
		"StageStart_",                          //  This might be from The-Ancient-Profession.
		// "AnimationStarting_TAPPlayerFreelance", //  This might be from The-Ancient-Profession.  We need this one for a comment at animation overall start.
		"AnimationStart_TAPPlayerFreelance",    //  This might be from The-Ancient-Profession.
		"AnimationEnding_TAPPlayerFreelance",   //  This might be from The-Ancient-Profession.
		"AnimationEnd_TAPPlayerFreelance",      //  This might be from The-Ancient-Profession.
		"StageStart_TAPPlayerFreelance",        //  This might be from The-Ancient-Profession.
		"StageEnd_TAPPlayerFreelance",          //  This might be from The-Ancient-Profession.

		//  "AnimationStarting_BattleFuck",   //  This is from the BattleFuck mod.  This is the one event, that we respond to.
		"AnimationStart_BattleFuck",   //  This is from the BattleFuck mod.  This is 4 seconds after the other, so we ignore this one and respond to the other one.
		"StageStart_BattleFuck", // This is from the BattleFuck mod.  Since this is about undressing itself, we won't stop clothing changes from this one.
		"StageEnd_BattleFuck",   // This is from the BattleFuck mod.  Since this is about undressing itself, we won't stop clothing changes from this one.
		"AnimationEnd_BattleFuck",   // This is from the BattleFuck mod.  nothing to do here.
		//  "AnimationEnding_BattleFuck", // This is from the BattleFuck mod.  We absolutely should comment on that.

		"AnimationStarting_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"AnimationStart_JailRapeNPC",  // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"AnimationChange_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"OrgasmStart_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"StageStart_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"StageEnd_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"AnimationEnding_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.
		"AnimationEnd_JailRapeNPC", // This is from the JailRape mod, but this is about some NPCs somewhere in the background, possibly quite far off, so IGNORE for our purposes.

		// "AnimationStarting_JailRapePC", // This is from the JailRape mod. 
		"AnimationStart_JailRapePC", // This is from the JailRape mod.  but animation starting event is separate and already all we need.
		"StageEnd_JailRapePC", // This is from the JailRape mod.  but animation starting event is separate and already all we need.
		
		"AnimationStart_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationStarting_CreatureSummoner", // This is from the Creature Summoner mod.
		"StageStart_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationChange", // This is from the Creature Summoner mod.
		"AnimationChange_CreatureSummoner", // This is from the Creature Summoner mod.
		

		"AnimationEnding_CreatureSummoner", // This is from the Creature Summoner mod.
		"AnimationEnd_CreatureSummoner", // This is from the Creature Summoner mod
		"StageEnd_CreatureSummoner", // This is from the Creature Summoner mod

		"AnimationStart_slacEngagement",   // 4 seconds after AnimationStarting_....
		"StageEnd_slacEngagement",
		// "AnimationStarting_slacEngagement",  
		"StageStart_slacEngagement",
		"AnimationChange_slacEngagement",
		"AnimationEnding_slacEngagement",
		"AnimationEnd_slacEngagement",

		"ActorChangeStart",                  //  This *might* be relevant, if that has some extra detail about the current SL scene and changes there, but it's just not a priority now.
		"ActorChangeStart_slacEngagement",   //  This *might* be relevant, if that has some extra detail about the current SL scene and changes there, but it's just not a priority now.

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
		// "AnimationStart_BodySearch" is intentionally excluded: body search itself is about clothing.
		"AnimationStarting_slacEngagement",
		"AnimationStart_slacEngagement",
		"StageStart_slacEngagement",
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
		"AnimationEnding_TAPPlayerFreelance",
		"AnimationEnd_TAPPlayerFreelance",
		"AnimationEnding_CreatureSummoner",
		"AnimationEnd_CreatureSummoner",
		"AnimationEnd_MatchMaker",
		"AnimationEnding_MatchMaker",
		"AnimationEnding_slacEngagement",
		"AnimationEnd_slacEngagement",
		"AnimationEnding_HelplessCreature",   //  This is from Aroused Creatures (I think)
		"AnimationEnd_HelplessCreature",      //  This is from Aroused Creatures (I think)
		"AnimationEnd_Helpless",    // This is from Devious Helplessness.
		"AnimationEnding_JailRapePC",
		"AnimationEnd_JailRapePC",

	};

	const std::string_view event_name = a_event->eventName;

	if (scene_start_events.contains(event_name))
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("Mod-Event-Based DISABLING OF CLOTHING-CHANGE-COMMENTS: {}  StrArg: {} ", a_event->eventName.c_str(), a_event->strArg.c_str());  
		set_current_animation_status("in_a_scene");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if (scene_end_events.contains(event_name))
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("Mod-Event-Based RE-ENABLING OF CLOTHING-CHANGE-COMMENTS: {}\n{}\n.", a_event->eventName.c_str(), a_event->strArg.c_str());  
		set_current_animation_status("not_in_a_scene");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
}

void handle_mod_event_broadcasts(const SKSE::ModCallbackEvent* a_event)
{

	if ( is_known_SUPERIRRELEVANT_mod_event(a_event->eventName.c_str())) {
		// This mod event is so frequent it clutters up the log even if we just dedicate one line to it, so we will just dismiss this one silently.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	toggle_in_a_scene_or_not_based_on_mod_events(a_event);
	

	if ( is_known_useless_event_that_can_be_completely_shortcircuited(a_event->eventName.c_str()))
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info("SKIPPING HANDLING OF IRRELEVANT MOD EVENT: Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	if ( (std::strcmp(a_event->eventName.c_str() , "SNMI_JustPumpMyStringToPlayerThought") == 0)  | 
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_IMPORANT_PlayerThought") == 0) |
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought") == 0) |
		(std::strcmp(a_event->eventName.c_str() , "SNMI_Pump_AS_LITTERAL_AS_POSSIBLE_PlayerThought") == 0) ) 
	{
		// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
		logger::info(".\n.\n.\nWe really push out a thought now.  From: {}\n{}\n.", a_event->eventName.c_str(), a_event->strArg.c_str());  
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// We log all other mod events, because they might be interesting for us to react to and turn into immersive player thoughts
	logger::info("MOD EVENT:  Name: ''{}''  StrArg: ''{}''  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
	std::string debug_message = std::format("MOD EVENT:  Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg );


	// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
	if ( (std::strcmp(a_event->eventName.c_str() , "UD_SentientDialogue") == 0)  ) {
		// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		std::string  thought_message = std::format("YOU, the player, suddenly have a feeling like your {} is speaking to you, even though it is just an item and not a living creature.  Is it maybe time to question your sanity?  What is going on?  You have no clue, but you suspect it's some sentient device speaking to you.  Say so in your response. ", a_event->strArg.c_str());
		DumpThoughts::throw_out_TTS_thought_message(thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	if (try_handle_device_equipped_event(a_event)) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	if (try_handle_device_removed_event(a_event)) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}





	// MOD EVENT:  Name: DeviousEventTrip and Fall  StrArg: Beea  NumArg: 0
	if ( (std::strcmp(a_event->eventName.c_str() , "DeviousEventTrip and Fall") == 0) ||  (std::strcmp(a_event->eventName.c_str() , "DeviousEventLeg Cuffs Trip Over") == 0) ) {
		// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		std::string  thought_message = std::format("YOU, the player, just tripped over your own feet, because you are wearing these devious bondage boots or leg cuffs that you are locked into and cannot take off.  What are you thinking now based on this? ", a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// MOD EVENT:  Name: DeviousEventStruggle  :  this seems to be a random forced struggle event with no real struggle but just for roleplay.  We make a comment.
	if ( (std::strcmp(a_event->eventName.c_str() , "DeviousEventStruggle") == 0)  ) {
		if (a_event->strArg.c_str() == RE::PlayerCharacter::GetSingleton()->GetName() ) {
			std::string  thought_message = std::format("YOU, the player, just can't take it any more.  In a frenzy, you must get out of your bondage items now.  Say so and let us know what you are feeling and thinking.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviousEventStruggle: Event noticed, but it's NOT ABOUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}	
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	
	// MOD EVENT:  Name: DeviceActorOrgasm  StrArg: Lillith  NumArg: 0
	if ( (std::strcmp(a_event->eventName.c_str() , "DeviceActorOrgasm") == 0)  ) {
		if (a_event->strArg.c_str() == RE::PlayerCharacter::GetSingleton()->GetName() ) {
			std::string  thought_message = std::format("YOU, the player, just orgasmed from the vibrating devices locked onto your body and into your sensitive parts.  You couldn't prevent it.  Gods, that was intense!  Say so and let us know what you are feeling and thinking.");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		} else {
			LillithOnlyBox(std::format("DeviceActorOrgasm: Event noticed, but it's NOT ABUT THE PLAYER?????  DoubleCheck this next time.  Actor is: {}", a_event->strArg.c_str()));
		}	
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	
	// MOD EVENT:  Name: SkyrimNetDDUDNG_Event  StrArg: Lillith's Genital Piercing (Common Soul Gem) stops vibrating.  NumArg: 0
	// MOD EVENT:  Name: SkyrimNetDDUDNG_Event  StrArg: Lillith's Genital Piercing (Common Soul Gem) stops vibrating.  NumArg: 0
	// Lillith's Genital Piercing (Common Soul Gem) starts vibrating.
	if ( (std::strcmp(a_event->eventName.c_str() , "SkyrimNetDDUDNG_Event") == 0)  ) {
		if ( (a_event->strArg == "Lillith's Genital Piercing (Common Soul Gem) stops vibrating." ) ||
		     (a_event->strArg == "Lillith's Genital Piercing (Common Soul Gem) starts vibrating.") ) {
			// do nothing here.  This is already a known event.  But we are specific and want to see the rest of them.
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		} else {
			std::string thought_message = std::format("SkyrimNetDDUDNG_Event: ''{}''", a_event->strArg.c_str() );
			// These events are numerous and mostly give text descriptions of what is happening concerning UD/DD device equipments.
			// LillithOnlyBox(thought_message);
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}	
	}



	// MOD EVENT:  Name: AnimationStarting_BattleFuck :  this is the start of a BattleFuck scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BattleFuck") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are now getting ambushed in a sexual assault.  Someone gips you from behind and wants to strip away your clothing and armour, so he can the rape you right in from of everybody.  There may be bystanders who come to watch the spectacle as you are potentially getting raped.  You may struggle to resist, but it's unclear if that will work!  Say so and let us know what you are feeling and thinking in that moment, given that you are about to be stripped and raped and make it clear from your response, that a rape is about to happen to you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_BattleFuck:  " + thought_message);
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


	// MOD EVENT:  YPS Thoughts:  Those will be pushed to the background channel.
	if ( (std::strcmp(a_event->eventName.c_str() , "YPS_ThoughtEvent") == 0)  ) {
		// std::string  thought_message = std::format(a_event->strArg.c_str());
		DumpThoughts::throw_out_AS_LITTERAL_AS_POSSIBLE_thought_message(a_event->strArg.c_str());   // this shouldn't be overdone, but hte background code makes sure of that.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// MOD EVENT:  Generic orgasm start (and end)  [2026-05-21 21:44:52.579] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmStart  StrArg:   NumArg: 0  // [2026-05-21 21:45:00.613] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmEnd  StrArg:   NumArg: 0
	if ( (std::strcmp(a_event->eventName.c_str() , "PlayerOrgasmStart") == 0)  ) {
		// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
		std::string  thought_message = std::format("Regardless whether you like it or not, from all the stimulation, you, the player, are now suddenly having an orgasm! Let us know this via your response. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	





	// MOD EVENT:  From SpankThatAss, we have the following event (running up and spanking, in contrast to bump-spanks, which seem not to trigger any mod event unfortunately)
	if ( (std::strcmp(a_event->eventName.c_str() , "_STA_RandomRunUpAndSpankComplete") == 0)  ) {
		// Name: _STA_RandomRunUpAndSpankComplete  StrArg:   NumArg: 0
		std::string  thought_message = std::format("Since you were busy focussing on crafting, someone used the opportunity and just ran up behind you and spanked your ass with full force while you were distracted! Let us know your response to that, and make sure you implicitly explain that your ass was just slapped hard in your response as well. If you can guess who it was from the context, feel free to mention the culprit as well. You can even tell them to stop, if you want.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}		
	// MOD EVENT:  From SpankThatAss, we have the following event:  spanking of any kind, and then we have this resistance loss by one, but we don't do anything on that for now)
	if ( (std::strcmp(a_event->eventName.c_str() , "DF-ResistanceLoss") == 0)  ) {
		// Name: DF-ResistanceLoss  StrArg:   NumArg: 1
		// std::string  thought_message = std::format("Someone just spanked your ass with full force or groped your tits outright! Let us know your response. ");
		// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}		

	// MOD EVENT:  From BodySearch, we have the following event:  AnimationStarting_BodySearch
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_BodySearch") == 0)  ) {
		std::string  thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But now the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. Let us know your response to that, and make sure you implicitly explain that you are being groped for pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		// More in this context:
		// StageStart_BodySearch
		// 4 seconds later:  AnimationStart_BodySearch
		// StageEnd_BodySearch
		// SSL_PREPARE_Thread1
		// SSL_LOCK_Thread1
		// AnimationStarting_slacEngagement
		// StageStart_slacEngagement
		// AnimationStart_slacEngagement
		// StageEnd_slacEngagement
		// SSL_CLEAR_Thread1
		// AnimationEnding_slacEngagement
	}	
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_BodySearch") == 0)  ) {
		std::string  thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But now the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. That has been going on for a while.  And now he is continuing to grope you even more as he pleases. Let us know your response to that. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		// More in this context:
		// StageStart_BodySearch
		// 4 seconds later:  AnimationStart_BodySearch
		// StageEnd_BodySearch
	}		
	if ( (std::strcmp(a_event->eventName.c_str() , "StageEnd_BodySearch") == 0) 
	|| (std::strcmp(a_event->eventName.c_str() , "AnimationStart_BodySearch") == 0) 
	|| (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BodySearch") == 0) ) {
		// nothing to do here, just exit.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnding_BodySearch") == 0)  ) {
		std::string  thought_message = std::format("A guard has just brought you to their guards baracks, saying he needs to do a body search.  But then the search turns out to be mainly him groping your body everywhere for his pleasure and amusement. Finally now, the guard has finished groping you under the pretext of a body search and is finally letting go of you.  Let us know your response to that.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	
	// MOD EVENT:  From some animal mod, creature maybe, we have the following event:  AnimationStarting_slacEngagement
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_slacEngagement") == 0)  ) {
		std::string  thought_message = std::format("A creature, an animal or a monster, has just managed to take advantage of you and start a sexual encounter with you, and you somehow couldn't resist or didn't resist and submitted into the sexual encounter.  Let us know your response to that, and make sure you mention or implicitly point out, that you are having sex with a creature. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		// More in this context:
		// StageStart_slacEngagement
		// 4 seconds later:  AnimationStart_slacEngagement
		// StageEnd_slacEngagement
		// SSL_PREPARE_Thread1
		// SSL_LOCK_Thread1
		// AnimationStarting_slacEngagement
		// StageStart_slacEngagement
		// AnimationStart_slacEngagement
		// StageEnd_slacEngagement
		// SSL_CLEAR_Thread1
		// AnimationEnding_slacEngagement
	}	
	
	// MOD EVENT:  From The Ancient Profession mod, we have the following event:  AnimationStarting_TAPPlayerFreelance
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_TAPPlayerFreelance") == 0)  ) {
		std::string  thought_message = std::format("You just managed to successfully prostitute yourself to a man and were paid the usual price of this profession.  You are now starting a sexual encounter with him, like a normal prostitute would.  Let us know your response to that, and make sure you implicitly explain that you are letting him fuck you and use you for his pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		// More in this context:
		// StageStart_slacEngagement
		// 4 seconds later:  AnimationStart_slacEngagement
		// StageEnd_slacEngagement
		// SSL_PREPARE_Thread1
		// SSL_LOCK_Thread1
		// AnimationStarting_slacEngagement
		// StageStart_slacEngagement
		// AnimationStart_slacEngagement
		// StageEnd_slacEngagement
		// SSL_CLEAR_Thread1
		// AnimationEnding_slacEngagement
	}	

	// MOD EVENT:  From some animal mod, creature maybe, we have the following event:  AnimationStarting_HelplessCreature
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_HelplessCreature") == 0)  ) {
		std::string  thought_message = std::format("You just were overcome and helplessly submit to a creature wanting sex with you.  You give up and just give in and are now starting a sexual encounter with it.  Let us know your response to that, and make sure you implicitly explain that you are letting yourself get fucked and used for its pleasure in your response as well. ");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// MOD EVENT:  Name: AnimationStarting_Helpless :  this is the start of a Devious-Helplessness-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_Helpless") == 0)  ) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string  thought_message = std::format("YOU, the player, are currently still helpless in your bondage gear and at the mercy of others.  But now someone is coming to take advantage of your helplessness and is going to use your body for sex and breeding and there is nothing you can do about it due to being bound in bondage gear.  You can only cry out in desperation and fear.  Let us feel your fear of being used for sex in this way in your response and let us know that it is too late and the sexual assault is going to happen.  Announce that in your response, so that the player is alerted to the situation.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_Helpless:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  Name: AnimationChange_Helpless :  this from a Devious-Helplessness-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationChange_Helpless") == 0)  ) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string  thought_message = std::format("YOU, the player, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding and there was nothing you were able to do to prevent that.  But now the attacker wants sex in yet another position, while you are still completely helpless and can do nothing to prevent your own abuse and rape.  Announce that in your response.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationChange_Helpless:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  Name: StageStart_Helpless :  this from a Devious-Helplessness-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_Helpless") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding.  And now he (or she) is continuing to fuck you.  Say that in your response.");

		// We need a cooldown here again for this part, because it might flood the queue too much otherwise.
		if (std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp < std::chrono::seconds(20)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: StageStart_Helpless because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp).count());
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this might need a cooldown.
		LillithOnlyBox("StageStart_Helpless:  " + thought_message);
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}		
	// MOD EVENT:  Name: AnimationEnd_Helpless :  this is the end of a Devious-Helplessness-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationEnd_Helpless") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, have been trapped in bondage gear and were helpless all the time.  And now an attacker has exploited you and used your body for sex and breeding and there was nothing you were able to do to prevent that.  But now your ordeal is over and the attacker has finally let go of you.  You are still trapped in your bondage gear, but at least the attacker stopped raping you.  Say so in your response and let us know how you feel now.");


		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationEnd_Helpless:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  the rest of the sequence, where we don't do anything and just return on those events.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStart_Helpless") == 0)  ||
	(std::strcmp(a_event->eventName.c_str() , "AnimationStarting_Helpless") == 0) ||
	(std::strcmp(a_event->eventName.c_str() , "StageStart_Helpless") == 0) ||
	(std::strcmp(a_event->eventName.c_str() , "StageEnd_Helpless") == 0) ||
	(std::strcmp(a_event->eventName.c_str() , "AnimationEnding_Helpless") == 0) ) {
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// The sequence is:
	// DONE:  AnimationStarting_Helpless
	// NOTHINGTODO:  AnimationStart_Helpless
	// NOTHINGTODO:  StageStart_Helpless	
	// NOTHINGTODO:  StageEnd_Helpless
	// DONE:  AnimationChange_Helpless
	// NOTHINGTODO:  StageStart_Helpless	
	// NOTHINGTODO:  StageEnd_Helpless
	// NOTHINGTODO:  AnimationEnding_Helpless
	// DONE:  AnimationEnd_Helpless




	// MOD EVENT:  Name: AnimationStarting_JailRapePC :  this is the start of a Jailrape-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationStarting_JailRapePC") == 0)  ) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string  thought_message = std::format("YOU, the player, are imprisoned by the guards.  Now one of them is going to rape you for his own fun and pleasure. You can only cry out in desperation and fear about what is to come.  Announce that in your response, so that the player is alerted to the situation.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationStarting_JailRapePC:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  Name: AnimationChange_JailRapePC :  this from a Jailrape-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "AnimationChange_JailRapePC") == 0)  ) {
		// This event is always about the player, nobody else.
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		std::string  thought_message = std::format("YOU, the player, are imprisoned by the guards.  One of them has already used you for his own fun and pleasure. But now he wants even more sex. He wants to try even more different sex positions with you. And to use your body in yet more ways. You cannot stop him from doing what he wants with you.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("AnimationChange_JailRapePC:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	
	// MOD EVENT:  Name: StageStart_JailRapePC :  this from a Jailrape-Scene.  We absolutely should comment on it.
	if ( (std::strcmp(a_event->eventName.c_str() , "StageStart_JailRapePC") == 0)  ) {
		// This event is always about the player, nobody else.
		std::string  thought_message = std::format("YOU, the player, are imprisoned by the guards.  One of them has already used you for his own fun and pleasure. But he wants even more sex. You are forced to play along and do what he wants. You cannot stop what is happening to you, because the attacker is too strong. You can try to resist, but that might make him even more aggressive. You can try not to get excited from the sexual stimulation of your body, but even that is becoming more difficult, and you can slowly feel yourself getting involuntarily more sexually excited. Or you can start to break and start to submit and lose your will to resist entirely, accepting the guards as your new masters, and accepting that it is better to obey them than face more punishment and hoping, that if you can please the guards better, they might let you go and not be mean to you any more. ");

		// We need a cooldown here again for this part, because it might flood the queue too much otherwise.
		if (std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp < std::chrono::seconds(15)) {
			SKSE::log::info("=====SKIPPING MOD EVENT: StageStart_JailRapePC because of cooldown.  Last thought was {} seconds ago.", std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_devious_helplessness_thought_timestamp).count());
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this might need a cooldown.
		LillithOnlyBox("StageStart_JailRapePC:  " + thought_message);
		last_devious_helplessness_thought_timestamp = std::chrono::steady_clock::now();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}	

	// The sequence of License-PlayerOppression mod events is:
	// 1.  BM-LPO_ViolationFound
	// 2.  BM-LPO_BountyStart  (4 seconds later)
	// 3.  BM-LPO_BountyEnd  (30 seconds later when running away from the guards)
	// MOD EVENT:  YPS initialization:  NOTE:  This is NOT the real handling, it's just capturing these at game startup, which is otherwise annoying
	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_ViolationFound") == 0) ) {			
		std::string  thought_message = std::format("The game has rather weird license regulations now. This is part of player oppression mod.  At this point, YOU as the player character, can see a guard approaching you. They want to catch you and give you a fine for some ridiculous license regulation or maybe even throw you in jail.  Announce that they are coming for you with your response, so that the player is alerted to the situation, and maybe even explain, that it is probably because of license regulation, as the player might otherwise not understand what is going on.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("Licenses-PlayerOppressionMod:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_BountyStart") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "BM-LPO_BountyEnd") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "BM-LPO_BountyStart") == 0) ) {			

		// We do nothing here, on purpose for now, since the Violation-Found event should already be enough.
		return;
	}
	// When you get a weapons license:  BM-LPO_WeaponLicense_20_Activate
	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_WeaponLicense_20_Activate") == 0) ) {			
		std::string  thought_message = std::format("The game has rather weird license regulations now. This is part of player oppression mod.  At this point, YOU as the player character, received your new weapons license.  Now you can (finally) carry your weapons out in the open as you please.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("Licenses-PlayerOppressionMod:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_BikiniLicense_20_Activate") == 0) ) {			
		std::string  thought_message = std::format("The game has rather weird license regulations now. This is part of player oppression mod.  At this point, YOU as the player character, received your new bikini license.  Now you can (finally) wear your bikini armor in public as you please, but this may still be a bit degrading, because this license doesn't allow you full armor and real armour, it only allows for skimpy bikini armour, making you a spectacle for male observers.  But still, it's better than nothing and the armor still gives you some protection, even if it's not as much as real armor would.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("Licenses-PlayerOppressionMod:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_CraftingLicense_20_Activate") == 0) ) {			
		std::string  thought_message = std::format("The game has rather weird license regulations now. This is part of player oppression mod.  At this point, YOU as the player character, received your new crafting license.  Now you can (finally) craft items in public as you please, which everybody else seems to be doing without much of a license, or at least you haven't seen one.  Could it be that you are being player and just harassed by the guards?  But at least you can now craft items in public without being harassed by guards and fined for it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("Licenses-PlayerOppressionMod:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	if ( (std::strcmp(a_event->eventName.c_str() , "BM-LPO_TradingLicense_20_Activate") == 0) ) {			
		std::string  thought_message = std::format("The game has rather weird license regulations now. This is part of player oppression mod.  At this point, YOU as the player character, received your new trading license.  Now you can (finally) trade items in other than food or jewelry items, which everybody else seems to be doing all the time without much of a license, or at least you haven't seen one.  Could it be that you are being played and just harassed by the guards?  But at least you can now trade items in public without being harassed by guards and fined for it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("Licenses-PlayerOppressionMod:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}


	if ( (std::strcmp(a_event->eventName.c_str() , "DF-ResistanceLossWithSeverity") == 0) ) {			
		std::string  thought_message = std::format("The NPCs in this game are rather mean and mean-spirited towards you and they seem to be putting you down all the time.  They are just so stupid and annoying that it's wearing you down, sucking your mental energy from you.  Maybe they want to see you fail, so that they can control and dominate you and do what they want with you when you have no power to resist any more.  In any case, they are slowly wearing you out and you may not be able to take it any more at some point.  Tell us how you feel about that.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		LillithOnlyBox("DF-ResistanceLossWithSeverity:  " + thought_message);
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	


	// MOD EVENT:  IF there was other SkyrimNetSpeech or thoughts, we restart our pause tracking, to not overflow the BACKGROUND TTS channel with too much content for the listener.  There should also be a little bit of pause and quiet here and there.
	if ( (std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechComplete") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechCompleted") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechStarted") == 0)  ||
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioStarted") == 0) ||
		(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioEnded") == 0)  ) {			

		// Let's also check, if it was player thoughts or player diagloge
		
		auto now = std::chrono::steady_clock::now();
		// auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_speech_timestamp);
		auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - DumpThoughts::GetLastSpeechTimestamp());

		SKSE::log::info("=====NO-SPEECH-TIMER for the BACKGROUND CHANNEL WAS RESET BY SKYRIMNET ModEvent after {} seconds. ", runtime.count());
		// last_speech_timestamp=std::chrono::steady_clock::now();
		DumpThoughts::reset_last_speech_timestamp();
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}
	// 	|| (std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechComplete") == 0)
	
	// MORE THINGS TO HANDLE: Name: yps_AddictionBuffChange  StrArg:   NumArg: 6

	// MORE THINGS TO HANDLE: Name: yps_FashionChange  StrArg: FingerNailPolish  NumArg: 0


	// MOD EVENT:  YPS initialization:  NOTE:  This is NOT the real handling, it's just capturing these at game startup, which is otherwise annoying
	if ( (std::strcmp(a_event->eventName.c_str() , "yps_HairStageChange") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "yps_FashionChange") == 0)  || 
		(std::strcmp(a_event->eventName.c_str() , "yps_AddictionBuffChange") == 0) ) {			

		// Let's also check, if it was player thoughts or player diagloge
		
		SKSE::log::info("WE JUST CHECKED AND FOUND {} seconds since game load.", DumpThoughts::seconds_since_game_load());
		if (DumpThoughts::seconds_since_game_load() < 180) {
			
			// This is just the initial setup of the YPS mod, which we don't care about.
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		// otherwise:  Mention this unhandled mod event, as usual.
	}




	
	// CODE-MARKER:  THIS IS THE ENTRY POINT FOR MORE MOD EVENTS TO BE HANDLED!!!!! kkkk



	
	// IF the MOD-EVENT really WASNT HANDLED BY THIS POINT, IT IS MAYBE SOMETHING NEW, AND THEREFORE WE MAKE A MESSAGEBOX-ANNOUNCEMENT OF it.
	LillithOnlyBox("An unhandled mod-event was discovered: " + debug_message);
	logger::info("An unhandled mod-event was discovered: {}" , debug_message );

	// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
	/*  IN THE END, WE CANNOT USE THIS, BECAUSE IT GETS TRIGGERED ALL THE TIME FROM E.g. UD ELLBOW BINDER NON-STOP FROM THE IDLE ANIMATION.
		And the magnitude is also almost the same as when falling to the floor instead.  That's a shame.

	if ( (std::strcmp(a_event->eventName.c_str() , "CBPCPlayerCollisionWithFemaleEvent") == 0)  ) {
		// In the CBPS mode, there are these collision configs below, but the str-args observed in real life so far were:  L Breast01
		// [NPC L Breast] [NPC R Breast] [NPC L Butt] [NPC R Butt]
		std::string  thought_message = std::format("YOU, the player, just took a hard hit one of your body parts.  You can guess which one it is from this string: '{}'. This was probably very painful. Say as much in your response, also mentioning the respective body part.  What are you thinking now based on this? ", a_event->strArg.c_str());
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		return RE::BSEventNotifyControl::kContinue;
	}
	*/
}
