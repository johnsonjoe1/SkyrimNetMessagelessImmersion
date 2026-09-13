#include "handle_worn_equipment_change.h"
#include "misc.h"

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
	if (a_event) {
		LillithOnlyBox("Worn-equipment change event received.");
	}

	return RE::BSEventNotifyControl::kContinue;
}
