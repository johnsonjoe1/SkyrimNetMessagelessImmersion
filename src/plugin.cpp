#include "log.h"


void OnDataLoaded()
{
   
}

std::string general_word_on_milk_pumps = R"SKSE(
A general word on Milk Pumps: Milk pumps in Skyrim are milking stalls, much like for a milk cow, only that this device is designed for human women.
The position is the same as a cow though, on all fours, and milk pumps attach from below and suck the milk from the woman's breasts.
The milk pump is a heavy, intricate contraption of dark wood, leather, and polished brass, built with a strange mix of craftsmanship and clinical purpose. 
Thick straps and padded supports hold the body steady while slender tubes and softly pulsing pumps work with relentless rhythm, drawing milk into waiting glass containers below. 
Valves hiss quietly, gears click in measured cadence, and the whole machine seems almost alive in the way it breathes and churns with mechanical patience. 
Despite its intimidating appearance, every curve and restraint is shaped for long use, worn smooth by countless sessions beneath the pump’s steady, inescapable pull.
)SKSE";

std::string standard_thought_instruction = R"SKSE(
Now your task is this:  Comment on this, with thoughts of YOU as the player. How does the situation make you feel? What are you thinking, being milked like a cow in this contraption?";
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




//  Here comes some code for hooking into the Tanning Rack
class ActivateEventHandler :
    public RE::BSTEventSink<RE::TESActivateEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(
        const RE::TESActivateEvent* event,
        RE::BSTEventSource<RE::TESActivateEvent>*)
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

        SKSE::log::info("[SkyrimNetMessagelessImmersion] =================== New activation Event noticed ===================.");
        SKSE::log::info(
            "[SkyrimNetMessagelessImmersion] Player activated: {}",
            base->GetName()
        );

        // For the moment, we ignore everything, that is not furniture.
        auto formType = base->GetFormType();
        if (formType == RE::FormType::Furniture)
        {
            SKSE::log::info("[SkyrimNetMessagelessImmersion] That activated object seems to be furniture, so we can proceed.");
        } else {
            SKSE::log::info("[SkyrimNetMessagelessImmersion] That activated object does not seem to be furniture, so we ignore it.");
            return RE::BSEventNotifyControl::kContinue;
        }

        // We want to broadcast mod events.  So we need this event source.
        auto eventSource = SKSE::GetModCallbackEventSource();

        // Since the activation event happend, we can start broadcasting the mod event right away.
        if (eventSource)
        {
            // auto furniture_name = base->GetName();
            const char* furniture_name = base->GetName();
            if (!furniture_name) {
                SKSE::log::error("[SkyrimNetMessagelessImmersion] The furniture name was null!  Emergency abort!");
                return RE::BSEventNotifyControl::kContinue;
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
            } else if (std::strcmp(furniture_name , "Blacksmith Forge") == 0) {
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Blacksmith Forge!  THIS GETS A SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
				auto* player = RE::PlayerCharacter::GetSingleton();
				auto furniture = player->GetOccupiedFurniture().get();
				if (furniture) {
					// player is already using furniture, so this is probably the second event.  No comment on this in the blacksmith forge case.
					SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Blacksmith Forge, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
        			return RE::BSEventNotifyControl::kContinue;
				}		else {
					mod_event_string_arg = blacksmith_forge_prompt_1;
					// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for Blacksmith Forge! " + mod_event_string_arg ).c_str());
					SKSE::log::info("THIS IS THE 1st Event for Blacksmith Forge! {} " , mod_event_string_arg );
				}
                mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
            } else if (std::strcmp(furniture_name , "Grindstone") == 0) {
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Grindstone!  THIS GETS A SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
				auto* player = RE::PlayerCharacter::GetSingleton();
				auto furniture = player->GetOccupiedFurniture().get();
				if (furniture) {
					// player is already using furniture, so this is probably the second event.  No comment on this in the blacksmith forge case.
					SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Grindstone, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
        			return RE::BSEventNotifyControl::kContinue;
				}		else {
					mod_event_string_arg = grindstone_prompt_1;
					// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for Grindstone! " + mod_event_string_arg ).c_str());
					SKSE::log::info("THIS IS THE 1st Event for Grindstone! {} " , mod_event_string_arg );
				}
                mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
            } else if (std::strcmp(furniture_name , "Workbench") == 0) {
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Workbench!  THIS GETS A SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
				auto* player = RE::PlayerCharacter::GetSingleton();
				auto furniture = player->GetOccupiedFurniture().get();
				if (furniture) {
					// player is already using furniture, so this is probably the second event.  No comment on this in the blacksmith forge case.
					SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Workbench, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
        			return RE::BSEventNotifyControl::kContinue;
				}		else {
					mod_event_string_arg = workbench_prompt_1;
					// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for Workbench! " + mod_event_string_arg ).c_str());
					SKSE::log::info("THIS IS THE 1st Event for Workbench! {} " , mod_event_string_arg );			
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
        			return RE::BSEventNotifyControl::kContinue;
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

        return RE::BSEventNotifyControl::kContinue;
    };
};

//  Instantiate the code for hooking into the Tanning Rack
static ActivateEventHandler g_activateHandler;


void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
        

		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0004 SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		// That has happend before, right?  SetupLog();
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0002 Log setup just passed.");
		// spdlog::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		SKSE::log::info("[SkyrimNetMessagelessImmersion] Message-at-startup:  SkyrimNetMessagelessImmersion.dll plugin was loaded. GREAT!");
		RE::ConsoleLog::GetSingleton()->Print("[SkyrimNetMessagelessImmersion] 0003 First log message should be written now.");

		// Now we register the event handler for the Tanning Rack, Benches and all the other crap.
		RE::ScriptEventSourceHolder::GetSingleton()
		->AddEventSink<RE::TESActivateEvent>(&g_activateHandler);


		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
        break;
	case SKSE::MessagingInterface::kNewGame:
		break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
	SetupLog();

    auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

    return true;
}