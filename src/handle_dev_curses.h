#pragma once

#include "RE/Skyrim.h"

#include <string>

namespace handle_dev_curses
{
	void on_event_triggered(RE::StaticFunctionTag*, std::string eventName,
		std::string eventSource, RE::TESForm* affectedActor, std::string otherEventData);
}
