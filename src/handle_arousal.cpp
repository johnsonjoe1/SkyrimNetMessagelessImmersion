#include "handle_arousal.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <array>
#include <format>
#include <string>

namespace
{
	class LogArousalResult final : public RE::BSScript::IStackCallbackFunctor
	{
	public:
		explicit LogArousalResult(const char* a_getter) : getter(a_getter) {}

		void operator()(RE::BSScript::Variable a_result) override
		{
			if (a_result.IsFloat()) {
				SKSE::log::info("[Arousal] Player {} = {}", getter, a_result.GetFloat());
			} else if (a_result.IsBool()) {
				SKSE::log::info("[Arousal] Player {} = {}", getter, a_result.GetBool());
			} else if (a_result.IsArray()) {
				const auto values = a_result.GetArray();
				if (!values) {
					SKSE::log::warn("[Arousal] Player {} returned no array", getter);
					return;
				}
				constexpr std::array names{
					"Nude", "Topless", "Bottomless", "ShowingChest",
					"ShowingAss", "ShowingGenitals", "ShowingBra", "ShowingUnderwear"
				};
				std::string contributions;
				for (std::uint32_t i = 0; i < values->size(); ++i) {
					if (!(*values)[i].IsFloat()) {
						SKSE::log::warn("[Arousal] Player {} returned a non-float array element", getter);
						return;
					}
					contributions += std::format("{}{}={}", i == 0 ? "" : ", ",
						i < names.size() ? names[i] : "Unknown", (*values)[i].GetFloat());
				}
				SKSE::log::info("[Arousal] Player {} = [{}]", getter, contributions);
			} else {
				SKSE::log::warn("[Arousal] Player {} returned an unexpected value type", getter);
			}
		}

		void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}

	private:
		const char* getter;
	};

	void log_getter(RE::BSScript::Internal::VirtualMachine* a_vm, const char* a_script,
		const char* a_getter, RE::BSScript::IFunctionArguments* a_args)
	{
		RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback{ new LogArousalResult(a_getter) };
		if (!a_vm->DispatchStaticCall(a_script, a_getter, a_args, callback)) {
			SKSE::log::warn("[Arousal] Could not dispatch {}.{}", a_script, a_getter);
		}
	}
}

void handle_arousal::log_player_arousal_state()
{
	auto* dataHandler = RE::TESDataHandler::GetSingleton();
	auto* player = RE::PlayerCharacter::GetSingleton();
	auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
	if (!dataHandler || !player || !vm || !dataHandler->LookupModByName("OSLAroused.esp")) {
		return;
	}

	// Results arrive asynchronously through the VM. Only log them; do not emit thoughts or set OSL state.
	log_getter(vm, "OSLArousedNativeConfig", "IsInOSLMode", RE::MakeFunctionArguments());
	log_getter(vm, "OSLArousedNativeConfig", "IsANDIntegrationEnabled", RE::MakeFunctionArguments());
	constexpr std::array getters{
		"GetArousalNoSideEffects",  // GetArousal would update OSL's internal state.
		"GetArousalBaseline", "GetLibido", "GetExposure", "GetActorTimeRate",
		"GetArousalMultiplier", "GetDaysSinceLastOrgasm",
		"IsActorArousalLocked", "IsActorExhibitionist", "IsActorNaked", "IsNaked",
		"IsViewingNaked", "IsInScene", "IsViewingScene", "IsWearingEroticArmor",
		"WornDeviceBaselineGain", "GetANDNudityScore", "GetANDFactionContributions"
	};
	for (const auto* getter : getters) {
		// Pass Actor, rather than PlayerCharacter, to match the public Papyrus signatures.
		log_getter(vm, "OSLArousedNative", getter, RE::MakeFunctionArguments(static_cast<RE::Actor*>(player)));
	}
}
