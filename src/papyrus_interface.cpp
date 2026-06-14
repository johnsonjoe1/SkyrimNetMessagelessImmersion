#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "misc.h"
#include <unordered_set>
#include "papyrus_interface.h"

namespace logger = SKSE::log;

std::string previous_milk_string = "No milk string HISTORY defined yet!";

/*
float SNMIPapyrus::previous_milk_level = 0;
float SNMIPapyrus::previous_lactacid_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
float SNMIPapyrus::previous_lactacid_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
float SNMIPapyrus::previous_milk_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
float SNMIPapyrus::previous_maid_level = -1.0f; 

*/
// FIRST WE INSERT THE PAPYRUS INTERACTION, because this will be triggered by papyrus and we later query the values from here, so the definition must come first.

void SNMIPapyrus::set_lovesickness_flag(RE::StaticFunctionTag*, float a_value)
{
    _lovesickness_flag = a_value;
	SKSE::log::info("Note:  Lovesickness_flag:  {}  previous_lovesickness_flag: {}", _lovesickness_flag, previous_lovesickness_flag);	
	if ( (_lovesickness_flag > previous_lovesickness_flag ) ) {
		SKSE::log::info("Note: >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LOVESICKNESS-STATEMENT TRIGGERED !!!!!!!!!!!!!!!!!!!!");	
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("The sex was so great, you are filled with euphoria.  But something else is the matter:  You think you might be falling in love right now.  Say so and let us know how you feel about that!  And make it clear that you speak about your imminent lovesickness in your response!");
		SKSE::log::info("Note:  Milk-level-update thought 3 was delivered.");
	}  else {
		SKSE::log::info("Note: No increase in values, so no LOVESICKNESS-STATEMENT triggerd.");	
	}
	previous_lovesickness_flag = a_value;  // update the previous level for the next check	
}
void SNMIPapyrus::set_lovesickness_euphoria(RE::StaticFunctionTag*, float a_value)
{
    _lovesickness_euphoria = a_value;
}


void SNMIPapyrus::SetMilkLevel(RE::StaticFunctionTag*, float a_value)
{
    _milkLevel = a_value;
	// float milk_max = 4.0;
	float milk_max = SNMIPapyrus::GetMilkMax();
	if (milk_max == 0) {
		milk_max = 4.0f;  // avoid division by zero, and put in default starting MME user level, only for this function locally
	}

	// maybe the mod isn't even installed.  in that case the level and previous level would be 0 and nothing needs to be done
	if (_milkLevel == 0 && previous_milk_level == 0) {
		return;
	}

    SKSE::log::info("Note:  Milk level updated VIA PUSH FROM PAPYRUS: {}", a_value);
	// So let's do some additional checks here:  If the level just went above 50% of max, this is worthy of a special thought.
	if ( (previous_milk_level > 0.15 ) && (a_value <= 0.15) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are now sucked dry and have no more milk!  Say so and let us know how that makes you feel!  And make it clear that you speak about the milk inside of your breasts in your response!");
		SKSE::log::info("Note:  Milk-level-update thought 1 was delivered.");
	}
	if ( (previous_milk_level < 0.5f * milk_max) &&  (previous_milk_level > 0.01f ) && (a_value >= 0.5f * milk_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached half of their capacity, so you may soon require to be milked!  Say so and let us know what you are thinking!  And make it clear that you speak about the milk inside of your breasts in your response!");
		SKSE::log::info("Note:  Milk-level-update thought 2 was delivered.");
	}
	if ( (previous_milk_level < milk_max) && (a_value >= milk_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached their maximum capacity.  So your breasts need to be milked in order to relieve the pressure in your breasts.  Your breasts need to be milked.  You need to be milked.  Otherwise the milk might start to leak from your breasts at any time now.  Let us know how you feel about that!  And make it clear that you speak about the milk inside of your breasts in your response!");
		SKSE::log::info("Note:  Milk-level-update thought 3 was delivered.");
	}	
	previous_milk_level = a_value;  // update the previous level for the next check
}

void SNMIPapyrus::SetMaidLevel(RE::StaticFunctionTag*, float a_value)
{
    _maidLevel = a_value;

	// maybe the mod isn't even installed.  in that case the level and previous level would be 0 and nothing needs to be done
	if (_maidLevel == 0 && previous_maid_level == 0) {
		return;
	}
	if (previous_maid_level == -1.0f) {  // This is the initial value, so we just set it without any checks, to avoid any weird messages at game start.
		previous_maid_level = _maidLevel;
		return;
	}	

    SKSE::log::info("Note:  Maid level updated VIA PUSH FROM PAPYRUS: {}", a_value);
	// So let's do some additional checks here:  If the level just went above 50% of max, this is worthy of a special thought.
	if ( (previous_maid_level < _maidLevel) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts have suddenly gained capacity and advanced to a new milk level!  Say so and let us know how that makes you feel!  And make it clear that you speak about the milk capacity of your breasts and that you might resembe a 'better milk maid' now in your response!");
		SKSE::log::info("Note:  Maid-level-update thought was delivered.");
	}
	previous_maid_level = _maidLevel;  // update the previous level for the next check
}

void SNMIPapyrus::set_yps_AddictionLevel(RE::StaticFunctionTag*, float a_value)
{
    _yps_AddictionLevel = a_value;

	// maybe the mod isn't even installed.  in that case the level and previous level would be 0 and nothing needs to be done
	if (_yps_AddictionLevel == 0 && previous_yps_AddictionLevel == 0) {
		return;
	}
	if (previous_yps_AddictionLevel == -1.0f) {  // This is the initial value, so we just set it without any checks, to avoid any weird messages at game start.
		previous_yps_AddictionLevel = _yps_AddictionLevel;
		return;
	}	

    SKSE::log::info("Note:  yps_AddictionLevel updated VIA PUSH FROM PAPYRUS: {}", a_value);
	// So let's do some additional checks here:  If the level just went above 50% of max, this is worthy of a special thought.
	if ( (previous_yps_AddictionLevel < _yps_AddictionLevel) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just gained a level in overall fashion addiction according to the YPS fashion vendor!  Say so and let us know how that makes you feel!");
		SKSE::log::info("Note:  Fashion-Addiction-level-GAIN thought was delivered.");
	}
	if ( (previous_yps_AddictionLevel > _yps_AddictionLevel) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just lost a level in overall fashion addiction according to the YPS fashion vendor!  Say so and let us know how that makes you feel!");
		SKSE::log::info("Note:  Fashion-Addiction-level-LOSS thought was delivered.");
	}	
	previous_yps_AddictionLevel = _yps_AddictionLevel;  // update the previous level for the next check
}
void SNMIPapyrus::set_yps_AddictionBuff(RE::StaticFunctionTag*, float a_value)
{
    _yps_AddictionBuff = a_value;

	// maybe the mod isn't even installed.  in that case the level and previous level would be 0 and nothing needs to be done
	if (_yps_AddictionBuff == 0 && previous_yps_AddictionBuff == 0) {
		return;
	}
	if (previous_yps_AddictionBuff == -1.0f) {  // This is the initial value, so we just set it without any checks, to avoid any weird messages at game start.
		previous_yps_AddictionBuff = _yps_AddictionBuff;
		return;
	}	

    SKSE::log::info("Note:  yps_AddicitonBuff updated VIA PUSH FROM PAPYRUS: {}", a_value);
	// So let's do some additional checks here:  If the level just went above 50% of max, this is worthy of a special thought.
	if ( (previous_yps_AddictionBuff < _yps_AddictionBuff) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just gained a stronger boost from your YPS fashion addiction!  You appear even more radiant and people will respond more favorable to you this way!  Say so and let us know how that makes you feel!");
		SKSE::log::info("Note:  yps_AddictionBuff-update thought was delivered.");
	}
	if ( (previous_yps_AddictionBuff > _yps_AddictionBuff) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just LOST some of your YPS fashion addiction buff!  You appear LESS radiant and people will respond LESS favorable to you this way!  Say so and let us know how that makes you feel!");
		SKSE::log::info("Note:  yps_AddictionBuff-update thought was delivered.");
	}
	previous_yps_AddictionBuff = _yps_AddictionBuff;  // update the previous level for the next check
}


void SNMIPapyrus::SetMilkMax(RE::StaticFunctionTag*, float a_value)
{
    _milkMax = a_value;
    SKSE::log::info("Note:  Milk MAXIMUM-LEVEL updated VIA PUSH FROM PAPYRUS: {}", a_value);
	previous_milk_max_level = a_value;  // update the previous level for the next check
}

void SNMIPapyrus::SetLactacidLevel(RE::StaticFunctionTag*, float a_value)
{
    _lactacidLevel = a_value;
	// float lactacid_max = 4.0;
    SKSE::log::info("Note:  Lactacid level updated VIA PUSH FROM PAPYRUS: {}", a_value);
	// So let's do some additional checks here:  If the level just went above 50% of max, this is worthy of a special thought.
	if ( (previous_lactacid_level <= 0 ) && (a_value > 0) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just received lactacid into your body, which you didn't have before!  This means that your breasts are going to start producing milk now!  Say so and let us know how that makes you feel!  And make it clear that you speak about your breasts and the milk in your response and also make sure that you mention the lactacid, that's causing it all!");
		DumpThoughts::reset_lactacid_added_speech_timestamp();  // reset the timestamp for the last speech about lactacid being added, so that we can later check if the player is talking about it in a timely manner.
		SKSE::log::info("Note:  Milk-level-update thought 1 was delivered.");
	}
	if ( (previous_lactacid_level > 0 ) &&  (a_value > previous_lactacid_level) ) {
		if (!DumpThoughts::too_early_for_next_lactacid_speech()) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just received more lactacid into your body on top of the level you had already!  This means that your breasts are going to be producing milk for even longer time!  Say so and let us know how that makes you feel!  And make it clear that you speak about your breasts and the milk in your response and also make sure that you mention the lactacid, that's causing it all!");
			DumpThoughts::reset_lactacid_added_speech_timestamp();  // reset the timestamp for the last speech about lactacid being added, so that we can later check if the player is talking about it in a timely manner.
			SKSE::log::info("Note:  Lactacid-level-update thought 2 was delivered.");
		}
	}
	previous_lactacid_level = a_value;  // update the previous level for the next check
}

void SNMIPapyrus::SetLactacidMax(RE::StaticFunctionTag*, float a_value)
{
    _lactacidMax = a_value;
	// float lactacid_max = 4.0;
    SKSE::log::info("Note:  Lactacid MAXIMUM-LEVEL updated VIA PUSH FROM PAPYRUS: {}", a_value);
/*	if ( (previous_lactacid_max_level > 0.15 ) && (a_value <= 0.15) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are now sucked dry and have no more milk!  Say so and let us know how that makes you feel!  And make it clear that you speak about your breasts and the milk in your response!");
	}
	if ( (previous_lactacid_max_level < 0.5f * lactacid_max) &&  (previous_lactacid_max_level > 0.01f ) && (a_value >= 0.5f * lactacid_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached half of their capacity!  Say so and let us know what you are thinking!  And make it clear that you speak about your breasts and the milk in your response!");
	}
	if ( (previous_lactacid_max_level < lactacid_max) && (a_value >= lactacid_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached their maximum capacity!  Milk might start to leak from your breasts at any time now.  Say so and let us know how you feel about that!  And make it clear that you speak about your breasts and the milk in your response!");
	}	
*/	
	previous_lactacid_max_level = a_value;  // update the previous level for the next check
}



void SNMIPapyrus::SetMilkString(RE::StaticFunctionTag*, std::string s_value)
{
    _milkString = s_value;
    SKSE::log::info("Note:  Milk string updated VIA PUSH FROM PAPYRUS: {}", s_value);
}

void SNMIPapyrus::SetYpsConditionString(RE::StaticFunctionTag*, std::string s_value)
{
    _yps_condition_string = s_value;
    SKSE::log::info("Note:  Yps condition string updated VIA PUSH FROM PAPYRUS: {}", s_value);
}



float SNMIPapyrus::GetMilkLevel()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _milkLevel;   }

float SNMIPapyrus::GetMaidLevel()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _maidLevel;   }



float SNMIPapyrus::GetMilkMax()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _milkMax;   }
std::string SNMIPapyrus::GetMilkString()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _milkString;   }
void SNMIPapyrus::SetKeepaliveLevel(RE::StaticFunctionTag*, float a_value)
{
    _keepaliveLevel = a_value;
    SKSE::log::info("Note:  Keepalive level updated VIA PUSH FROM PAPYRUS: {}", a_value);
}
float SNMIPapyrus::GetKeepaliveLevel()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _keepaliveLevel;   }
bool SNMIPapyrus::Register(RE::BSScript::IVirtualMachine* a_vm)
{
    a_vm->RegisterFunction("SetMilkLevel", "SNMI_Native", SetMilkLevel);
    a_vm->RegisterFunction("SetLactacidLevel", "SNMI_Native", SetLactacidLevel);	
	a_vm->RegisterFunction("SetLactacidMax", "SNMI_Native", SetLactacidMax);		
	a_vm->RegisterFunction("SetMilkString", "SNMI_Native", SetMilkString);
    a_vm->RegisterFunction("SetKeepaliveLevel", "SNMI_Native", SetKeepaliveLevel);
	a_vm->RegisterFunction("SetMaidLevel", "SNMI_Native", SetMaidLevel);

	a_vm->RegisterFunction("set_yps_AddictionLevel", "SNMI_Native", set_yps_AddictionLevel);
	a_vm->RegisterFunction("set_yps_AddictionBuff", "SNMI_Native", set_yps_AddictionBuff);

	a_vm->RegisterFunction("set_lovesickness_flag", "SNMI_Native", set_lovesickness_flag);
	a_vm->RegisterFunction("set_lovesickness_euphoria", "SNMI_Native", set_lovesickness_euphoria);


	// function set_yps_AddictionLevel(float a_value) Global Native
	// function set_yps_AddictionBuff(float a_value) Global Native

	a_vm->RegisterFunction("SetYpsConditionString", "SNMI_Native", SetYpsConditionString);
    return true;
}

std::string SNMIPapyrus::GetYpsConditionString()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _yps_condition_string;   }


void SNMIPapyrus::handle_mme_milk_value_changes_and_produce_thoughts_from_them()
{
    SKSE::log::info("Note:  handle_mme_milk_value_changes_and_produce_thoughts_from_them WAS CALLED!!!");

	// Now, PERIODICALLY, we take care of changes in the values of other mods.  We put this here, because this gets
	// triggered reasonably often.  
	float current_milk = SNMIPapyrus::GetMilkLevel();
	float current_keepalive = SNMIPapyrus::GetKeepaliveLevel();
	float current_maid_level = SNMIPapyrus::GetMaidLevel();
	
	std::string current_milk_string = SNMIPapyrus::GetMilkString();
	SKSE::log::info("Current Keepalive: {}", current_keepalive);  
	SKSE::log::info("Current milk level is: {}", current_milk);
	SKSE::log::info("Current maid level is: {}", current_maid_level);
	SKSE::log::info("Current milk string is: {}", current_milk_string);
	SKSE::log::info("PREVIOUS milk string is: {}", previous_milk_string);


	// We check if there is a change in the milk string

	if ( (std::strcmp(current_milk_string.c_str() , previous_milk_string.c_str()) == 0)   |  (std::strcmp(previous_milk_string.c_str() , "No milk string HISTORY defined yet!") == 0)  |  (std::strcmp(previous_milk_string.c_str() , "No milk string defined IN PLUGIN yet!") == 0)) {
		// There was no change in milk string OR it was still the startup value, so nothing much to do here, except kill the startup value.
		previous_milk_string = current_milk_string;  // Update the previous milk string to the current one for the next comparison.
	} else {
		// There was a change in milk string
		DumpThoughts::throw_out_TTS_thought_message("Due to milk slowly accumulating in her breasts, " + current_milk_string);
		previous_milk_string = current_milk_string;  // Update the previous milk string to the current one for the next comparison.			
	}

	SKSE::log::info("Note:  handle_mme_milk_value_changes_and_produce_thoughts_from_them FINISHED!!!");

}
