#pragma once

namespace SNMI
{
    struct Settings
    {
        bool debugLogging{ true };
		bool enablePlayerDirtThoughts{ true };
		bool enableMilkThoughts{ true };
        bool enableANDNudityThoughts{ true };
        bool enableSLSFthoughts{ true };
        int SLSFthoughtThreshold{ 5 };
        int SLSFthoughtCooldown{ 300 };
        bool enableConstantWhining{ true };
        bool enableLicensesPlayerOppressionThoughts{ true };
        bool enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts{ true };
		int updateInterval{ 5 };
		int silenceRequiredBeforeSpontaneousStatusWhining{ 90 };
    };

    Settings& GetSettings();

    void LoadSettings();
}
