#pragma once

namespace SNMI
{
    struct Settings
    {
        bool debugLogging{ false };
		bool enablePlayerDirtThoughts{ false };
		bool enableMilkThoughts{ false };
        bool enableANDNudityThoughts{ false };
        bool enableLicensesPlayerOppressionThoughts{ false };
        bool enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts{ false };
		int updateInterval{ 5 };
    };

    Settings& GetSettings();

    void LoadSettings();
}
