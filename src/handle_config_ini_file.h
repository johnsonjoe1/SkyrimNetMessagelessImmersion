#pragma once

namespace SNMI
{
    struct Settings
    {
        bool debugLogging{ false };
		bool enablePlayerDirtThoughts{ false };
		bool enableMilkThoughts{ false };
        bool enableANDNudityThoughts{ false };
        bool enableSLSFthoughts{ true };
        int SLSFthoughtThreshold{ 5 };
        bool enableConstantWhining{ true };
        bool enableLicensesPlayerOppressionThoughts{ false };
        bool enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts{ false };
		int updateInterval{ 5 };
		int silenceRequiredBeforeSpontaneousStatusWhining{ 90 };
    };

    Settings& GetSettings();

    void LoadSettings();
}
