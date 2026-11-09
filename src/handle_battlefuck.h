#pragma once

#include "SKSE/SKSE.h"

namespace handle_battlefuck
{
	bool has_insertion_occurred();
	bool try_handle_mod_event(const SKSE::ModCallbackEvent* a_event);
}
