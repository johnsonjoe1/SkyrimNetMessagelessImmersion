
@ECHO ***** doesn't work from another directory than the working folder of the source script.
@ECHO ***** So, since the batch file is assumed to be here, we don't hard code any path change here.

@ECHO ***** run the compliler

ECHO  Trying to compile the SNMI_Native.psc file now:

"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Native.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source" -flags=TESV_Papyrus_Flags.flg

ECHO  Trying to compile the SNMI_Papyrus_Bridge_Script.psc file now:
@REM  NOW WITH MY MME_Storage.psc STUB:

"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" SNMI_Papyrus_Bridge_Script.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\SlaveTatsSE-1.3.9\Source\Scripts;C:\Modding\SKSE_on_Pandora\mods\JContainers SE\scripts\source;C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\Source" -flags=TESV_Papyrus_Flags.flg

ECHO  Trying to compile the ypsThoughts.psc file now:

"C:\GAMES\steamapps\common\Skyrim Special Edition\Papyrus Compiler\PapyrusCompiler.exe" ypsThoughts.psc -i="C:\GAMES\steamapps\common\Skyrim Special Edition\Data\Scripts\Source;C:\Modding\SKSE_on_Pandora\dev\SkyrimNetMessagelessImmersion\release\Scripts\Source;C:\Modding\SKSE_on_Pandora\mods\PapyrusUtil SE - Modders Scripting Utility Functions\Source\Scripts" -flags=TESV_Papyrus_Flags.flg





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
del *.pex

@REM  COPY ypsThoughts.pex         "C:\Modding\SKSE_on_Pandora\mods\yps-ImmersiveFashion (6.9.2)\Scripts\"
@REM  COPY CC_SexEventsScript.pex  "C:\Modding\SKSE_on_Pandora\mods\Bimbos of Skyrim 1.9.0.7\scripts\"
