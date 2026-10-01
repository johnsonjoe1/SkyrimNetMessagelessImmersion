#pragma once

#include "SKSE/SKSE.h"

#include <string_view>

namespace handle_jailrape
{
	bool is_known_irrelevant_event(std::string_view a_eventName);
	bool try_handle_mod_event(const SKSE::ModCallbackEvent* a_event);
}