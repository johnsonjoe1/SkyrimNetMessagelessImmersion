#pragma once

#include "SKSE/SKSE.h"

#include <chrono>

namespace handle_jailrape
{
	bool try_handle_mod_event(
		const SKSE::ModCallbackEvent* a_event,
		std::chrono::steady_clock::time_point& a_lastThoughtTimestamp);
}