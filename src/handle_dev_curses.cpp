#include "handle_dev_curses.h"

#include "SKSE/SKSE.h"

#include <format>
#include <utility>

void handle_dev_curses::on_event_triggered(RE::StaticFunctionTag*, std::string eventName,
	std::string eventSource, RE::TESForm* affectedActor, std::string otherEventData)
{
	const RE::FormID actorID = affectedActor ? affectedActor->GetFormID() : 0;
	// Resolve the form and show the diagnostic on the game thread; retain no raw form pointer.
	SKSE::GetTaskInterface()->AddTask([eventName = std::move(eventName),
		eventSource = std::move(eventSource), actorID, otherEventData = std::move(otherEventData)] {
		auto* actorForm = actorID ? RE::TESForm::LookupByID(actorID) : nullptr;
		const auto message = std::format(
			"SNMI: DCurses_EventTriggered\nEvent: {}\nSource: {}\nActor: {} [{:08X}]\nOther data: {}",
			eventName, eventSource, actorForm ? actorForm->GetName() : "None/unavailable",
			actorID, otherEventData);
		SKSE::log::info("{}", message);
		RE::DebugMessageBox(message.c_str());
	});
}
