#include "player_thought_history.h"

#include "log.h"

#include <ctime>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

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

    SKSE::log::info(">>>>>>>>> The previous log of all player thoughts now looks like this:");
    PlayerThoughtHistory::LogRecords();

}

const std::vector<PlayerThoughtRecord>& PlayerThoughtHistory::GetRecords()
{
	return records;
}

void PlayerThoughtHistory::LogRecords()
{
	if (records.empty()) {
		SKSE::log::info("No SkyrimNet player thoughts have been recorded this session.");
		return;
	}

	SKSE::log::info("=====SKYRIMNET PLAYER THOUGHT HISTORY ({} records)=====", records.size());
	for (const auto& record : records) {
		const auto timestamp = std::chrono::system_clock::to_time_t(record.timestamp);
		std::tm localTime{};
		localtime_s(&localTime, &timestamp);

		std::ostringstream timestampText;
		timestampText << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
		SKSE::log::info("[{}] {}", timestampText.str(), record.text);
	}
}

std::string PlayerThoughtHistory::GetLogSince(std::chrono::system_clock::time_point a_since)
{
	std::ostringstream thoughtLog;
	for (const auto& record : records) {
		if (record.timestamp < a_since) {
			continue;
		}

		const auto timestamp = std::chrono::system_clock::to_time_t(record.timestamp);
		std::tm localTime{};
		localtime_s(&localTime, &timestamp);

		thoughtLog << '[' << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] " << record.text << '\n';
	}

	return thoughtLog.str();
}


