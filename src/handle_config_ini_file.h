#pragma once

namespace SNMI
{
    struct Settings
    {
        bool debugLogging{ false };
		bool enablePlayerDirtThoughts{ false };
		int updateInterval{ 33 };
    };

    Settings& GetSettings();

    void LoadSettings();
}
