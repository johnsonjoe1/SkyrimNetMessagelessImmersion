#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include <string_view>
#include <unordered_set>

std::string general_word_on_milk_pumps = R"SKSE(
A general word on Milk Pumps: Milk pumps in Skyrim are milking stalls, much like for a milk cow, only that this device is designed for human women.
The position is the same as a cow though, on all fours, and milk pumps attach from below and suck the milk from the woman's breasts.
The milk pump is a heavy, intricate contraption of dark wood, leather, and polished brass, built with a strange mix of craftsmanship and clinical purpose. 
Thick straps and padded supports hold the body steady while slender tubes and softly pulsing pumps work with relentless rhythm, drawing milk into waiting glass containers below. 
Valves hiss quietly, gears click in measured cadence, and the whole machine seems almost alive in the way it breathes and churns with mechanical patience. 
Despite its intimidating appearance, every curve and restraint is shaped for long use, worn smooth by countless sessions beneath the pump’s steady, inescapable pull.
)SKSE";

std::string standard_thought_instruction = R"SKSE(
Now your task is this:  Comment on this, with thoughts of YOU as the player. How does the situation make you feel? What are you thinking now?";
)SKSE";

std::string milk_pump_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are lowering yourself into a so-called milk pump as a subject for extraction of milk from your breasts. 
Milk pumps in Skyrim are milking stalls, much like for a milk cow, only that this device is designed for human women.
The position is the same as a cow though, on all fours, and milk pumps attach from below and suck the milk from the woman's breasts.
Moreover, you will be fittet with a so-called Milking Cuirass, which is like a piece of armor, that is designed to be suck milk from your breasts and also features humiliating cow design
and leaves your body almost completely naked otherwise.
)SKSE" + general_word_on_milk_pumps + standard_thought_instruction;

std::string milk_pump_prompt_2 = R"SKSE(
Now the situation is this: YOU, the player, just finished subjecting yourself into a so-called milk pump, as a subject for extraction of milk from your breasts. 
Your mana and stamina have probably been completely drained. And there is probably no more milk left in your breasts, so you are probably feeling empty and exhausted. 
Also, there might be residual pain in your nipples, and of course the humiliation of being treated like a cow and milked in this contraption might be overwhelming.
The Milking Cuirass you were fitted with, which is like a piece of armor, that is designed to be suck milk from your breasts and also features humiliating cow design 
is removed and you get back into your outfit from before.
)SKSE" + general_word_on_milk_pumps + standard_thought_instruction;

std::string blacksmith_forge_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Blacksmith Forge. 
Blacksmith Forges allow you to craft entire new weapons and armor or also other small useful items for everyday use, but you must have the right materials 
and the nescessary smithing skill to do so.
)SKSE" + standard_thought_instruction;

std::string grindstone_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Grindstone. 
Grindstones allow you to sharpen and improve your weapons.  You must have the right materials 
and the necessary smithing skill to do so.  It is not possible to create new items here. The Grindstone is only for improvements.
)SKSE" + standard_thought_instruction;

std::string workbench_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Workbench. 
Workbenches allow you to improve your armour items.  You must have the right materials 
and the necessary smithing skill to do so.  It is not possible to create new items here. The Workbench is only for improvements.
)SKSE" + standard_thought_instruction;

std::string tanning_rack_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Tanning Rack. 
Tanning Racks allow you to break down items from your inventory into basic leather or leather stripes or simple components like that.
If you have raw hides or old leather items, you can make basic leather components from them. You may also be able to craft clothing items.
)SKSE" + standard_thought_instruction;

std::string smelter_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Smelter. 
Here you can turn in your raw ores, weapons or armour to smelt and disassemble them into fresh raw materials to be used later as a source for new items or improvements at the Blacksmith forge, the Workbench or the Grindstone.
You must have the right materials to do so.
)SKSE" + standard_thought_instruction;

std::array<std::string, 5> furniture_prompt_list = {
    blacksmith_forge_prompt_1,
    grindstone_prompt_1,
    workbench_prompt_1,
    tanning_rack_prompt_1,
    smelter_prompt_1
};

std::array<std::string, 5> furniture_prompt_triggers = {
    "Blacksmith Forge",
    "Grindstone",
    "Workbench",
    "Tanning Rack",
    "Smelter"  // Arcane Enchanter  // Alchemist's Retort  // Alchemy Lab  //  Staff Enchanter   // Bench   // other seats, what about beds and that?
};

int get_furniture_list_index(std::string_view keyword)
{
	for (std::size_t i = 0; i < furniture_prompt_triggers.size(); ++i)
	{
		if (keyword == furniture_prompt_triggers[i])
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

void handle_furniture_item_activation(RE::TESBoundObject *base)
{
	SKSE::log::info("That activated object seems to be furniture, so we can proceed.");

	// We want to broadcast mod events.  So we need this event source.
	auto eventSource = SKSE::GetModCallbackEventSource();

	// Since the activation event happend, we can start broadcasting the mod event right away.
	if (eventSource)
	{
		// auto furniture_name = base->GetName();
		const char* furniture_name = base->GetName();
		if (!furniture_name) {
			SKSE::log::error("[SkyrimNetMessagelessImmersion] The furniture name was null!  Emergency abort!");
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		std::string  mod_event_name = "Nothing so far";
		std::string  mod_event_string_arg = "Mod event string not set yet";

		if (std::strcmp(furniture_name , "Milk Pump") == 0) {
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Milk Pump!  THIS GETS A SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is using furniture
				mod_event_string_arg = milk_pump_prompt_2;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 2nd Event for Milk Pump! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 2nd Event for Milk Pump! {} " , mod_event_string_arg );
			}		else {
				mod_event_string_arg = milk_pump_prompt_1;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for Milk Pump! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for Milk Pump! {} " , mod_event_string_arg );
			}
			mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
		} else if (get_furniture_list_index(furniture_name) != -1) {
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a piece of furniture that is in our list of special furniture!  This gets a SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
		
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is already using furniture, so this is probably the second event.  No comment on this for all the furnitures in this standard list.
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a furniture from the list of special furniture, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
			} else {
				mod_event_string_arg = furniture_prompt_list[get_furniture_list_index(furniture_name)];
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for this furnitures item (because player not in a furniture already! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for this furniture item (because player not in a furniture already)! {} " , mod_event_string_arg );
			}
			mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
		} else {

			//  LATER:  Add special treatment for:  Bench
			//  LATER:  Add special treatment for:  Wood Chopping Block


			mod_event_name = "SNMI_PlayerActivatedSomething";
			mod_event_string_arg = furniture_name;
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated SOMETHING THAT IS NOT A MILK PUMP AT ALL!!  This gets normal treatment.");
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is already using furniture, so this is probably the second event.  No comment on this in the blacksmith forge case.
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated SOME FURNITURE ITEM, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
			}	else {

				std::string generic_furniture_prompt_1 = R"SKSE(Now the situation is this: YOU, the player, are about to use a )SKSE";
				generic_furniture_prompt_1 = generic_furniture_prompt_1 + furniture_name;
				generic_furniture_prompt_1 = generic_furniture_prompt_1 + R"SKSE(. )SKSE" + standard_thought_instruction;

				mod_event_string_arg = generic_furniture_prompt_1;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for SOME FURNITURE ITEM! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for SOME FURNITURE ITEM! {} " , mod_event_string_arg );
			}				
		}
		
		SKSE::ModCallbackEvent my_event(
			mod_event_name,                        // event name
			mod_event_string_arg,                  // arbitrary string argument 
			123.0f,                                // arbitrary float argument
			RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
		);
		eventSource->SendEvent(&my_event);
		// This seems to have worked, so we say as much in the log.
		spdlog::info("[SkyrimNetMessagelessImmersion] Mod-event string:  {}", base->GetName());
		spdlog::info("[SkyrimNetMessagelessImmersion] Mod-event sender:  {}", RE::PlayerCharacter::GetSingleton()->GetName());
		spdlog::info("[SkyrimNetMessagelessImmersion] ********************** Sent mod event: {} completed.", mod_event_name);
	} else {
		SKSE::log::error("[SkyrimNetMessagelessImmersion] Failed to get mod event broadcast callback event source!");
	};
}
