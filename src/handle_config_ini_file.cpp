#include "handle_config_ini_file.h"
#include <Windows.h>
#include <filesystem>

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

        SKSE::log::info("Trying to read config file now.  Expected location = {}", configPath.string());

        settings.debugLogging = GetPrivateProfileIntW(L"General", L"DebugLogging", 0, configPath.c_str()) != 0;
        SKSE::log::info("Finished reading (or defaulting to fallback for)config variable settings.debug.  New variable value = {}", settings.debugLogging);

        settings.updateInterval = GetPrivateProfileIntW(L"Timing", L"UpdateInterval", 33, configPath.c_str());
        SKSE::log::info("Finished reading (or defaulting to fallback for)config variable settings.updateInterval.  New variable value = {}", settings.updateInterval);
    }
}


