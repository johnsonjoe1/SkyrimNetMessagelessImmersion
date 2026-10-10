#include "handle_worn_equipment_change.h"
#include "DumpThoughts.h"
#include "log.h"
#include "misc.h"
#include <algorithm>

namespace logger = SKSE::log;

int get_pelvic_property(const CurrentlyWornItemRecord& worn_item);
int get_chest_property(const CurrentlyWornItemRecord& worn_item);
int get_ass_property(const CurrentlyWornItemRecord& worn_item);
int get_transparent_top_property(const CurrentlyWornItemRecord& worn_item);

std::vector<CurrentlyWornItemRecord> currently_worn_item_records;
std::vector<CurrentlyWornItemRecord> historic_worn_item_records;

void refresh_currently_worn_item_records()
{
	logger::info("ENTERING:  refresh_currently_worn_item_records");
	currently_worn_item_records.clear();
	// For our purposes, the actor is always the player character.
	RE::Actor* actor = RE::PlayerCharacter::GetSingleton();
	if (!actor) {
		logger::info("EMERGENCY-LEAVING:  refresh_currently_worn_item_records BECAUSE actor is null");
		return;
	}
	// Loop through inventory and cache everything currently worn.
	auto inventory = actor->GetInventory();
	for (const auto& [item, entry] : inventory)
	{
		if (!entry.second->IsWorn()) {
			continue;
		}
		auto armor = item->As<RE::TESObjectARMO>();
		if (!armor) {
			continue;
		}
		currently_worn_item_records.push_back(CurrentlyWornItemRecord{
			.form_id = item->GetFormID(),
			.armor = armor,
			.slot_mask = armor->GetSlotMask().underlying(),
			.item = item
		});
		auto& record = currently_worn_item_records.back();
		record.keywords.reserve(armor->numKeywords);
		for (std::uint32_t i = 0; i < armor->numKeywords; ++i) {
			auto* keyword = armor->keywords[i];
			if (keyword) {
				record.keywords.push_back(keyword);
			}
		}
		bool full_spam_of_equipment_keywords = true;
		if (full_spam_of_equipment_keywords) {
			logger::info("Worn item:  {}", item->GetName());
			for (const auto* keyword : record.keywords) {
				logger::info("    Keyword:  {}", keyword->GetFormEditorID());
			}
		}
		record.pelvic_property = get_pelvic_property(record);
		record.chest_property = get_chest_property(record);
		record.ass_property = get_ass_property(record);
		record.transparent_top_property = get_transparent_top_property(record);
		if (record.pelvic_property > 0 || record.chest_property > 0 || record.ass_property > 0) {
			logger::info("************");
			logger::info("************");
			logger::info("************");
			logger::info("Result of scan for FLASHING KEYWORDS:   Pelvic: {}, Chest: {}, Ass: {}", record.pelvic_property, record.chest_property, record.ass_property);
			logger::info("************");
			logger::info("************");
			logger::info("************");
		} else {
			logger::info("Result of scan for FLASHING KEYWORDS:   NOTHING!!!");
		}
	}

	std::sort(currently_worn_item_records.begin(), currently_worn_item_records.end(),
		[](const CurrentlyWornItemRecord& lhs, const CurrentlyWornItemRecord& rhs) {
			// logger::info("LEAVING:  refresh_currently_worn_item_records");
			return lhs.form_id < rhs.form_id;
		});
	currently_worn_item_records.erase(
		std::unique(
			currently_worn_item_records.begin(),
			currently_worn_item_records.end(),
			[](const CurrentlyWornItemRecord& lhs, const CurrentlyWornItemRecord& rhs) {
				//logger::info("LEAVING:  refresh_currently_worn_item_records");
				return lhs.form_id == rhs.form_id;
			}),
		currently_worn_item_records.end());

	logger::info("LEAVING:  refresh_currently_worn_item_records");
}

bool player_has_item_in_inventory(const std::string& item_name)
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player || item_name.empty()) {
		return false;
	}

	for (const auto& [item, entry] : player->GetInventory()) {
		if (!item || entry.first <= 0) {
			continue;
		}

		const auto* name = item->GetName();
		if (name && ::_stricmp(name, item_name.c_str()) == 0) {
			return true;
		}
	}

	return false;
}

std::string name_of_worn_slutty_item()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return {};
	}

	for (const auto& [item, entry] : player->GetInventory()) {
		if (!item || !entry.second || !entry.second->IsWorn()) {
			continue;
		}

		auto* armor = item->As<RE::TESObjectARMO>();
		if (!armor) {
			continue;
		}

		const auto* name = item->GetName();
		if (!name || !*name) {
			continue;
		}

		for (std::uint32_t index = 0; index < armor->numKeywords; ++index) {
			auto* keyword = armor->keywords[index];
			const auto* editorID = keyword ? keyword->GetFormEditorID() : nullptr;
			if (editorID && std::strcmp(editorID, "CC_SluttyItem") == 0) {
				return name;
			}
		}
	}

	return {};
}

handle_worn_equipment_change* handle_worn_equipment_change::get_singleton()
{
	static handle_worn_equipment_change singleton;
	return &singleton;
}

void handle_worn_equipment_change::register_event_handler()
{
	RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESEquipEvent>(get_singleton());
}

RE::BSEventNotifyControl handle_worn_equipment_change::ProcessEvent(
	const RE::TESEquipEvent* a_event,
	RE::BSTEventSource<RE::TESEquipEvent>*)
{
	if (!a_event || a_event->actor.get() != RE::PlayerCharacter::GetSingleton()) {
		return RE::BSEventNotifyControl::kContinue;
	}
	logger::info("Hook ENTERED AT ALL");

	auto* item = RE::TESForm::LookupByID(a_event->baseObject);
	const auto* item_name = item ? item->GetName() : nullptr;
	logger::info("Hook TRIGGERED for item {}: {} (FormID {:08X})",
		a_event->equipped ? "equipped" : "unequipped",
		item_name ? item_name : "<unknown>",
		a_event->baseObject);

	// NOTE:  This event isn't just triggerd by change in WORN equipment, it is triggered by ANY change in worn items.
	//        Therefore we have to disable this message, at least for now, because it triggers excessively during normal gameplay.
	// LillithOnlyBox("Player worn-equipment change event received.  -->  Triggering a refresh of the currently worn items records.");
	refresh_currently_worn_item_records();

	return RE::BSEventNotifyControl::kContinue;
}

handle_inventory_change* handle_inventory_change::get_singleton()
{
	static handle_inventory_change singleton;
	return &singleton;
}

void handle_inventory_change::register_event_handler()
{
	RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESContainerChangedEvent>(get_singleton());
}

RE::BSEventNotifyControl handle_inventory_change::ProcessEvent(
	const RE::TESContainerChangedEvent* a_event,
	RE::BSTEventSource<RE::TESContainerChangedEvent>*)
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!a_event || !player) {
		return RE::BSEventNotifyControl::kContinue;
	}

	const auto player_form_id = player->GetFormID();
	const bool added = a_event->newContainer == player_form_id && a_event->oldContainer != player_form_id;
	const bool removed = a_event->oldContainer == player_form_id && a_event->newContainer != player_form_id;
	if (!added && !removed) {
		return RE::BSEventNotifyControl::kContinue;
	}

	auto* item = RE::TESForm::LookupByID(a_event->baseObj);
	const auto* item_name = item ? item->GetName() : nullptr;
	std::int32_t inventory_count_after = 0;
	for (const auto& [inventory_item, entry] : player->GetInventory()) {
		if (inventory_item && inventory_item->GetFormID() == a_event->baseObj) {
			inventory_count_after = entry.first;
			break;
		}
	}
	const auto event_count = static_cast<std::int64_t>(a_event->itemCount);
	const auto changed_count = event_count < 0 ? -event_count : event_count;
	const auto inventory_count_before = added ?
		static_cast<std::int64_t>(inventory_count_after) - changed_count :
		static_cast<std::int64_t>(inventory_count_after) + changed_count;

	logger::info("Player INVENTORY CHANGED: item {}: {} (FormID {:08X}), count before: {}, count after: {}",
		added ? "added" : "removed",
		item_name ? item_name : "<unknown>",
		a_event->baseObj,
		inventory_count_before,
		inventory_count_after);

	if (added && item_name && _strnicmp(item_name, "Waifu ", 6) == 0) {
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
			"You just found another Waifu card. React to finding another card and make it clear that it is a Waifu card.");
	}

	if (item_name && _strnicmp(item_name, "Lockpick", 8) == 0) {
		// Depending on counts, we might comment on lockpicks
		if (added && (inventory_count_before<20) && (inventory_count_after>inventory_count_before)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"You just obtained some lockpicks. React to finding in a positive way, because you were running low a bit on lockpicks with less than 20 in your inventory.  Mention that you were running low on lockpicks in your response.");
		} else if (removed && (inventory_count_before>3) && (inventory_count_after==3)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"You are down to your last 3 lockpicks. You have to be careful now. React to this finding and mention that you are down to your last 3 lockpicks.");
		} else if (removed && (inventory_count_before>0) && (inventory_count_after==0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"Your lockpicks are all gone.  How are you supposed to get out of a locked situation now? React to this situation and mention that you have no lockpicks left anymore now.");
		}	
	}

	if (item_name && _strnicmp(item_name, "Chastity Key", 11) == 0) {
		// Depending on counts, we might comment on Chastity key
		if (added && (inventory_count_before==0) && (inventory_count_after>0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"You just obtained a Chastity Key. Finally!  React to finding it and make it clear that it is a Chastity Key and that you can now get out of chastity bras and chastity belts now, in case that you ever need to.");
		} else if (removed && (inventory_count_before>0) && (inventory_count_after==0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"Your Chastity Keys are all gone. React to this situation and mention that you have no Chastity Keys left and can't get out of chastity bras and chastity belts any more in case you ever need to.  Be sure to mention in your response, that you now do not have any Chastity Keys left.");
		}	
	}

	if (item_name && _strnicmp(item_name, "Restraints Key", 11) == 0) {
		// Depending on counts, we might comment on Restraints key
		if (added && (inventory_count_before==0) && (inventory_count_after>0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"You just obtained a Restraints Key. Finally!  React to finding it and make it clear that it is a Restraints Key and that you can now get out of restraints now, in case that you ever need to.");
		} else if (removed && (inventory_count_before>0) && (inventory_count_after==0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"Your Restraints Keys are all gone. React to this situation and mention that you have no Restraints Keys left and can't get out of restraints any more in case you ever need to.  Be sure to mention in your response, that you now do not have any Restraints Keys left.");
		}	
	}

	if (item_name && _strnicmp(item_name, "Piercing Removal Tool", 21) == 0) {
		// Depending on counts, we might comment on Piercing Removal Tool
		if (added && (inventory_count_before==0) && (inventory_count_after>0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"You just obtained a Piercing Removal Tool. Finally!  React to finding it and make it clear that it is a Piercing Removal Tool and that you can now remove piercings now, in case that you ever need to.");
		} else if (removed && (inventory_count_before>0) && (inventory_count_after==0)) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(
				"Your Piercing Removal Tools are all gone. React to this situation and mention that you have no Piercing Removal Tools left and can't remove piercings any more in case you ever need to.  Be sure to mention in your response, that you now do not have any Piercing Removal Tools left.");
		}	
	}

	return RE::BSEventNotifyControl::kContinue;
}
