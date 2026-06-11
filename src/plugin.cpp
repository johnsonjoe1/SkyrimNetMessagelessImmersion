#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_AND_modesty.h"
#include "handle_iNeed.h"
#include "handle_yps.h"
#include "misc.h"
#include "papyrus_interface.h"
#include <unordered_set>
#include <optional>


namespace logger = SKSE::log;


float previous_dirt_value = 100000;  // some impossible value, so that no message occurs (unless dirt value 0, which wouldn't likely be the case in mid-game)


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
			LillithOnlyBox("WOW! There in NOT EVEN AN ACTOR in this ACTIVE EVENT CHANGE!  ABORTING HANDLER (WHICH IS NO BIG PROBLEM FOR YOUR GAME, BY THE WAY)!!!");
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
			// return RE::BSEventNotifyControl::kContinue;
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
			LillithOnlyBox(stomach_rot_status.c_str());	// This is so rare, it can afford to have a message box.
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
			SKSE::log::info("XX-- our event handler for UD BONDAGE BOOTS SLOWDOWN REMOVAL!");
			DumpThoughts::throw_out_TTS_thought_message(std::format("YOU, the player, had your feet locked into bondage boots the whole time and couldn't get them off. This has slowed you down the whole time. But now you got rid of the locking bondage devices on your feet. But the important point is:  This means you can finally move much faster again!  (And you won't trip over your feet any more.)  Say as much in your response.")); //  + standard_thought_instruction;
			return RE::BSEventNotifyControl::kContinue;
		}



		// THIS SECTION OF THE CODE SHOULD BE CALLED VERY FREQUENTLY IN THE COURSE OF MAGIC EFFECTS.
		// THIS MEANS WE CAN PUT SOME EXPERIMENTAL EFFECTS HERE.
		//

		handle_iNeed::handle_iNeed_hunger_thirst_and_fatigue_stuff();

		handle_yps::handle_yps_fashion_detection_stuff();

		// Some new stuff there:  Test what we can easily get from SLSF 3.4.1
		// GlobalVariable Property SLSF_CurrentFamePCLocation_Group Auto Hidden
		// GlobalVariable Property SLSF_CurrentFamePCLocation_Khajiit Auto Hidden

		//auto* fameGroup = RE::TESForm::LookupByEditorID<RE::TESGlobal>("SLSF_CurrentFamePCLocation_Group");
		// SLSF_Reloaded_CurrentExhibitionistFame
		auto* fameGroup = RE::TESForm::LookupByEditorID<RE::TESGlobal>("SLSF_Reloaded_CurrentExhibitionistFame");

		if (fameGroup) {
			float value = fameGroup->value;
			SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-Lookup worked out of the box! SLSF_Reloaded_CurrentExhibitionistFame:  {}", value);
			
		} else {
			SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-Lookup DID NOT WORK STRAIGHT out of the box!");
		}

		struct FameGlobal
		{
			// index-number
			// previous_value
			// current_value
			uint32_t formID;
			const char* name;
			RE::TESGlobal* global;
			float previous_value;
			float current_value;
		};

		static std::vector<FameGlobal> fameGlobals = {
{ 0x0080C , "SLSF_Reloaded_CurrentSlutFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0080D , "SLSF_Reloaded_CurrentWhoreFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0080E , "SLSF_Reloaded_CurrentExhibitionistFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0080F , "SLSF_Reloaded_CurrentOralFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00810 , "SLSF_Reloaded_CurrentAnalFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00811 , "SLSF_Reloaded_CurrentNastyFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00812 , "SLSF_Reloaded_CurrentPregnantFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00813 , "SLSF_Reloaded_CurrentDominantFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00814 , "SLSF_Reloaded_CurrentSubmissiveFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00815 , "SLSF_Reloaded_CurrentSadistFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00816 , "SLSF_Reloaded_CurrentMasochistFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00817 , "SLSF_Reloaded_CurrentGentleFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00818 , "SLSF_Reloaded_CurrentMenFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00819 , "SLSF_Reloaded_CurrentWomenFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081A , "SLSF_Reloaded_CurrentOrcFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081B , "SLSF_Reloaded_CurrentKhajiitFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081C , "SLSF_Reloaded_CurrentArgonianFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081D , "SLSF_Reloaded_CurrentBestialityFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081E , "SLSF_Reloaded_CurrentGroupFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0081F , "SLSF_Reloaded_CurrentBoundFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00820 , "SLSF_Reloaded_CurrentTattooFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00821 , "SLSF_Reloaded_CurrentCumDumpFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00822 , "SLSF_Reloaded_CurrentCheatFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00823 , "SLSF_Reloaded_CurrentCuckFame" , nullptr, -99.0f, -99.0f } , 
{ 0x0082F , "SLSF_Reloaded_CurrentAirheadFame" , nullptr, -99.0f, -99.0f } , 
{ 0x00824 , "SLSF_Reloaded_OralCumVisible" , nullptr, -99.0f, -99.0f } , 
{ 0x00825 , "SLSF_Reloaded_AnalCumVisible" , nullptr, -99.0f, -99.0f } , 
{ 0x00826 , "SLSF_Reloaded_VaginalCumVisible" , nullptr, -99.0f, -99.0f } , 
{ 0x0082A , "SLSF_Reloaded_CommentFrequency" , nullptr, -99.0f, -99.0f } , 
{ 0x0082B , "SLSF_Reloaded_VisiblyBound" , nullptr, -99.0f, -99.0f } , 
{ 0x0082C , "SLSF_Reloaded_LightlyBound" , nullptr, -99.0f, -99.0f } , 
{ 0x0082D , "SLSF_Reloaded_HeavilyBound" , nullptr, -99.0f, -99.0f } , 
{ 0x0082E , "SLSF_Reloaded_Skooma" , nullptr, -99.0f, -99.0f } , 
{ 0x00830 , "SLSF_Reloaded_IsBelted" , nullptr, -99.0f, -99.0f } , 
{ 0x00831 , "SLSF_Reloaded_IsCollared" , nullptr, -99.0f, -99.0f } , 
{ 0x00835 , "SLSF_Reloaded_OralCumCount" , nullptr, -99.0f, -99.0f } , 
{ 0x00836 , "SLSF_Reloaded_AnalCumCount" , nullptr, -99.0f, -99.0f } , 
{ 0x00837 , "SLSF_Reloaded_VaginalCumCount" , nullptr, -99.0f, -99.0f } , 
{ 0x00838 , "SLSF_Reloaded_TotalCumCount" , nullptr, -99.0f, -99.0f } , 
{ 0x00829 , "SLSF_AllowComment" , nullptr, -99.0f, -99.0f } , 
{ 0x0080B , "SLSF_Reloaded_NPCScanSucess" , nullptr, -99.0f, -99.0f } , 
{ 0x00805 , "SLSF_Reloaded_CustomLocationCount" , nullptr, -99.0f, -99.0f } 
		};

		for (auto& fame : fameGlobals) {
			if (!fame.global) {
				fame.global = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>( fame.formID, "SLSF Reloaded.esp");
				SKSE::log::info("NEW+NEW+NEW+NEW:   WE HAD TO LOOK UP THE POINTER FOR SLSF- GLOBAL:  {}", fame.name);
				// In this case (game probably), we assign the current value and previous value as well, so there is 'no change'
				fame.previous_value = fame.global->value;
				fame.current_value = fame.global->value;
			} else {
				SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-GLOBALPointer was already there for:  {}", fame.name);
				float cur_value = fame.global->value;
				SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-GLOBALPointer -> Value was :  {}", cur_value);
				// Assign new value and message-box-report value changes for now
				
				fame.current_value = fame.global->value;
				if (fame.current_value != fame.previous_value) {
					if ( fame.current_value > fame.previous_value ) {
						// Increase:  say as much
						SKSE::log::info("SLSF-Handling: Detected and INCREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
						LillithOnlyBox(std::format("SLSF-Handling: Detected and INCREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
					} else {
						// Decrease:  say as much
						SKSE::log::info("SLSF-Handling: Detected and INCREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
						LillithOnlyBox(std::format("SLSF-Handling: Detected and DECREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
					}
				}
				fame.previous_value = fame.global->value;
			}
		}


		auto* global =
			RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(
				0x805,
				"SLSF Reloaded.esp");

		if (global) {
			float value_from_FormID = global->value;
			SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-Lookup via FormID:  {}", value_from_FormID);
		} else {
			SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-Lookup via FormID DID NOT WORK STRAIGHT out of the box!");

		}

/*
FE000805
FE00080B
FE00080C
FE00080D
FE00080E
FE00080F
FE000810
FE000811
FE000812
FE000813
FE000814
FE000815
FE000816
FE000817
FE000818
FE000819
FE00081A
FE00081B
FE00081C
FE00081D
FE00081E
FE00081F
FE000820
FE000821
FE000822
FE000823
FE000824
FE000825
FE000826
FE000829
FE00082A
FE00082B
FE00082C
FE00082D
FE00082E
FE00082F
FE000830
FE000831
FE000835
FE000836
FE000837
FE000838
*/


		// We have from another mod:
		// PlayerDirt = Game.GetFormFromFile(0x000DA8, "Bathing in Skyrim.esp") as GlobalVariable
		// DirtString = " Dirt: " + ((Bis.GetPlayerDirt() * 100.0) as Int) + "%"
		//
		// So this should be a value between 0 and 1, and 1 meaning 100% dirty, and 0 meaning completely clean.
		// This should allow for direct native access to the same from C++:
		//	
		auto* playerDirt =
			RE::TESDataHandler::GetSingleton()
				->LookupForm<RE::TESGlobal>(
					0x000DA8,
					"Bathing in Skyrim.esp");
						
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

		handle_AND_modesty::handle_AND_modesty_and_nakedness_stuff();
		
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



		// Formerly we had YPS handling here, not the funciton but hte contents of the funciton



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
			std::string  thought_message = std::format("=============== ************************************ LETS STOP HERE.  STOP NOW.  LETS STOP NOW AND CUT THIS SHORT:  THERE IS ONLY ONE THING REQUIRED RIGHT NOW:  GIVE BACK THE THOUGHT, THAT {{ player.name }}  WAS THINKING.  AND WHAT WAS SHE THINKING?  THE INFORMATION IS RIGHT HERE, BECAUSE WE KNOW THE FOLLWING SENTENCE:  {}  NOW THAT YOU HAVE THE INFORMATION, FORGET ALL THE REST AND JUST GIVE BACK THE RIGHT ANSWER!!! You can paraphrase and adapt it a little bit, but you don't have to.  THE REST BELOW CAN ALSO BE IGNORED, but any afterthought or narrator comment should then be in third person afterwards! ************************************ ================== ", a_event->strArg.c_str());
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

				// Let's also check, if it was player thoughts or player diagloge


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
		LillithOnlyBox("An unhandled mod-event was discovered: " + debug_message);

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
	"CaptiveDefeatInit"  // This is called every time a new cell is entered and merely a technical event,  probably for CaptivePlayer.
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