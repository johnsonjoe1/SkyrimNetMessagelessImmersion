#pragma once

#include "RE/Skyrim.h"

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
