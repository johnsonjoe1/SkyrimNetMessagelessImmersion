
# Project description
Ever had to open the Skyrim Magic Effect screen to see what is going on with your character?
Ever overlooked a notification message that flashed by but was important for gameplay?

This project takes SkyrimNet and adds some additional triggers to trigger player-thoughts that should resolve that.
The idea is to make Skyrim basically messageless, because the player character notices all changes in-game and reflects on them.
So that in an ideal scenario, all widgets and all fonts could be removed from the screen for better immersion.

The idea is, not to change anything about what you do in game.  The only change should be more immersion and more responsiveness concerning SkyrimNet player-thoughts, which in turn should be enough to make all other prompts also aware of relevant proceedings.
So everything should play the same, just with an potentially messageless and widgetless interface if you want it that way.

# Compatibility

The current build uses CommonLibSSE-NG 9.1.0 and has been tested successfully on the Steam version of Skyrim 1.6.1170 with SKSE 2.2.6. It is built as an Address Library-compatible, version-independent SKSE plugin and is expected to support Skyrim 1.7.104 when used with SKSE 2.3.1 and Address Library v13, but that runtime has not yet been tested. GOG and Skyrim VR are also currently untested.

NOTE:  All testing is done for Sexlab P+.  This one is not a hard requirement or anything, but it's API and responses might be slightly different from the older Sexlab 1.66b, so maybe some deterioration of quality or some missing features might happen on systems still running the old 1.66b version.  Sorry for any inconvenience this may cause.

# Installation and requirements

SkyrimNet is the only direct mod requirement. Its own requirements must also be installed and working:

* [Skyrim Script Extender (SKSE)](https://skse.silverlock.org/)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [powerofthree's Papyrus Extender](https://www.nexusmods.com/skyrimspecialedition/mods/22854)
* [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048)
* [Latest Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)
* [Native EditorID Fix](https://www.nexusmods.com/skyrimspecialedition/mods/85260)
* [Prisma UI](https://www.nexusmods.com/skyrimspecialedition/mods/148718)

The other mods mentioned below are optional. SNMI does not modify them; it listens for their events or reads their state when they are present.

Version 0.6.12 and later supports both the legacy SkyrimNet trigger layout used through beta25 RC6 and the external content-plugin layout introduced in beta25 RC7. Both layouts are included in the same archive; no installer choice or manual migration is required.

# Recent changes

* Migrated from the obsolete CommonLib vcpkg port to upstream CommonLibSSE-NG 9.1.0, pinned as a Git submodule for reproducible builds and future Skyrim 1.7.x support.
* Thoughts that occur during player dialogue are queued and played afterwards. Individual call sites may instead discard a thought or process it immediately, and the queue is cleared when a game is loaded or started.
* Recent player thoughts from the preceding five minutes are included as context to improve continuity between successive SkyrimNet responses.
* Added HUD-notification handling for events that do not expose a more useful native event, including gag restrictions, surrender, missing pickaxes, fishing failures, insufficient gold, and supported arousal notifications.
* Added player worn-equipment tracking, including immediate AND transparency/flashing observations and better identification of relevant worn items.
* Expanded YPS handling for hair, cosmetics, stockings, nails, care products, fashion addiction, foot conditions, and heel-training status. YPS thoughts are suppressed during SexLab scenes.
* Expanded SLAC handling for player-involved scene starts, stage changes, scene endings, and approaching creatures, with filtering and cooldowns to reduce unrelated or repeated thoughts.
* Expanded active-magic-effect handling, including additional restraint, hood, bimbo-corruption, and cum-effect cases.

# How does it work?  

It's a simple project.  It just hooks into magic effect changes and mod broadcasts from other mods (or the base game), and if something
relevant shows up, we trigger a player-thought response.  
Any other prompts from SkyrimNet are aware of the player-thoughts.  So SkyrimNet might pick up on the additional
information as well, without extra code.  Minimal invasiveness and a gentle conflict-free and overwrite-free presence in your load order is the goal.


# Supported mods

Support is intentionally selective rather than complete. SNMI does not modify these mods; it observes their exposed events, state, equipment, or magic effects and turns selected changes into SkyrimNet player thoughts.

* [YPS Fashion](https://www.loverslab.com/files/file/2583-immersive-hair-growth-and-styling-yps-devious-immersive-fashion-2025-06-08/): relays YPS's own thoughts, and comments on hair length and dye changes, makeup, nail polish, stockings, piercings, fashion addiction, foot conditions, heel training, and YPS movement penalties. YPS thoughts are suppressed during SexLab scenes. To avoid duplicate voice lines, set YPS's built-in thought volume to zero and reduce its thought frequency.
* [Milk Mod Economy (MME)](https://www.loverslab.com/files/file/6103-milk-mod-economy-se/): reports milk fullness changes, Lactacid changes, milk-pump use, and Maid-level progress.
* [Advanced Nudity Detection (AND)](https://www.nexusmods.com/skyrimspecialedition/mods/165289): reports changes to nudity, partial nudity, modesty, and flashing-risk states; worn equipment is also scanned to identify relevant transparent and flash-risk items.
* [SexLab Sexual Fame Reloaded (SLSF)](https://www.loverslab.com/files/file/35874-sexlab-sexual-fame-reloaded/): monitors significant increases and decreases in the tracked fame categories and produces a combined response when appropriate.
* [Battle Fuck](https://www.loverslab.com/files/file/18241-battle-fuck/): comments on player struggle events.
* [SexLab Body Search](https://www.loverslab.com/files/file/9318-sexlab-body-search/): comments when a player body-search scene begins.
* Devious Devices, Unforgiving Devices, and Unforgiving Skyrim: reacts to supported device equip/remove events, sentient-device dialogue, device struggles and falls, and blocked inventory, magic, or quick-access hotkeys. Selected restraint, gag, drool, tear, and similar magic effects are also recognized.
* [SexLab Aroused Creatures (SLAC)](https://www.loverslab.com/files/file/6022-sexlab-aroused-creatures-se-2026-02-20/): handles player-involved scene starts, stage changes, scene endings, and approaching-creature events, with player filtering and cooldowns to limit repeated thoughts.
* [SL Survival](https://www.loverslab.com/blogs/entry/20175-sl-survival/): comments on becoming barefoot or shod again, and on Bikini Curse and related out-of-breath effects.
* [Spank That Ass (STA)](https://www.loverslab.com/blogs/entry/20176-spank-that-ass/): reacts to the run-up-and-spank broadcast.
* [The Ancient Profession](https://www.loverslab.com/files/file/11556-the-ancient-profession-2024-06-24/): comments on the generic freelance-work scene.
* [SexLab Jail Rape](https://www.loverslab.com/files/file/9111-sexlab-jail-rape/): reacts to player scene starts, position changes, and later stages, with a short stage-thought cooldown.
* [Devious Followers](https://www.loverslab.com/files/file/44435-devious-followers-203-2025/): comments when the player's resistance is reduced.
* [Licenses - Player Oppression](https://www.nexusmods.com/skyrimspecialedition/mods/110418?tab=description): reacts to guard bounty pursuit and resolution, license or permit acquisition, and the application or removal of the magic-suppression effect. License thoughts can be disabled in the SNMI INI.
* iNeed: monitors hunger, thirst, and fatigue level changes; reacts to refilling waterskins and entering or leaving water.
* Bathing in Skyrim: comments when the player becomes very dirty, is partially cleaned by swimming, or becomes fully clean after bathing. These thoughts can be disabled in the SNMI INI.
* Creature Summoner: comments when the player summons a supported creature.
* Some basic support for the SE Version of the Apropos 2 mod (from the LL forum:  https://www.loverslab.com/topic/136768-apropos-2-for-sse/),
* SexLab P+: tracks player-involved scene boundaries to avoid out-of-place clothing and fashion thoughts, and recognizes supported cum-effect application and removal.
* Vanilla Skyrim: comments on selected diseases and cures (currently including Stomach Rot), and on using supported furniture such as blacksmith forges, tanning racks, and grindstones. Furniture thoughts use type-specific cooldowns.

Note, that as said, there are only minimal gentle changes, nothing big or invasive.  And none of them are required in any form.  I add more stuff, as I play along and find something is missing and more response from SkyrimNet would be appropriate here and there.  Not a very systematic or completionist approach, but rather picking up stuff here and there.



# Usage/Configuration

The mod includes a limited configuration file at `SKSE/Plugins/SkyrimNetMessagelessImmersion.ini`.
The following switches in its `[Thoughts]` section are live and accept `1` or `0`:

* `EnableMilkThoughts`
* `EnableLicensesPlayerOppressionThoughts`
* `EnablePlayerDirtThoughts`
* `EnableANDNudityThoughts`
* `EnableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts`

The `EnablePlugin`, `DebugLogging`, and `EnableAproposThoughts` entries are placeholders and currently have no effect. `UpdateInterval` controls how often periodic status checks run, in whole seconds. Most other behavior remains hard-coded, and there is no MCM.
If you want, you can disable e.g. the background thought channel. On SkyrimNet beta25 RC7 or later, disable or edit it through the `johnsonjoe1.snmi` external plugin in SkyrimNet's dashboard. On RC6 or earlier, delete or edit `SKSE/Plugins/SkyrimNet/config/triggers/SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought.yaml`.

# Contributing guidelines
You really want to help?  Great!  There are no rules.  Do what you want and can.  

# License
As free as possible. I guess nobody want this code anyway.  And that is referring only to the parts I added to the project, not the template/libraries that I started with.  See their license in the respective places.

# Credits
I took the template from https://github.com/Monitor221hz/CommonLibSSE-NG-Template-Plugin
So all credit goes to whoever contributes there and is credited there.

# Technical information for mod developers on how to compile this stuff

CommonLibSSE-NG 9.1.0 (commit `a898f469851c464d05137bb74b069dd234897643`) is pinned as a Git submodule. After cloning or updating this repository, initialize it and its nested OpenVR submodule before configuring the project:

```powershell
git submodule update --init --recursive
cmake --preset build-release-msvc
cmake --build --preset release-msvc
```

The build requires Visual Studio 2022 with Desktop development with C++, CMake, Ninja, and a `VCPKG_ROOT` environment variable pointing to vcpkg. The vcpkg manifest installs the remaining build dependencies from the baseline pinned in `vcpkg-configuration.json`.

Thanks!
