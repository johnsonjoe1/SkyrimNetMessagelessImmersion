#pragma once

namespace SNMI
{
    struct Settings
    {
        bool debugLogging{ false };
		bool enablePlayerDirtThoughts{ false };
		bool enableMilkThoughts{ false };
        bool enableANDNudityThoughts{ false };
		int updateInterval{ 33 };
    };

    Settings& GetSettings();

    void LoadSettings();
}
