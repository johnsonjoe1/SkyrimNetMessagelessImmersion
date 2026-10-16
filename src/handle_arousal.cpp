#include "handle_arousal.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "RE/S/SendHUDMessage.h"
#include <Windows.h>
#include <array>
#include <format>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

namespace
{
	struct ArousalHUDReport
	{
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
				tasks->AddTask([message = std::move(message)] {
					RE::SendHUDMessage::ShowHUDMessage(message.c_str());
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

	ResultHandler log_contribution(std::string a_name, bool a_active, HUDReport a_hud)
	{
		return [name = std::move(a_name), a_active, a_hud](RE::BSScript::Variable a_result) {
			log_value(name + " contribution")(a_result);
			std::scoped_lock lock(a_hud->mutex);
			if (a_result.IsFloat()) {
				if (a_active) {
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
								log_contribution(std::format("Static:{}", a_name), a_active.GetBool(), a_hud), a_hud);
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
									log_contribution(std::format("Dynamic:{}", name), true, a_hud), a_hud);
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
	// SLO's getters only read state. No thoughts or state writes.
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
