#include "handle_arousal.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <Windows.h>
#include <array>
#include <format>
#include <functional>
#include <string>
#include <utility>

namespace
{
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
		RE::BSScript::IFunctionArguments* a_args, ResultHandler a_handler)
	{
		auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!vm) {
			delete a_args;
			return;
		}
		RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback{ new ArousalResult(std::move(a_handler)) };
		if (!vm->DispatchStaticCall(a_script, a_getter, a_args, callback)) {
			SKSE::log::warn("[Arousal] Could not dispatch {}.{}", a_script, a_getter);
		}
	}

	void log_static_effect(const char* a_name)
	{
		// Resolve IDs each time: effect registration order and enabled plugins can change.
		query("slaInternalModules", "GetStaticEffectId", RE::MakeFunctionArguments(RE::BSFixedString(a_name)),
			[a_name](RE::BSScript::Variable a_result) {
				if (!a_result.IsInt()) {
					SKSE::log::warn("[Arousal] Could not resolve static effect {}", a_name);
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
					log_value(std::format("static effect {} active", a_name)));
				query("slaInternalModules", "GetStaticEffectValue", RE::MakeFunctionArguments(static_cast<RE::Actor*>(player), std::int32_t(id)),
					log_value(std::format("static effect {} contribution", a_name)));
			});
	}

	void log_dynamic_effects(RE::Actor* a_player)
	{
		query("slaInternalModules", "GetDynamicEffectCount", RE::MakeFunctionArguments(static_cast<RE::Actor*>(a_player)),
			[](RE::BSScript::Variable a_result) {
				if (!a_result.IsInt() || a_result.GetSInt() < 0) {
					SKSE::log::warn("[Arousal] Could not read dynamic effect count");
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
						[](RE::BSScript::Variable a_name) {
							if (!a_name.IsString() || a_name.GetString().empty()) {
								return;  // Effect may have disappeared since the count was read.
							}
							const std::string name(a_name.GetString());
							auto* actor = static_cast<RE::Actor*>(RE::PlayerCharacter::GetSingleton());
							if (actor) {
								// Read by name so removal/reordering cannot pair a name with another effect's value.
								query("SloangNative", "GetDynamicEffectValue",
								RE::MakeFunctionArguments(static_cast<RE::Actor*>(actor), RE::BSFixedString(name)),
									log_value(std::format("dynamic effect {} contribution", name)));
							}
						});
				}
			});
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

	// SLO's getters only read state. Results arrive asynchronously; no thoughts or state writes.
	query("SloangNative", "GetVersion", RE::MakeFunctionArguments(), log_value("SLO version"));
	constexpr std::array getters{
		"GetArousal", "GetArousalInt", "GetExposure", "GetDaysSinceLastOrgasm",
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
		log_static_effect(effect);
	}
	log_dynamic_effects(player);
}
