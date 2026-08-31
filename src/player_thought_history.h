#pragma once

#include <chrono>
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
}