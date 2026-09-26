#include "handle_worn_equipment_change.h"
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

	// NOTE:  This event isn't just triggerd by change in WORN equipment, it is triggered by ANY change in 
	//        inventory.  Therefore we have to disable this message, at least for now, because it triggers
	//        excessively during normal gameplay.
	// LillithOnlyBox("Player worn-equipment change event received.  -->  Triggering a refresh of the currently worn items records.");
	refresh_currently_worn_item_records();

	return RE::BSEventNotifyControl::kContinue;
}
