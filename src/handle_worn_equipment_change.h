#pragma once

#include "RE/Skyrim.h"
#include <string>
#include <vector>

struct CurrentlyWornItemRecord
{
	RE::FormID form_id{};
	RE::TESObjectARMO* armor{nullptr};
	std::uint32_t slot_mask{0};
	RE::TESBoundObject* item{nullptr};
	std::vector<RE::BGSKeyword*> keywords;
	int pelvic_property{0};
	int chest_property{0};
	int ass_property{0};
	int transparent_top_property{0};
};

extern std::vector<CurrentlyWornItemRecord> currently_worn_item_records;
extern std::vector<CurrentlyWornItemRecord> historic_worn_item_records;

void refresh_currently_worn_item_records();
bool player_has_item_in_inventory(const std::string& item_name);

class handle_worn_equipment_change : public RE::BSTEventSink<RE::TESEquipEvent>
{
public:
	static void register_event_handler();

	RE::BSEventNotifyControl ProcessEvent(
		const RE::TESEquipEvent* a_event,
		RE::BSTEventSource<RE::TESEquipEvent>* a_event_source) override;

private:
	static handle_worn_equipment_change* get_singleton();
};
