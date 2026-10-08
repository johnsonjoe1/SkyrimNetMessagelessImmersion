#include "handle_config_ini_file.h"
#include <Windows.h>
#include <filesystem>
#include <cerrno>
#include <cwchar>
#include <cwctype>
#include <limits>

namespace
{
    SNMI::Settings settings;

    std::filesystem::path GetPluginDirectory()
    {
        HMODULE module = nullptr;

        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetPluginDirectory),
            &module);

        wchar_t path[MAX_PATH];

        GetModuleFileNameW(
            module,
            path,
            MAX_PATH);

        return std::filesystem::path(path).parent_path();
    }
}

namespace SNMI
{
    Settings& GetSettings()
    {
        return settings;
    }

    void LoadSettings()
    {
        const auto configPath = GetPluginDirectory() / L"SkyrimNetMessagelessImmersion.ini";
        SKSE::log::info("Trying to read config file now.  If you use MO2, the base tree position of the expected file location will appear to be in the real game folder, not your mod directory.  That is normal.  The rest of the path is what counts.  For debugging purposes we still print the internal expected location:  Expected location = {}", configPath.string());

        settings.debugLogging = GetPrivateProfileIntW(L"General", L"DebugLogging", 0, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.debug.  New variable value = {}", settings.debugLogging);
        if (!settings.debugLogging) {
            SKSE::log::info("DebugLogging has been disabled by you via a setting in the SkyrimNetMessagelessImmersion.ini file. From here onwards, only warnings and errors will be logged.");
        }
        spdlog::set_level(settings.debugLogging ? spdlog::level::trace : spdlog::level::warn);

		settings.enablePlayerDirtThoughts = GetPrivateProfileIntW(L"Thoughts", L"EnablePlayerDirtThoughts", 1, configPath.c_str()) != 0;
		SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enablePlayerDirtThoughts.  New variable value = {}", settings.enablePlayerDirtThoughts);

        settings.enableMilkThoughts = GetPrivateProfileIntW(L"Thoughts", L"EnableMilkThoughts", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableMilkThoughts.  New variable value = {}", settings.enableMilkThoughts);

        settings.enableANDNudityThoughts = GetPrivateProfileIntW(L"Thoughts", L"EnableANDNudityThoughts", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableANDNudityThoughts.  New variable value = {}", settings.enableANDNudityThoughts);

        settings.enableSLSFthoughts = GetPrivateProfileIntW(L"Thoughts", L"enableSLSFthoughts", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableSLSFthoughts.  New variable value = {}", settings.enableSLSFthoughts);

        settings.enableConstantWhining = GetPrivateProfileIntW(L"Thoughts", L"EnableConstantWhining", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableConstantWhining.  New variable value = {}", settings.enableConstantWhining);

        settings.enableLicensesPlayerOppressionThoughts = GetPrivateProfileIntW(L"Thoughts", L"EnableLicensesPlayerOppressionThoughts", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableLicensesPlayerOppressionThoughts.  New variable value = {}", settings.enableLicensesPlayerOppressionThoughts);

        settings.enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts = GetPrivateProfileIntW(L"Thoughts", L"EnableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts", 1, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts.  New variable value = {}", settings.enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts);

        settings.updateInterval = GetPrivateProfileIntW(L"Timing", L"UpdateInterval", 5, configPath.c_str());
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.updateInterval.  New variable value = {}", settings.updateInterval);

        settings.silenceRequiredBeforeSpontaneousStatusWhining = 90;
        wchar_t silenceValue[64]{};
        const auto silenceLength = GetPrivateProfileStringW(L"Timing", L"SilenceRequiredBeforeSpontaneousStatusWhining", L"", silenceValue, 64, configPath.c_str());
        if (silenceLength > 0 && silenceLength < 63) {
            wchar_t* end = nullptr;
            errno = 0;
            const auto seconds = std::wcstol(silenceValue, &end, 10);
            const bool parsed = end != silenceValue;
            while (std::iswspace(*end)) {
                ++end;
            }
            if (parsed && *end == L'\0' && errno != ERANGE && seconds >= 0 && seconds <= std::numeric_limits<int>::max()) {
                settings.silenceRequiredBeforeSpontaneousStatusWhining = static_cast<int>(seconds);
            }
        }
        SKSE::log::info("Finished reading (or defaulting to fallback for) config variable settings.silenceRequiredBeforeSpontaneousStatusWhining.  New variable value = {}", settings.silenceRequiredBeforeSpontaneousStatusWhining);
    }
}
