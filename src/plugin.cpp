#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"

#include <unordered_set>

namespace logger = SKSE::log;

// static auto last_speech_timestamp = std::chrono::steady_clock::now();

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
If you have raw hides or old leather items, you can make basic leather components from them. But you must have the right materials
It is not possible to create new armour or complex items here. Also improvements to items are not possible.
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
std::string previous_milk_string = "No milk string HISTORY defined yet!";

float previous_milk_level = 0;
float previous_lactacid_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
float previous_lactacid_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
float previous_milk_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)

float previous_iNeed_fatigue_level = 1000000;  // this will not trigger any getting-more-tired messages at game start



// FIRST WE INSERT THE PAPYRUS INTERACTION, because this will be triggered by papyrus and we later query the values from here, so the definition must come first.

class SNMIPapyrus
{
public:
    static bool Register(RE::BSScript::IVirtualMachine*);
    static void SetMilkLevel(RE::StaticFunctionTag*, float a_value);
	static void SetMilkMax(RE::StaticFunctionTag*, float a_value);
	static void SetLactacidLevel(RE::StaticFunctionTag*, float a_value);
	static void SetLactacidMax(RE::StaticFunctionTag*, float a_value);
	static void SetMilkString(RE::StaticFunctionTag*, std::string a_value);
    static float GetMilkLevel();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
	static float GetMilkMax();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
	
    static std::string GetMilkString();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
    static void SetKeepaliveLevel(RE::StaticFunctionTag*, float a_value);
    static float GetKeepaliveLevel();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
private:
    static inline float _milkLevel = 124356.0f;
	static inline float _milkMax = 123456.0f;
	static inline float _lactacidLevel = 0.0f;
	static inline float _lactacidMax = 0.0f;
	static inline std::string _milkString = "No milk string defined IN PLUGIN yet!";
	static inline float _keepaliveLevel = 0.0f;
};

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
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are now sucked dry and have no more milk!  Say so and let us know how that makes you feel!  And make it clear that you speak about your breasts and the milk inside of your breasts in your response!");
	}
	if ( (previous_milk_level < 0.5f * milk_max) &&  (previous_milk_level > 0.01f ) && (a_value >= 0.5f * milk_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached half of their capacity, so you may soon require to be milked!  Say so and let us know what you are thinking!  And make it clear that you speak about your breasts and the milk inside of your breasts in your response!");
	}
	if ( (previous_milk_level < milk_max) && (a_value >= milk_max) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("Your breasts are filling with milk and have just reached their maximum capacity so your breasts need to be milked in order to relieve the pressure and the weight!  Milk might start to leak from your breasts at any time now.  Say so and let us know how you feel about that!  And make it clear that you speak about your breasts and the milk inside of your breasts in your response!");
	}	
	previous_milk_level = a_value;  // update the previous level for the next check
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
	}
	if ( (previous_lactacid_level > 0 ) &&  (a_value > previous_lactacid_level) ) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("You just received more lactacid into your body on top of the level you had already!  This means that your breasts are going to be producing milk for even longer time!  Say so and let us know how that makes you feel!  And make it clear that you speak about your breasts and the milk in your response and also make sure that you mention the lactacid, that's causing it all!");
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

float SNMIPapyrus::GetMilkLevel()     //  Here we must NOT use static.  That keyword belongs into the class definition only.
{    return _milkLevel;   }
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
    return true;
}



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

class UIDMatchVisitor :
    public RE::MagicTarget::ForEachActiveEffectVisitor
{
public:
    UIDMatchVisitor(std::uint16_t uid) :
        targetUID(uid),
        found(nullptr)
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
                // logger::info("Effect: {}", base->GetName());
				logger::info(
					"Effect ptr={} base={}",
					(void*)effect,
					base ? base->GetName() : "NULL"
				);
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

//  Here comes teh code for hooking into the active effect application and removal, i.e. the list of currently active effects.
class ChangesToTheActiveMagicEffectListEventHandler :
    public RE::BSTEventSink<RE::TESActiveEffectApplyRemoveEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(
        const RE::TESActiveEffectApplyRemoveEvent* a_event,
        RE::BSTEventSource<RE::TESActiveEffectApplyRemoveEvent>*
    ) override
    {
        // Protect from null pointer access, just in case.
		if (!a_event) {
			RE::DebugMessageBox("WOW! Null EVENT POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's see what this event is about.  Who is the actor and what is the effect?		
		auto* targetRef = a_event->target.get();
		if (!targetRef) {
			RE::DebugMessageBox("WOW! Null TARGET POINTER received in the ACTIVE EVENT CHANGE HANDLER!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		RE::Actor* actor;
		actor = targetRef->As<RE::Actor>();
		if (!actor) {
			RE::DebugMessageBox("WOW! There in NOT EVEN AN ACTOR in this ACTIVE EVENT CHANGE!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		// ✔ only care about player
		if (!actor->IsPlayerRef()) {
			return RE::BSEventNotifyControl::kContinue;
		}
		logger::info("========== Found an effect, that is actually about the Player.  Let's go into more details below! =============");		
		SKSE::log::info(
			"Effect {} on {} | UID={}",
			a_event->isApplied ? "APPLIED" : "REMOVED",
			actor->GetName(),
			a_event->activeEffectUniqueID);

		auto* magicTarget = actor->GetMagicTarget();
		auto* ref = a_event->target.get();

		// MyVisitor visitor;  // old Version had really no parameter
		UIDMatchVisitor visitor(a_event->activeEffectUniqueID);  // new version, we want to find the effect with the same UID in the current list of active effects.
		magicTarget->VisitEffects(visitor);
		auto* effect = visitor.GetResult();

		if (!effect)		
		{
			logger::info("No matching ActiveEffect found for UID {}", a_event->activeEffectUniqueID);
			RE::DebugMessageBox("WOW! AFTER RUNNING VisitEffects, we got NO MATCHING ACTIVE EFFECT FOUND for the current UID!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		auto* base = effect->GetBaseObject();
		if (!base)
		{
			RE::DebugMessageBox("WOW! THE BASE OBJECT IS NULL!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
			return RE::BSEventNotifyControl::kContinue;
		}
		auto our_form_id = base->GetFormID();
		auto base_name = base->GetName();
		auto caster = effect->caster.get();
		auto* source = effect->spell;
		// Let's inspect the FormID and hte Form behind the effect, to see if we can identify it.  
		// auto* form = RE::TESForm::LookupByID(0x2803BD99);
		auto* form = RE::TESForm::LookupByID(our_form_id);
		
		//  Cell Tracking Effect
		if ( (std::strcmp(base_name, "Cell Tracking Effect") == 0) || (std::strcmp(base_name, "SCO_CellChangeDetectMgef") == 0) || (std::strcmp(base_name, "SCO_CellChangeBegin") == 0) )  {   // SCO_CellChangeDetectMgef  SCO_CellChangeBegin
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME CELL TRACKING FROM MIA's LAIR or some other mod.");
			return RE::BSEventNotifyControl::kContinue; 
		}
		//  Cell Tracking Effect
		if (std::strcmp(base_name, "Maintenance") == 0) {
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME Hunger-Tracking from RND-Needs-Mod.");
			return RE::BSEventNotifyControl::kContinue;
		}
		//  SOS_Addon_PHF_Recolor
		if (std::strcmp(base_name, "SOS_Addon_PHF_Recolor") == 0) {
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME technicallity from SOS (schlongs-of-skyrim).");
			return RE::BSEventNotifyControl::kContinue;
		}
		//  Automate Hunger Script
		if (std::strcmp(base_name, "Automate Hunger Script") == 0) {
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME technicallity from SOS (schlongs-of-skyrim).");
			return RE::BSEventNotifyControl::kContinue;
		}
		//  Consume Food Portion:  This is actually from CACO
		if (std::strcmp(base_name, "Consume Food Portion") == 0) {
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME technicallity from SOS (schlongs-of-skyrim).");
			return RE::BSEventNotifyControl::kContinue;
		}
		//  RaceMenuHH Scale Effect
		if (std::strcmp(base_name, "RaceMenuHH Scale Effect") == 0) {
			logger::info("----------------->>>>>>>>>>>>>>> SKIPPING THE REST, BECAUES THIS IS SOME technicallity from RaceMenuHH.");
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track YPS-Fashion-High-Heels-slowdown-effect:  this gets activated and deactivated all the time, so we have to keep our messages in check
		if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "High Heel Novice") == 0)  ) && (a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx High Heels Novice MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS HIGH HEELS NOVICE! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You already have some experience with them, but you are still a High Heels Novice, so they still slow you down a bit. It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			return RE::BSEventNotifyControl::kContinue;
		}
		if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "High Heel Novice") == 0)  ) && (!a_event->isApplied) ) {
			// We do nothing here.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of High Heels Novice MAGIC EFFECT.");
			return RE::BSEventNotifyControl::kContinue;
		}
		if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Untrained Feet") == 0)  ) && (a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Untrained Feet' (high heels) MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS UNTRAINED FEET! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You are totally untrained with high heels. You are not even a High Heels Novice yet. So they slow you down massively now.  It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			return RE::BSEventNotifyControl::kContinue;
		}
		if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Untrained Feet") == 0)  ) && (!a_event->isApplied) ) {
			// We do nothing here.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Untrained Feet MAGIC EFFECT.");
			return RE::BSEventNotifyControl::kContinue;
		}


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
			RE::DebugMessageBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
			SKSE::log::info("XX-- our event handler for FOOD-BASED-DISEASE!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just ate something!  As a total surprise, you now notice, that you may have just contracted the so-called disease '{}' from it!  You need to announce the potential infection in your response, so that the actual player is informed.  You may do that implicitly, in the form of regret, surprise anger or shock.  It is a potentially dangerous condition. Be sure to mention the name of the disease '{}' in your response.  ", base_name, base_name)); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track Unforgiving Devices Struggle Exhaustion here:  FIRST THE APPLICATION OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "Exhaustion") == 0)  ) && ( (std::strcmp(source->GetName(), "Struggle exhaustion") == 0)  ) && (a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for UNFORGIVING-DEVICES STRUGGLE EXHAUSTION! ", base_name);
			//RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("XX-- our event handler for UD STRUGGLE EXHAUSTION APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just tried getting out of your locking bondage devices for a whole while. You may have made some progress, but nevertheless now you are too exhausted to continue.  Say as much in your response.")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track Unforgiving Devices Struggle Exhaustion here:  NOW THE REMOVAL OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "Exhaustion") == 0)  ) && ( (std::strcmp(source->GetName(), "Struggle exhaustion") == 0)  ) && (!a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for UNFORGIVING-DEVICES STRUGGLE EXHAUSTION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("XX-- our event handler for UD STRUGGLE EXHAUSTION REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just were trying to get out of your locking bondage devices for a whole while. You may have made some progress, but in any case, that activity had made you exhausted to the point where you couldn't continue any more.  But now time has passed and you're feeling better and you're good to go and maybe could continue trying.  Say as much in your response.")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track UD/US Black-Goo-Application-Effect here:  FIRST THE APPLICATION OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "Device Manifest") == 0)  ) && (a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for BLACK-GOO-APPLICATION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("XX-- our event handler for BLACK-GOO-APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("Some substance called black goo just came in contact with you, and, to your horror, it manifested into a bondage device, thus trapping you as the victim now locked into said device.  What are you thinking in the face of this situation? ")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track UD/DD slowdown-effect from bondage boots:  :  NOW THE REMOVAL OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "SpeedMult Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "BootSlow-Enchant") == 0)  ) && (a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for UD BONDAGE-BOOTS SLOWDOWN APPLICATION! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("XX-- our event handler for UD BONDAGE BOOTS SLOWDOWN APPLICATION!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, just got locking bondage boots equipped onto your feet and you cannot take them off any more. But the important point is:  You cannot walk or run so fast any more with these heels equipped onto your feet! You will be slowed down for the whole time while wearing them (thus less able to run away from dangerious things)! Say as much in your response.")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}
		// Let's try to track UD/DD slowdown-effect from bondage boots:  NOW THE REMOVAL OF THE EFFECT.
		if (base && ( (std::strcmp(base->GetName(), "SpeedMult Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "BootSlow-Enchant") == 0)  ) && (!a_event->isApplied) )
		{
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for UD BONDAGE-BOOTS SLOWDOWN REMOVAL! ", base_name);
			// RE::DebugMessageBox(stomach_rot_status.c_str());	
			SKSE::log::info("XX-- our event handler for UD BONDAGE BOOTS SLOWDOWAN REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, had your feet locked into bondage boots the whole time and couldn't get them off. This has slowed you down the whole time. But now you got rid of the locking bondage devices on your feet. But the important point is:  This means you can finally move much faster again!  (And you won't trip over your feet any more.)  Say as much in your response.")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}



		// THIS SECTION OF THE CODE SHOULD BE CALLED VERY FREQUENTLY IN THE COURSE OF MAGIC EFFECTS.
		// THIS MEANS WE CAN PUT SOME EXPERIMENTAL EFFECTS HERE.
		//
		// We have from another mod:
		//
		// GlobalVariable function GetINeedFatigue() global
    	// return Game.GetFormFromFile(0x12DC, "iNeed.esp") as GlobalVariable
		// endFunction
		//
		// This should allow for direct native access to the same from C++:
		//
		// 
		auto* fatigueGV =
			RE::TESDataHandler::GetSingleton()
				->LookupForm<RE::TESGlobal>(0x12DC, "iNeed.esp");

		if (fatigueGV) {
			float fatigue = fatigueGV->value;
			logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: value={}", fatigue);
			if (fatigue == 0) {
				logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means NO FATIGUE AT ALL, NOTHING! ");
				if (fatigue < previous_iNeed_fatigue_level) {
					DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are full rested from sleep and you are completely rid of your fatigue now!  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				}							
			} else if (fatigue == 1) {
				logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means MILD FATIGUE! ");
				if (fatigue > previous_iNeed_fatigue_level) {
					DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling mild fatigue.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				}
			} else if (fatigue == 2) {
				logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means MODERATE FATIGUE! ");
				if (fatigue > previous_iNeed_fatigue_level) {
					DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling moderate fatigue.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				}				
			} else if (fatigue == 3) {
				logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means SEVERE FATIGUE! ");
				if (fatigue > previous_iNeed_fatigue_level) {
					DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::format("You are feeling severe fatigue!  This is not just a little bit, but really severe fatigue that is impairing your abilities.  Say so in your response and let us know how that makes you feel!  And make it clear that you speak about your fatigue in your response!"));
				}				
			} else {
				logger::info(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>iNeed Fatigue GlobalVariable found: i guess that means some unknown level of fatigue! ");
			}
			// We only update fatigue, if the whole iNeed stuff worked.
			previous_iNeed_fatigue_level = fatigue;					
		}

		
				/*
Effect APPLIED on Beea | UID=10
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:150] Effect ptr=0x12896d8a8e0 base=Movement Speed Penalty
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:274] Base name: Movement Speed Penalty
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:275] Base ptr: 0x12163e2fe40
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:276] Base-FormID: 66030BDA
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:278] Base-Form Type: 18   (This means: MGEF) 
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:279] base-Effect EDID: yps_effPenaltySpeedMult
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:280] Source pointer: 0x12163e59c00
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:282] Caster: Beea
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:284] Source ptr: 0x12163e59c00
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:287] Magnitude: -10
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:288] Duration: 0
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:292] Source name: High Heel Novice
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:293] Source FormID: 66031C03
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:294] Source EDID: ypsSpellSpeedDebuff10
[2026-05-16 17:06:54.777] [log] [info] [plugin.cpp:307] Form with ID 66030BDA found: Movement Speed Penalty





[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:206] Effect APPLIED on Beea | UID=19
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:148] Effect ptr=0x13c3b010e00 base=SpeedMult Penalty
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:230] Effect ptr: 0x13c3b010e00
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:231] UID: 19
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:253] Base name: SpeedMult Penalty
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:254] Base ptr: 0x136e8560100
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:255] Base-FormID: 1401332D
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:257] Base-Form Type: 18   (This means: MGEF) 
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:258] base-Effect EDID: zadx_effPenaltySpeedMult
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:259] Source pointer: 0x136e826f4c0
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:261] Caster: Beea
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:263] Source ptr: 0x136e826f4c0
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:266] Magnitude: -20
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:267] Duration: 0
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:271] Source name: BootSlow-Enchant
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:272] Source FormID: 1401332E
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:273] Source EDID: zadx_EnchSlowBoots
[2026-05-15 12:07:15.265] [log] [info] [plugin.cpp:286] Form with ID 1401332D found: SpeedMult Penalty



[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:205] ========== Found an effect, that is actually about the Player.  Let's go into more details below! =============
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:206] Effect APPLIED on Beea | UID=8
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:148] Effect ptr=0x1fcb7fae2a0 base=SpeedMult Penalty
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:230] Effect ptr: 0x1fcb7fae2a0
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:231] UID: 8
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:253] Base name: SpeedMult Penalty
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:254] Base ptr: 0x1f769311d00
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:255] Base-FormID: 1401332D
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:257] Base-Form Type: 18   (This means: MGEF) 
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:258] base-Effect EDID: zadx_effPenaltySpeedMult
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:259] Source pointer: 0x1f768e3f4c0
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:261] Caster: Beea
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:263] Source ptr: 0x1f768e3f4c0
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:266] Magnitude: -20
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:267] Duration: 0
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:271] Source name: BootSlow-Enchant
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:272] Source FormID: 1401332E
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:273] Source EDID: zadx_EnchSlowBoots
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:286] Form with ID 1401332D found: SpeedMult Penalty
[2026-05-15 22:49:42.524] [log] [info] [plugin.cpp:300] xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx NOT THE DRUNK STUMBLE SCRIPT!
[2026-05-15 22:49:42.607] [log] [info] [plugin.cpp:495] MOD EVENT:  Name: DeviceEquippedBallet Boots  StrArg: Beea  NumArg: 1

*/
		SKSE::log::info(".");
		SKSE::log::info(".");
		SKSE::log::info("ABOVE IS A POTENTIALLY UNHANDLED MAGIC EFFECT??? CHECK THE BASE NAME AND SOURCE NAME TO SEE IF IT'S SOMETHING YOU WANT TO REACT TO, OR IF IT'S SOME RANDOM EFFECT THAT YOU DON'T CARE ABOUT.  IF IT'S THE LATTER, THEN YOU PROBABLY WANT TO ADD A NEW IF-STATEMENT FOR THIS EFFECT IN THIS HANDLER, SO THAT IT DOESN'T GET LOGGED IN SUCH DETAIL ANY MORE, BECAUSE THAT WOULD BE ANNOYING.  CHECK THE BASE NAME AND SOURCE NAME TO SEE WHAT EFFECT THIS IS ABOUT.  IF IT'S AN EFFECT YOU CARE ABOUT, THEN CONSIDER ADDING A CUSTOM MESSAGE FOR IT IN THIS HANDLER, SO THAT YOUR TTS CAN REACT TO IT IN A MEANINGFUL WAY! ");

        return RE::BSEventNotifyControl::kContinue;
    }
	private:

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


class ModEventHandler :
    public RE::BSTEventSink<SKSE::ModCallbackEvent>
{
public:

    RE::BSEventNotifyControl ProcessEvent(
        const SKSE::ModCallbackEvent* a_event,
        RE::BSTEventSource<SKSE::ModCallbackEvent>*)
        override
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }
        // logger::info("MOD EVENT:");
        // logger::info("  Name: {}", a_event->eventName);
        // logger::info("  StrArg: {}", a_event->strArg);
        // logger::info("  NumArg: {}", a_event->numArg);
		

		if ( is_known_useless_event(a_event->eventName.c_str()))
		{
			// We ignore those mod event broadcasts, because we cannot and do not need to make them into reasonable immersive player thoughts or talk in any way. 
			logger::info("=== Mod Event Ignored:  Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
			return RE::BSEventNotifyControl::kContinue;
		}

		// Now, PERIODICALLY, we take care of changes in the values of other mods.  We put this here, because this gets
		// triggered reasonably often.  
		float current_milk = SNMIPapyrus::GetMilkLevel();
		float current_keepalive = SNMIPapyrus::GetKeepaliveLevel();
		std::string current_milk_string = SNMIPapyrus::GetMilkString();
		SKSE::log::info("Current Keepalive: {}", current_keepalive);  
		SKSE::log::info("Current milk level is: {}", current_milk);
		SKSE::log::info("Current milk string is: {}", current_milk_string);
		SKSE::log::info("PREVIOUS milk string is: {}", previous_milk_string);
		// We check if there is a change in the milk string
		if ( (std::strcmp(current_milk_string.c_str() , previous_milk_string.c_str()) == 0)  ) {
			// There was no change in milk string
		} else {
			// There was a change in milk string
			DumpThoughts::throw_out_TTS_thought_message("Due to milk slowly accumulating in her breasts, " + current_milk_string);
			previous_milk_string = current_milk_string;  // Update the previous milk string to the current one for the next comparison.			
		}

		

		// We log all other mod events, because they might be interesting for us to react to and turn into immersive player thoughts
		logger::info("MOD EVENT:  Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg);
		std::string debug_message = std::format("MOD EVENT:  Name: {}  StrArg: {}  NumArg: {}" , a_event->eventName.c_str() , a_event->strArg.c_str() , a_event->numArg );


		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceRemovedBoots
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviceRemovedBoots") == 0)  ) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("YOU, the player, just managed to get our of your locking bondage boots.  What a relief!  What are you thinking now based on this? ");
			return RE::BSEventNotifyControl::kContinue;
		}
		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviceRemovedWristRestraint") == 0)  ) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message("YOU, the player,  managed to get our of your locking bondage device.  Your wrists are free again!  What a relief!  What are you thinking now based on this? ");
			return RE::BSEventNotifyControl::kContinue;
		}
		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
		if ( (std::strcmp(a_event->eventName.c_str() , "UD_SentientDialogue") == 0)  ) {
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("YOU, the player, suddenly have a feeling like your {} is speaking to you, even though it is just an item and not a living creature.  Is it maybe time to question your sanity?  What are you thinking now based on this? ", a_event->strArg.c_str());
			DumpThoughts::throw_out_TTS_thought_message(thought_message);
			return RE::BSEventNotifyControl::kContinue;
		}
		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviceEquippedBoots") == 0)  || (std::strcmp(a_event->eventName.c_str() , "DeviceEquippedBallet Boots") == 0)) {  
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("YOU, the player, just got locked into bondage boots and you cannot take them off any more because they got locked onto your feet.  What are you thinking now based on this? ", a_event->strArg.c_str());
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
			return RE::BSEventNotifyControl::kContinue;
		}
		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedyoke
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviceEquippedyoke") == 0)  ) {
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("YOU, the player, just got locked into an iron bondage yoke. Such a yoke is an iron bondage device, that locks around your neck and wrists, trapping your wrists in a position next to your shoulders, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.  What are you thinking now based on this? ", a_event->strArg.c_str());
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
			return RE::BSEventNotifyControl::kContinue;
		}
		// These are mod events, that we actually could and should use to react to them via thoughts:  DeviceEquippedStraitJacket
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviceEquippedStraitJacket") == 0)  ) {
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("YOU, the player, just got locked into a strait jacket.  The jacket holds your arms and hands in tight sleeves bound around your torso, so that you are helpless and at the mercy of others. This device got locked onto you and now you cannot get of out it.  What are you thinking now based on this? ", a_event->strArg.c_str());
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
			return RE::BSEventNotifyControl::kContinue;
		}

		 


		// MOD EVENT:  Name: DeviousEventTrip and Fall  StrArg: Beea  NumArg: 0
		if ( (std::strcmp(a_event->eventName.c_str() , "DeviousEventTrip and Fall") == 0)  ) {
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("YOU, the player, just tripped over your own feet, because you are wearing these devious bondage boots that you are locked into and cannot take off.  What are you thinking now based on this? ", a_event->strArg.c_str());
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
			return RE::BSEventNotifyControl::kContinue;
		}
		// MOD EVENT:  YPS Thoughts:  Those will be pushed to the background channel.
		if ( (std::strcmp(a_event->eventName.c_str() , "YPS_ThoughtEvent") == 0)  ) {
			std::string  thought_message = std::format("=============== ************************************ LETS STOP HERE.  STOP NOW.  LETS STOP NOW AND CUT THIS SHORT:  THERE IS ONLY ONE THING REQUIRED RIGHT NOW:  GIVE BACK THE THOUGHT, THAT \"{{ player.name }}  WAS THINKING.  AND WHAT WAS SHE THINKING?  THE INFORMATION IS RIGHT HERE, BECAUSE WE KNOW THE FOLLWING SENTENCE:  {}  NOW THAT YOU HAVE THE INFORMATION, FORGET ALL THE REST AND JUST GIVE BACK THE RIGHT ANSWER!!! You can paraphrase and adapt it a little bit, but you don't have to.  THE REST BELOW CAN ALSO BE IGNORED, but any afterthought or narrator comment should then be in third person afterwards! ************************************ ================== ", a_event->strArg.c_str());
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(thought_message);   // this shouldn't be overdone, but hte background code makes sure of that.
			return RE::BSEventNotifyControl::kContinue;
		}
		// MOD EVENT:  Generic orgasm start (and end)  [2026-05-21 21:44:52.579] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmStart  StrArg:   NumArg: 0  // [2026-05-21 21:45:00.613] [log] [info] [plugin.cpp:634] MOD EVENT:  Name: PlayerOrgasmEnd  StrArg:   NumArg: 0
		if ( (std::strcmp(a_event->eventName.c_str() , "PlayerOrgasmStart") == 0)  ) {
			// Name: UD_SentientDialogue  StrArg: Hand restraint  NumArg: 1
			std::string  thought_message = std::format("Regardless whether you like it or not, from all the stimulation, you, the player, are now suddenly having an orgasm! Let us know this via your response. ");
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
			return RE::BSEventNotifyControl::kContinue;
		}
		

		// MOD EVENT:  IF there was other SkyrimNetSpeech or thoughts, we restart our pause tracking, to not overflow the BACKGROUND TTS channel with too much content for the listener.  There should also be a little bit of pause and quiet here and there.
		if ( (std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechComplete") == 0)  || 
			(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechCompleted") == 0)  || 
			(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechStarted") == 0)  ||
			(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioStarted") == 0) ||
			(std::strcmp(a_event->eventName.c_str() , "SkyrimNet_AudioEnded") == 0)  ) {			

			auto now = std::chrono::steady_clock::now();
			// auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - last_speech_timestamp);
			auto runtime = std::chrono::duration_cast<std::chrono::seconds>(now - DumpThoughts::GetLastSpeechTimestamp());

			SKSE::log::info("=====NO-SPEECH-TIMER for the BACKGROUND CHANNEL WAS RESET BY SKYRIMNET ModEvent after {} seconds. ", runtime.count());
			// last_speech_timestamp=std::chrono::steady_clock::now();
			DumpThoughts::reset_last_speech_timestamp();
			return RE::BSEventNotifyControl::kContinue;
		}
		// 	|| (std::strcmp(a_event->eventName.c_str() , "SkyrimNet_SpeechComplete") == 0)
		
		// MORE THINGS TO HANDLE: Name: yps_AddictionBuffChange  StrArg:   NumArg: 6

		// MORE THINGS TO HANDLE: Name: yps_FashionChange  StrArg: FingerNailPolish  NumArg: 0

		// MORE THINGS TO HANDLE: (This is from the ass-slap or tit-slap in Spank-that-Ass / Devious Followers Mod) Name: DF-ResistanceLoss  StrArg:   NumArg: 1


		// IF the MOD-EVENT really WASNT HANDLED BY THIS POINT, IT IS MAYBE SOMETHING NEW, AND THEREFORE WE MAKE A MESSAGEBOX-ANNOUNCEMENT OF it.
		if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
		{
			RE::DebugMessageBox(("SNMI:  An unhandled mod-event was discovered: " + debug_message).c_str());
		} else {
			SKSE::log::info("SNMI:  An unhandled mod-event was discovered: {}", debug_message);
		}  

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


        return RE::BSEventNotifyControl::kContinue;
    }

private:

std::unordered_set<std::string> ignored_mod_events = {
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
	"SSL_CLEAR_Thread0",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
	"AnimationEnding",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
	"AnimationEnding_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
	"AnimationEnd",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
	"AnimationEnd_MatchMaker",   // This is technical Sexlab-(PPlus?)-related event, thing to do for us now and here.
};

	bool is_known_useless_event(std::string event_name)
	{
		if (ignored_mod_events.contains(event_name)) {
			return true;
			// logger::info("Found!");
		}
		return false;
	}

};




//  Here comes the code for hooking into the furniture usage events, or even all usage events, but for now we focus on furniture.
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
            } else if (GetKeywordIndex(furniture_name) != -1) {
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a piece of furniture that is in our list of special furniture!  This gets a SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
			
				auto* player = RE::PlayerCharacter::GetSingleton();
				auto furniture = player->GetOccupiedFurniture().get();
				if (furniture) {
					// player is already using furniture, so this is probably the second event.  No comment on this for all the furnitures in this standard list.
					SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a furniture from the list of special furniture, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
        			return RE::BSEventNotifyControl::kContinue;
				} else {
					mod_event_string_arg = furniture_prompt_list[GetKeywordIndex(furniture_name)];
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
	private:
	int GetKeywordIndex(std::string_view keyword)
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
		RE::ScriptEventSourceHolder::GetSingleton()
		->AddEventSink<RE::TESActivateEvent>(&g_activateHandler);

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
        break;
	case SKSE::MessagingInterface::kNewGame:
		DumpThoughts::reset_last_game_load_or_reload_timestamp();
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