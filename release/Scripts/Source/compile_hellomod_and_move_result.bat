
@ECHO ***** doesn't work from another directory than the working folder of the source script.
@ECHO ***** So, since the batch file is assumed to be here, we don't hard code any path change here.

@ECHO ***** run the compliler
@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" HM_HelloModNewQuest.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SkyrimNet-beta15.5\Source\Scripts" -flags=TESV_Papyrus_Flags.flg

@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" HM_PlayerReferenceAliasScript.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SkyrimNet-beta15.5\Source\Scripts" -flags=TESV_Papyrus_Flags.flg

@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" ypsThoughts.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SkyrimNet-beta15.5\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts" -flags=TESV_Papyrus_Flags.flg

@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" CC_SexEventsScript.psc -i="C:\Modding\SKSE_on_Pandora\mods\SexLab Framework PPLUS - V2.16.0 - 1.6.1170\Source\Scripts;C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SkyrimNet-beta15.5\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\Bimbos of Skyrim 1.9.0.7\scripts\source;C:\Modding\SKSE_on_Pandora\mods\SlaveTatsSE-1.3.9\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\SexLabFrameworkAE_v\scripts\Source;C:\Modding\SKSE_on_Pandora\mods\powerofthree's Papyrus Extender\Source\scripts;C:\Modding\SKSE_on_Pandora\mods\JContainers SE\scripts\source;C:\Modding\SKSE_on_Pandora\mods\MCM Helper\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\RaceMenu\scripts\source;C:\Modding\SKSE_on_Pandora\mods\Mfg Fix NG\source\scripts;C:\Modding\SKSE_on_Pandora\mods\SexLab Inflation Framework SE 1.2.2 beta Installer (1)\Scripts\Source;C:\Modding\AlternateSEInstance\mods\FNIS non-XXL Fores New Idles in Skyrim SE - FNIS SE\source\scripts;C:\Modding\AlternateSEInstance\mods\FNIS Creature Pack SE 7.6\source\scripts;C:\Modding\SKSE_on_Pandora\mods\SkyUI\scripts\Headers\surely_SDK_51;C:\Modding\SKSE_on_Pandora\mods\SkyUI\scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SexLab-Separate-Orgasm 2023-01-16 (1)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SLAX - 29 - 2019-10-21-0 (wip)\scripts\Source;C:\Modding\SKSE_on_Pandora\mods\OSL Aroused FOMOD 2.8.2\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\Sexlab Disparity 14.4 - SSE\scripts\Sources;C:\Modding\SKSE_on_Pandora\mods\SkyrimLovense-0.1.0\Source\Scripts" -flags=TESV_Papyrus_Flags.flg

ECHO  Trying to complie the SNMI_Bridge.psc file now:


@REM First good approach:
@REM  "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Bridge.psc -i="C:\Modding\SKSE_on_Pandora\mods\Milk-Mod-Economy-Scripts-20220522\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SexLab Framework PPLUS - V2.16.0 - 1.6.1170\Source\Scripts;C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\Bimbos of Skyrim 1.9.0.7\scripts\source;C:\Modding\SKSE_on_Pandora\mods\SlaveTatsSE-1.3.9\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\SexLabFrameworkAE_v\scripts\Source;C:\Modding\SKSE_on_Pandora\mods\powerofthree's Papyrus Extender\Source\scripts;C:\Modding\SKSE_on_Pandora\mods\JContainers SE\scripts\source;C:\Modding\SKSE_on_Pandora\mods\MCM Helper\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\Mfg Fix NG\source\scripts;C:\Modding\SKSE_on_Pandora\mods\SexLab Inflation Framework SE 1.2.2 beta Installer (1)\Scripts\Source;C:\Modding\AlternateSEInstance\mods\FNIS non-XXL Fores New Idles in Skyrim SE - FNIS SE\source\scripts;C:\Modding\AlternateSEInstance\mods\FNIS Creature Pack SE 7.6\source\scripts;C:\Modding\SKSE_on_Pandora\mods\SexLab-Separate-Orgasm 2023-01-16 (1)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\SLAX - 29 - 2019-10-21-0 (wip)\scripts\Source;C:\Modding\SKSE_on_Pandora\mods\OSL Aroused FOMOD 2.8.2\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\Sexlab Disparity 14.4 - SSE\scripts\Sources;C:\Modding\SKSE_on_Pandora\mods\SkyrimLovense-0.1.0\Source\Scripts" -flags=TESV_Papyrus_Flags.flg
@REM Minimal approach:
@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Bridge.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\Milk-Mod-Economy-Scripts-20220522\Scripts\Source" -flags=TESV_Papyrus_Flags.flg
@REM  NOW WITH MY MME_Storage.psc STUB:

@REM  "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Bridge.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source" -flags=TESV_Papyrus_Flags.flg

"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Native.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source" -flags=TESV_Papyrus_Flags.flg

@REM DISABLE BECAUSE OF DEPENDENCY.  "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Papyrus_Bridge_Script.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\Apropos\Scripts\Source" -flags=TESV_Papyrus_Flags.flg
"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Papyrus_Bridge_Script.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source" -flags=TESV_Papyrus_Flags.flg

"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" ypsThoughts.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts" -flags=TESV_Papyrus_Flags.flg

@REM "C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" _STA_SpankUtil.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\Spank That Ass/scripts/source" -flags=TESV_Papyrus_Flags.flg


@REM C:\Modding\SKSE_on_Pandora\mods\SLAX - 29 - 2019-10-21-0 (wip)\scripts\Source;
@REM C:\Modding\SKSE_on_Pandora\mods\SLAX - 29 - 2019-10-21-0 (wip)\scripts\Source
@REM C:\Modding\SKSE_on_Pandora\mods\OSL Aroused FOMOD 2.8.2\Scripts\Source
@REM C:\Modding\SKSE_on_Pandora\mods\SkyUI\scripts\Headers\surely_SDK_51
@REM C:\Modding\SKSE_on_Pandora\mods\SkyUI\scripts\Headers;
@REM C:\Modding\SKSE_on_Pandora\mods\SLO Aroused NG\Source\Scripts;

@ECHO ***** copy the output to the upper directory
COPY *.pex ..

@ECHO ***** copy the output to the LIVE MOD DIRECTORY, which would be C:\Modding\SKSE_on_Pandora\mods\SkyrimNetMessagelessImmersion\Scripts\Source

@REM COPY SNMI_*.psc C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source\
@REM COPY SNMI_*.pex C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\
@REM COPY ypsThoughts.psc C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source\
@REM COPY ypsThoughts.pex C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\

@ECHO ***** We compile everything in "release" and therefore we copy it to the live mod dir

COPY SNMI_*.psc C:\Modding\SKSE_on_Pandora\mods\SkyrimNetMessagelessImmersion\Scripts\Source\
COPY SNMI_*.pex C:\Modding\SKSE_on_Pandora\mods\SkyrimNetMessagelessImmersion\Scripts\
COPY ypsThoughts.psc C:\Modding\SKSE_on_Pandora\mods\SkyrimNetMessagelessImmersion\Scripts\Source\
COPY ypsThoughts.pex C:\Modding\SKSE_on_Pandora\mods\SkyrimNetMessagelessImmersion\Scripts\




@REM  COPY ypsThoughts.pex         "C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\"
@REM  COPY CC_SexEventsScript.pex  "C:\Modding\SKSE_on_Pandora\mods\Bimbos of Skyrim 1.9.0.7\scripts\"
