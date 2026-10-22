
This is a release of SkyrimNet Messageless Immersion (SNMI).
SkyrimNet and its requirements must be installed and working. See the project README for the requirements and supported integrations.

Both the legacy SkyrimNet trigger layout used through beta25 RC6 and the external content-plugin layout introduced in beta25 RC7 are included. SkyrimNet uses the appropriate layout; no installer choice or manual migration is required.

On RC7 or later, SNMI's triggers appear in the SkyrimNet dashboard as the external plugin "SkyrimNet Messageless Immersion" (johnsonjoe1.snmi), where individual triggers can be disabled or edited. On RC6 or earlier, the legacy trigger files remain under SKSE/Plugins/SkyrimNet/config/triggers/.

The current build uses CommonLibSSE-NG 9.1.0. Tested configuration: Steam Skyrim 1.6.1170 with SKSE 2.2.6. Skyrim 1.7.104, GOG, and Skyrim VR remain untested; the CommonLib upgrade alone does not confirm compatibility with those runtimes. SexLab integration testing uses SexLab P+; behavior with older SexLab versions may differ.

Configuration is in SKSE/Plugins/SkyrimNetMessagelessImmersion.ini. Restart the game after editing it. DebugLogging works for native logging and the bridge's guarded Papyrus traces, and is enabled in the supplied INI. EnablePlugin and EnableAproposThoughts are placeholders with no effect. The project README describes all working settings and current limitations, including HUD/debug behavior that remains active independently of logging.

Source, requirements, configuration reference, and release notes:
https://github.com/johnsonjoe1/SkyrimNetMessagelessImmersion

