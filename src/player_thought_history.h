#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

struct PlayerThoughtRecord
{
	std::chrono::system_clock::time_point timestamp;
	std::string text;
};

namespace PlayerThoughtHistory
{
	void TryRecordSkyrimNetSpeech(std::string_view a_eventName, std::string_view a_payload);
	const std::vector<PlayerThoughtRecord>& GetRecords();
	void LogRecords();
	std::string GetLogSince(std::chrono::system_clock::time_point a_since);
	std::string get_thought_history_as_a_string(std::uint32_t lookback_period_in_seconds);
}
