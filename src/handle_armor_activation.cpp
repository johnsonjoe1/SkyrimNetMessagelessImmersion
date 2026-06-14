#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include <string_view>
#include <unordered_set>

void handle_armor_item_activation(RE::TESBoundObject *base)
	{
	SKSE::log::info("That activated object seems to be an armor item, so we will handle it (though not much so far).");
	// LillithOnlyBox("plugin.cpp:   Some piece of arour was activated!!!!");

	auto armor = base->As<RE::TESObjectARMO>();
	if (!armor) {
		LillithOnlyBox("plugin.cpp:   SEVERE ERROR!! CASTING the armor item at TESObjectARMO failed!!!  Exiting this handler!");
		return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
	}

	// State the name of the armour piece
	SKSE::log::info("Armor item: {}", armor->GetName());
	// std::string name_string = std::format("FOUND and Armor item named: {} " , armor->GetName());
	
	std::string combined_string = std::format("ACTIVATED and Armor item named: {} with keywords: " , armor->GetName());
	// LillithOnlyBox(std::format("FOUND and Armor item named: {} " , armor->GetName()));

	// Let's put all the keywords into an unordered set...
	std::unordered_set<std::string> keywordSet;
	for (auto keyword : armor->GetKeywords()) {
		if (!keyword) {
			continue;
		}
		auto edid = keyword->GetFormEditorID();
		if (edid) {
			keywordSet.insert(edid);
		}
	}

	// run through the list of keywords
	const auto& keywords = armor->GetKeywords();
	for (auto keyword : keywords) {
		if (!keyword) {
			continue;
		} 
		combined_string += keyword->GetFormEditorID();
		combined_string += " , ";
		//LillithOnlyBox(std::format("Keyword: {} ({:08X})", keyword->GetFormEditorID(), keyword->GetFormID()));
		SKSE::log::info("Keyword: {} ({:08X})", keyword->GetFormEditorID(), keyword->GetFormID() );
	}
	LillithOnlyBox(combined_string);

	std::string thought_string = std::format("This {} is " , armor->GetName() );

	bool we_have_something_interesting_to_say = false;

	if (keywordSet.contains("ArmorHeavy")) {
		thought_string += " a piece of heavy armor";
	} else if (keywordSet.contains("ArmorLight")) {
		thought_string += " a piece of light armor";
	} else if (keywordSet.contains("ArmorClothing")) {
		thought_string += " a piece of clothing";
	}

	// Handle YPS special case of specific fashion purpose attributes
	if (keywordSet.contains("ypsClothingUgly")) {
		thought_string += " and it's very ugly, according to the fashion vendor Y.P.S.";
		we_have_something_interesting_to_say = true;
	} else if (keywordSet.contains("ypsClothingOrdinary")) {
		thought_string += " and I would surely be looking really ordinary with that on, according to the fashion vendor Y.P.S.";
		we_have_something_interesting_to_say = true;
	} else if (keywordSet.contains("ypsClothingCute")) {
		thought_string += " and it would be looking kind of cute on me, according to the fashion vendor Y.P.S.";
		we_have_something_interesting_to_say = true;
	} else if (keywordSet.contains("ypsClothingFashion")) {
		thought_string += " and would make me look really fashionabe, for sure, according to the fashion vendor Y.P.S.";
		we_have_something_interesting_to_say = true;
	} else if (keywordSet.contains("ypsClothingSexy")) {
		thought_string += " and would make me look outright sexy, according to the fashion vendor Y.P.S.";
		we_have_something_interesting_to_say = true;
	} else { thought_string += ".";  }
	// Handle YPS special case of fingerless gloves
	if (keywordSet.contains("ypsFingerlessGloves")) {
		thought_string += " This is also an original fingerless glove, that could be worn even with very long nails, so a true keeper.";
		we_have_something_interesting_to_say = true;
	}
	
	if (keywordSet.contains("SLA_ArmorSpendex")) {
		thought_string += " This is made from some kind of spandex material.";
		we_have_something_interesting_to_say = true;
	}
	if (keywordSet.contains("SLA_ArmorLewdLeotard")) {
		thought_string += " It is shaped as a lewd leotard.";
		we_have_something_interesting_to_say = true;
	}			
	if (keywordSet.contains("EroticArmor")) {
		thought_string += " I guess it could be called an erotic armor.";
		we_have_something_interesting_to_say = true;
	}	
	if (keywordSet.contains("bikini")) {
		thought_string += " It is part of a bikini armor outfit.";
		we_have_something_interesting_to_say = true;
	}	

	if ( (keywordSet.contains("AND_PelvicFlashRisk")) |
			(keywordSet.contains("AND_PelvicFlashRiskExtreme")) |
			(keywordSet.contains("AND_PelvicFlashRiskHigh")) |
			(keywordSet.contains("AND_PelvicFlashRiskLow")) |
			(keywordSet.contains("AND_PelvicFlashRiskUltra")) |				
			(keywordSet.contains("AND_AssFlashRisk")) |
			(keywordSet.contains("AND_AssFlashRiskHigh")) |
			(keywordSet.contains("AND_AssFlashRiskExtreme")) |
			(keywordSet.contains("AND_AssFlashRiskUltra")) |				
			(keywordSet.contains("AND_AssFlashRiskLow")) ) 
	{
		thought_string += " I bet that with this thing on, I would have a risk of flashing my ass or pelvis upon every move. ";
		we_have_something_interesting_to_say = true;
	}
	if ( (keywordSet.contains("AND_ChestFlashRisk")) |
			(keywordSet.contains("AND_ChestFlashRiskLow")) |
			(keywordSet.contains("AND_ChestFlashRiskHigh")) |
			(keywordSet.contains("AND_ChestFlashRiskExtreme")) |
			(keywordSet.contains("AND_ChestFlashRiskUltra")) ) 
	{
		thought_string += " I bet that with this thing on, I would have a risk of flashing my chest upon every move. ";
		we_have_something_interesting_to_say = true;
	}


	LillithOnlyBox(thought_string);
	// 
	// DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_string);
	if (we_have_something_interesting_to_say) {
		DumpThoughts::throw_out_AS_LITTERAL_AS_POSSIBLE_thought_message(thought_string);
	}

}

