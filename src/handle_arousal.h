#pragma once
#include <cstddef>
#include <string>

namespace handle_arousal
{
	void log_player_arousal_state();
	void reset_arousal_tracking();
	// Names of the strongest positive contributors from the latest completed periodic check.
	// Defaults to three; pass 2 for two. Returns fewer names if fewer contribute, or empty if unavailable.
	std::string get_top_arousal_contributors(std::size_t a_count = 3);
	// Prompt sentence explaining up to three ranked positive contributors; pass 2 for two.
	// Returns empty when no positive contributions are available.
	std::string get_top_arousal_contribution_string(std::size_t a_count = 3);
	std::string legacy_get_top_arousal_contributors(std::size_t a_count = 3);
}
