#include "handle_arousal.h"
#include "DumpThoughts.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "RE/S/SendHUDMessage.h"
#include "misc.h"
#include <Windows.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	// Accessed only on the game thread, including results delivered by Papyrus callbacks.
	std::optional<float> previousArousal;
	std::uint64_t arousalSession = 0;
	std::uint64_t nextArousalSample = 0;
	std::uint64_t lastArousalSample = 0;
	constexpr int arousalThoughtCooldownSeconds = 180;
	std::chrono::steady_clock::time_point lastArousedThoughtTimestamp =
		std::chrono::steady_clock::now() - std::chrono::seconds(arousalThoughtCooldownSeconds);
	std::chrono::steady_clock::time_point lastDistractedThoughtTimestamp =
		std::chrono::steady_clock::now() - std::chrono::seconds(arousalThoughtCooldownSeconds);
	std::chrono::steady_clock::time_point lastNeedSexThoughtTimestamp =
		std::chrono::steady_clock::now() - std::chrono::seconds(arousalThoughtCooldownSeconds);
	std::chrono::steady_clock::time_point lastFocusedThoughtTimestamp =
		std::chrono::steady_clock::now() - std::chrono::seconds(arousalThoughtCooldownSeconds);
	std::mutex contributorMutex;
	std::map<std::string, float> latestContributions;
	std::uint64_t lastContributionSample = 0;

	std::string contributor_name(const std::string& a_name)
	{
		if (a_name == "Static:Timed") return "timed arousal";
		if (a_name == "Static:Naked") return "nudity exposure";
		if (a_name == "Static:Exhibitionist") return "exhibitionism";
		if (a_name == "Static:Sex") return "SexLab sexual activity";
		if (a_name == "Static:OSex") return "OStim sexual activity";
		if (a_name == "Static:Devices") return "devious devices";
		if (a_name == "Static:Sleep") return "sleep";
		if (a_name == "Static:Legacy") return "legacy exposure";
		if (a_name == "Dynamic:UnforgivingDevices") return "Unforgiving Devices";
		const auto separator = a_name.find(':');
		return separator == std::string::npos ? a_name : a_name.substr(separator + 1);
	}

	std::vector<std::pair<std::string, float>> ranked_arousal_contributors()
	{
		std::vector<std::pair<std::string, float>> ranked;
		{
			std::scoped_lock lock(contributorMutex);
			for (const auto& [name, value] : latestContributions) {
				if (std::isfinite(value) && value > 0.0f) {
					ranked.emplace_back(name, value);
				}
			}
		}
		std::sort(ranked.begin(), ranked.end(), [](const auto& a_left, const auto& a_right) {
			return a_left.second != a_right.second ? a_left.second > a_right.second : a_left.first < a_right.first;
		});
		return ranked;
	}

	std::string contributor_reason(const std::string& a_name)
	{
		using Translation = std::pair<std::string_view, std::string_view>;
		static constexpr std::array translations{
			Translation{ "timed arousal", "that you have gone for quite some time without sexual relief" },
			Translation{ "Devious Devices", "that I have been freshly bound into submission with these bondage devices" },
			Translation{ "DeviousDevices", "that I have been freshly bound into submission with these bondage devices" },
			Translation{ "Unforgiving Devices", "that I have been freshly bound into submission with these bondage devices" },
			Translation{ "Devices", "that I'm still wearing bondage gear" },
			Translation{ "nudity exposure", "that you have been exposed to nudity" },
			Translation{ "exhibitionism", "that being seen exposed is arousing you" },
			Translation{ "SexLab sexual activity", "that sexual activity has been arousing you" },
			Translation{ "OStim sexual activity", "that sexual activity has been arousing you" }
		};
		// The static Devices effect is displayed as "devious devices" by the name-only helper.
		const std::string name = a_name == "Static:Devices" ? "Devices" : contributor_name(a_name);
		for (const auto& [key, reason] : translations) {
			if (name == key) {
				return std::string(reason);
			}
		}
		return "the arousing effect of " + name;
	}

	bool submit_arousal_thought_if_off_cooldown(
		std::string& a_text,
		std::chrono::steady_clock::time_point& a_lastThoughtTimestamp)
	{
		const auto attribution = handle_arousal::get_top_arousal_contribution_string();
		if (!attribution.empty()) {
			a_text += " " + attribution;
		}
		if (!cooldown_has_passed(a_lastThoughtTimestamp, arousalThoughtCooldownSeconds)) {
			SKSE::log::info(
				"[Arousal] Thought suppressed because its {}-second cooldown has not elapsed: {}",
				arousalThoughtCooldownSeconds,
				a_text);
			return false;
		}
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(a_text);
		a_lastThoughtTimestamp = std::chrono::steady_clock::now();
		return true;
	}

	void handle_arousal_sample(float a_arousal, std::uint64_t a_session, std::uint64_t a_sample)
	{
		if (a_session != arousalSession || a_sample <= lastArousalSample || !std::isfinite(a_arousal)) {
			return;
		}
		lastArousalSample = a_sample;
		const bool enteredArousedRange = previousArousal && *previousArousal < 70.0f &&
			a_arousal >= 70.0f && a_arousal < 90.0f;
		const bool enteredDistractedRange = previousArousal && *previousArousal < 50.0f &&
			a_arousal >= 50.0f && a_arousal < 70.0f;
		const bool enteredNeedSexRange = previousArousal && *previousArousal < 90.0f && a_arousal >= 90.0f;
		const bool enteredFocusedRange = previousArousal && *previousArousal > 20.0f &&
			a_arousal >= 0.0f && a_arousal <= 20.0f;
		previousArousal = a_arousal;
		if (enteredArousedRange && !DumpThoughts::too_early_after_game_load()) {
			std::string thought_text = "Your general arousal and horniness have just noticeably increased. You are feeling horny now, "
				"and your growing desire is becoming harder to ignore and more distracting. Express this increase in a brief "
				"first-person thought in character.  Make it clear that you are talking about horniness.  Don't mention numerical values or game mechanics.  Mention the reason why you are feeling this way.";
			if (submit_arousal_thought_if_off_cooldown(thought_text, lastArousedThoughtTimestamp)) {
				SKSE::log::info("[Arousal] Player crossed from below 70 into 70-89; submitted an arousal thought.");
				LillithOnlyBox(std::format("Arousal crossed over 70 into 70-89: THE THOUGHT TEXT PROMPT IS: {}", thought_text));
			}
		} else if (enteredDistractedRange && !DumpThoughts::too_early_after_game_load()) {
			std::string thought_text = "Your general arousal and horniness have just increased. You are starting to feel noticeably turned on, "
				"and thoughts of pleasure are beginning to distract you. Express this growing desire in a brief "
				"first-person thought in character. Make it clear that you are talking about horniness.  Make it clear that you are feeling horny.  Don't mention numerical values or game mechanics.  Mention the reason why you are feeling this way.";
			if (submit_arousal_thought_if_off_cooldown(thought_text, lastDistractedThoughtTimestamp)) {
				SKSE::log::info("[Arousal] Player crossed from below 50 into 50-69; submitted an arousal thought.");
				LillithOnlyBox(std::format("Arousal crossed over 50 into 50-69: THE THOUGHT TEXT PROMPT IS: {}", thought_text));
			}
		} else if (enteredNeedSexRange && !DumpThoughts::too_early_after_game_load()) {
			std::string thought_text = "Your general arousal and horniness have just increased to an intense level. "
				"Your need for sexual relief is now very strong, and thoughts of pleasure are making it difficult to concentrate. "
				"Express this heightened desire in a brief first-person thought in character. Make it clear that you are talking about horniness.  Don't mention numerical values or game mechanics.  Make it clear that you are feeling very horny and urgently craving sexual relief, without mentioning numerical values or game mechanics.  Mention the reason why you are feeling this way.";
			if (submit_arousal_thought_if_off_cooldown(thought_text, lastNeedSexThoughtTimestamp)) {
				SKSE::log::info("[Arousal] Player crossed from below 90 into 90+; submitted an arousal thought.");
				LillithOnlyBox(std::format("Arousal crossed over 90 into 90+: THE THOUGHT TEXT PROMPT IS: {}", thought_text));
			}
		} else if (enteredFocusedRange && !DumpThoughts::too_early_after_game_load()) {
			std::string thought_text = "Your general arousal and horniness have now subsided to a low level. "
				"Your desire is less distracting, and you feel calmer and able to focus more clearly again. "
				"Express this easing of arousal in a brief first-person thought in character.  Make it clear that you are talking about horniness.  Don't mention numerical values or game mechanics.  Make it clear that you are feeling less horny.  Mention the reason why you are feeling this way.";
			if (submit_arousal_thought_if_off_cooldown(thought_text, lastFocusedThoughtTimestamp)) {
				SKSE::log::info("[Arousal] Player crossed from above 20 into 0-20; submitted an arousal thought.");
				LillithOnlyBox(std::format("Arousal dropped from above 20 into 0-20: THE THOUGHT TEXT PROMPT IS: {}", thought_text));
			}
		}
	}

	struct ArousalHUDReport
	{
		const std::uint64_t session = arousalSession;
		const std::uint64_t sample = ++nextArousalSample;
		std::mutex mutex;
		std::size_t pending = 1;  // Keep the report open while the initial requests are queued.
		std::optional<float> total;
		std::map<std::string, float> contributions;
		bool partial = false;

		void add_request()
		{
			std::scoped_lock lock(mutex);
			++pending;
		}

		void finish_request(bool a_failed = false)
		{
			std::string message;
			{
				std::scoped_lock lock(mutex);
				partial |= a_failed;
				if (--pending != 0 || !total) {
					return;
				}
				message = std::format("[Arousal] {:.1f} | ", *total);
				bool first = true;
				for (const auto& [name, value] : contributions) {
					if (value == 0.0f) {
						continue;
					}
					message += std::format("{}{} {:+.1f}", first ? "" : ", ", name, value);
					first = false;
				}
				if (first) {
					message += "no nonzero effects";
				}
				if (partial) {
					message += " (partial read)";
				}
			}
			// Papyrus callbacks can run off the main thread; send the notification on the game thread.
			if (auto* tasks = SKSE::GetTaskInterface()) {
				tasks->AddTask([message = std::move(message), values = contributions, arousal = *total, session = session, sample = sample] {
					if (session != arousalSession || sample <= lastContributionSample) {
						return;
					}
					lastContributionSample = sample;
					{
						std::scoped_lock lock(contributorMutex);
						latestContributions = values;
					}
					// Generate the thought after publishing attribution from this same completed sample.
					handle_arousal_sample(arousal, session, sample);

					const auto* player = RE::PlayerCharacter::GetSingleton();
					const auto* playerName = player ? player->GetName() : nullptr;
					if (playerName && std::string_view(playerName) == "Lillith") {
						RE::SendHUDMessage::ShowHUDMessage(message.c_str());
					}
				});
			}
		}
	};

	using HUDReport = std::shared_ptr<ArousalHUDReport>;
	using ResultHandler = std::function<void(RE::BSScript::Variable)>;

	class ArousalResult final : public RE::BSScript::IStackCallbackFunctor
	{
	public:
		explicit ArousalResult(ResultHandler a_handler) : handler(std::move(a_handler)) {}

		void operator()(RE::BSScript::Variable a_result) override
		{
			handler(std::move(a_result));
		}

		void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}

	private:
		ResultHandler handler;
	};

	ResultHandler log_value(std::string a_label)
	{
		return [label = std::move(a_label)](RE::BSScript::Variable a_result) {
			if (a_result.IsFloat()) {
				SKSE::log::info("[Arousal] Player {} = {}", label, a_result.GetFloat());
			} else if (a_result.IsBool()) {
				SKSE::log::info("[Arousal] Player {} = {}", label, a_result.GetBool());
			} else if (a_result.IsInt()) {
				SKSE::log::info("[Arousal] Player {} = {}", label, a_result.GetSInt());
			} else {
				SKSE::log::warn("[Arousal] Player {} returned an unexpected value type", label);
			}
		};
	}

	void query(const char* a_script, const char* a_getter,
		RE::BSScript::IFunctionArguments* a_args, ResultHandler a_handler, HUDReport a_hud = nullptr)
	{
		if (a_hud) {
			a_hud->add_request();
		}
		auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!vm) {
			delete a_args;
			if (a_hud) {
				a_hud->finish_request(true);
			}
			return;
		}
		RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback{ new ArousalResult(
			[handler = std::move(a_handler), a_hud](RE::BSScript::Variable a_result) {
				handler(std::move(a_result));  // Nested requests join the same report before this one finishes.
				if (a_hud) {
					a_hud->finish_request();
				}
			}) };
		if (!vm->DispatchStaticCall(a_script, a_getter, a_args, callback)) {
			SKSE::log::warn("[Arousal] Could not dispatch {}.{}", a_script, a_getter);
			if (a_hud) {
				a_hud->finish_request(true);
			}
		}
	}

	ResultHandler log_contribution(std::string a_name, HUDReport a_hud)
	{
		return [name = std::move(a_name), a_hud](RE::BSScript::Variable a_result) {
			log_value(name + " contribution")(a_result);
			std::scoped_lock lock(a_hud->mutex);
			if (a_result.IsFloat()) {
				// Timed includes the full Timed/TimedCycle group value. Count that group once.
				// The activity flag only describes timed updates, not whether a value contributes.
				if (name != "Static:TimedCycle") {
					a_hud->contributions[name] = a_result.GetFloat();
				}
			} else {
				a_hud->partial = true;
			}
		};
	}

	void log_static_effect(const char* a_name, HUDReport a_hud)
	{
		// Resolve IDs each time: effect registration order and enabled plugins can change.
		query("slaInternalModules", "GetStaticEffectId", RE::MakeFunctionArguments(RE::BSFixedString(a_name)),
			[a_name, a_hud](RE::BSScript::Variable a_result) {
				if (!a_result.IsInt()) {
					SKSE::log::warn("[Arousal] Could not resolve static effect {}", a_name);
					std::scoped_lock lock(a_hud->mutex);
					a_hud->partial = true;
					return;
				}
				const auto id = a_result.GetSInt();
				if (id < 0) {
					SKSE::log::info("[Arousal] Player static effect {} is not registered", a_name);
					return;
				}
				auto* player = static_cast<RE::Actor*>(RE::PlayerCharacter::GetSingleton());
				if (!player) {
					return;
				}
				query("slaInternalModules", "IsStaticEffectActive", RE::MakeFunctionArguments(static_cast<RE::Actor*>(player), std::int32_t(id)),
					[a_name, id, a_hud](RE::BSScript::Variable a_active) {
						log_value(std::format("static effect {} active", a_name))(a_active);
						if (!a_active.IsBool()) {
							std::scoped_lock lock(a_hud->mutex);
							a_hud->partial = true;
							return;
						}
						auto* actor = static_cast<RE::Actor*>(RE::PlayerCharacter::GetSingleton());
						if (actor) {
							query("slaInternalModules", "GetStaticEffectValue",
								RE::MakeFunctionArguments(static_cast<RE::Actor*>(actor), std::int32_t(id)),
								log_contribution(std::format("Static:{}", a_name), a_hud), a_hud);
						}
					}, a_hud);
			}, a_hud);
	}

	void log_dynamic_effects(RE::Actor* a_player, HUDReport a_hud)
	{
		query("slaInternalModules", "GetDynamicEffectCount", RE::MakeFunctionArguments(static_cast<RE::Actor*>(a_player)),
			[a_hud](RE::BSScript::Variable a_result) {
				if (!a_result.IsInt() || a_result.GetSInt() < 0) {
					SKSE::log::warn("[Arousal] Could not read dynamic effect count");
					std::scoped_lock lock(a_hud->mutex);
					a_hud->partial = true;
					return;
				}
				const auto count = a_result.GetSInt();
				SKSE::log::info("[Arousal] Player dynamic effect count = {}", count);
				auto* player = static_cast<RE::Actor*>(RE::PlayerCharacter::GetSingleton());
				if (!player) {
					return;
				}
				for (std::int32_t i = 0; i < count; ++i) {
					query("slaInternalModules", "GetDynamicEffect", RE::MakeFunctionArguments(static_cast<RE::Actor*>(player), std::int32_t(i)),
						[a_hud](RE::BSScript::Variable a_name) {
							if (!a_name.IsString() || a_name.GetString().empty()) {
								return;  // Effect may have disappeared since the count was read.
							}
							const std::string name(a_name.GetString());
							auto* actor = static_cast<RE::Actor*>(RE::PlayerCharacter::GetSingleton());
							if (actor) {
								// Read by name so removal/reordering cannot pair a name with another effect's value.
								query("SloangNative", "GetDynamicEffectValue",
								RE::MakeFunctionArguments(static_cast<RE::Actor*>(actor), RE::BSFixedString(name)),
									log_contribution(std::format("Dynamic:{}", name), a_hud), a_hud);
							}
						}, a_hud);
				}
			}, a_hud);
	}
}

void handle_arousal::log_player_arousal_state()
{
	auto* dataHandler = RE::TESDataHandler::GetSingleton();
	auto* player = RE::PlayerCharacter::GetSingleton();
	auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

	SKSE::log::info("[Arousal] STARTING AROUSAL LOGGING handler!!");

	if (!dataHandler || !player || !vm || !dataHandler->LookupModByName("SexLabAroused.esm") ||
		!GetModuleHandleW(L"SexlabArousedNG.dll")) {
		SKSE::log::warn("[Arousal] Could not start arousal logging handler due to missing data or mod!");
		SKSE::log::info("[Arousal] ABORTING AROUSAL LOGGING handler!!");
		return;
	}

	// Collect one HUD report per periodic check after the asynchronous contribution reads finish.
	auto hud = std::make_shared<ArousalHUDReport>();
	query("SloangNative", "GetArousal", RE::MakeFunctionArguments(static_cast<RE::Actor*>(player)),
		[hud](RE::BSScript::Variable a_result) {
			log_value("GetArousal")(a_result);
			if (a_result.IsFloat()) {
				std::scoped_lock lock(hud->mutex);
				hud->total = a_result.GetFloat();
			}
		}, hud);
	// SLO's getters only read state; no arousal values are changed.
	query("SloangNative", "GetVersion", RE::MakeFunctionArguments(), log_value("SLO version"));
	constexpr std::array getters{
		"GetArousalInt", "GetExposure", "GetDaysSinceLastOrgasm",
		"IsArousalLocked", "IsArousalBlocked", "IsActorExhibitionist", "IsActorNaked",
		"GetGenderPreference"
	};
	for (const auto* getter : getters) {
		// Pass Actor, rather than PlayerCharacter, to match the public Papyrus signatures.
		query("SloangNative", getter, RE::MakeFunctionArguments(static_cast<RE::Actor*>(player)), log_value(getter));
	}
	// Built-in static effect IDs from SLO's default, DD, SexLab and OStim plugins.
	constexpr std::array effects{
		"Naked", "Exhibitionist", "Orgasm", "Timed", "TimedCycle", "Legacy", "Sleep",
		"Devices", "Sex", "Fatigue", "Trauma", "OSex", "OFatigue", "OTrauma"
	};
	for (const auto* effect : effects) {
		log_static_effect(effect, hud);
	}
	log_dynamic_effects(player, hud);
	hud->finish_request();
}

void handle_arousal::reset_arousal_tracking()
{
	++arousalSession;  // Discard callbacks still pending from the previous game session.
	previousArousal.reset();
	nextArousalSample = 0;
	lastArousalSample = 0;
	lastContributionSample = 0;
	const auto initialCooldownTimestamp =
		std::chrono::steady_clock::now() - std::chrono::seconds(arousalThoughtCooldownSeconds);
	lastArousedThoughtTimestamp = initialCooldownTimestamp;
	lastDistractedThoughtTimestamp = initialCooldownTimestamp;
	lastNeedSexThoughtTimestamp = initialCooldownTimestamp;
	lastFocusedThoughtTimestamp = initialCooldownTimestamp;
	std::scoped_lock lock(contributorMutex);
	latestContributions.clear();
}

std::string handle_arousal::get_top_arousal_contributors(std::size_t a_count)
{
	const auto ranked = ranked_arousal_contributors();
	const auto count = std::min(a_count, ranked.size());
	std::string result;
	for (std::size_t i = 0; i < count; ++i) {
		if (i != 0) {
			result += i + 1 == count ? " and " : ", ";
		}
		result += contributor_name(ranked[i].first);
	}
	return result;
}

std::string handle_arousal::get_top_arousal_contribution_string(std::size_t a_count)
{
	const auto ranked = ranked_arousal_contributors();
	constexpr std::array prefixes{
		"The reasons for your arousal are first and foremost ",
		" and then to a lesser extent ",
		" and then to an even lesser extent "
	};
	const auto count = std::min({ a_count, ranked.size(), prefixes.size() });
	std::string result;
	for (std::size_t i = 0; i < count; ++i) {
		result += prefixes[i];
		result += contributor_reason(ranked[i].first);
	}
	if (!result.empty()) {
		result += '.';
	}
	return result;
}


std::string handle_arousal::legacy_get_top_arousal_contributors(std::size_t a_count)
{
	std::vector<std::pair<std::string, float>> ranked;
	{
		std::scoped_lock lock(contributorMutex);
		for (const auto& [name, value] : latestContributions) {
			if (std::isfinite(value) && value > 0.0f) {
				ranked.emplace_back(name, value);
			}
		}
	}
	std::sort(ranked.begin(), ranked.end(), [](const auto& a_left, const auto& a_right) {
		return a_left.second != a_right.second ? a_left.second > a_right.second : a_left.first < a_right.first;
	});
	const auto count = std::min(a_count, ranked.size());
	std::string result;
	for (std::size_t i = 0; i < count; ++i) {
		if (i != 0) {
			result += i + 1 == count ? " and " : ", ";
		}
		result += contributor_name(ranked[i].first);
	}
	return result;
}
