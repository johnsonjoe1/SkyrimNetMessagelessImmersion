#include "player_thought_history.h"

#include "log.h"

#include <nlohmann/json.hpp>

namespace
{
	std::vector<PlayerThoughtRecord> records;
}

void PlayerThoughtHistory::TryRecordSkyrimNetSpeech(std::string_view a_eventName, std::string_view a_payload)
{
	if (a_eventName != "SkyrimNet_SpeechStarted") {
		return;
	}

	const auto payload = nlohmann::json::parse(a_payload, nullptr, false);
	if (payload.is_discarded()) {
		SKSE::log::warn("Could not parse SkyrimNet_SpeechStarted payload as JSON.");
		return;
	}

	if (payload.value("speakerName", "") != "Player Thoughts" || payload.value("targetName", "") != "Player") {
		return;
	}

	const auto text = payload.value("text", "");
	if (text.empty()) {
		SKSE::log::warn("SkyrimNet player-thought event contained no text.");
		return;
	}

	records.push_back({ std::chrono::system_clock::now(), text });
	SKSE::log::info("Recorded SkyrimNet player thought: {}", text);

    
}

const std::vector<PlayerThoughtRecord>& PlayerThoughtHistory::GetRecords()
{
	return records;
}