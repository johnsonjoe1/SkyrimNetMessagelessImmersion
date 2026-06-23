
# Project description
Ever had to open the Skyrim Magic Effect screen to see what is going on with your character?
Ever overlooked a notification message that flashed by but was important for gameplay?

This project takes SkyrimNet and adds some additional triggers to trigger player-thoughts that should resolve that.
The idea is to make Skyrim basically messageless, because the player character notices all changes in-game and reflects on them.
So that in an ideal scenario, all widgets and all fonts could be removed from the screen for better immersion.

Of course, this is just a noob project, with so far no real experience with Skyrim modding.
At present, there is only a SKSE plugin and some trigger-yaml files for SkyrimNet and since recently also the first papyrus bridge.

The idea is, not to change anything, but just make the interface potentially messageless and widgetless.
Nothing is overwritten concerning other mods and no require and no other mods or proceedinds are changed.  That is one of the ideas here:  

Make especially those messages that just scroll by and are easily overlooked into a player-thought (or later maybe some other) reaction,
e.g. when getting infected with a disease, and it's visible in the active effect page, it should also trigger a response thought in-game.
When the disease gets worse, it should trigger another reaction.  
If an items has some magic effects attached to it, that could trigger more responses by the player-character in game.

# Installation instructions
There are now some release packages.  

Requirements/Versions:  The Skyrim version I'm using is Skyrim AE latest version 1.6.1170, Skyrim SKSE the latest version as of 2026, and SkyrimNet (21.2+ version or any later version that still has trigger yaml-files, but they don't change that much).
Any other mods, that would benefit from the plugin are not required, as it doesn't change them, it just listens to them and reads out stuff from them. 


# Features list
At present, there are some hooks into the activation of furniture, like Tanning Rack and Blacksmith forge, that should trigger player thought.
Then there are some hooks into the active magic effect process.  Whatever seems reasonable and can easily be interpreted from what is there in the event data will trigger a response.
Also, mod event broadcasts from other mods will be listened to, and, where applicable, trigger a player-character thought response (depending on relevance).
According to importance, some thoughts will be pushed with priority and on a no-cooldown channel.
Other less important and more backgroundish events and sitation will only trigger thought-responses if it has been quiet for a while and nothing is going on.

At present there is some content from vanilla Skyrim:
* Thoughts upon using vanilla furniture blacksmith forge, tanning rack, grindstone and others.
* Thoughts upon contracting certain diseases (Stomach rot, others untested).

At present there is some content from Devious Devices / Unforgiving Devices:
* Thoughts upon NEW slowdown penatly applied from some Devious Devices/Unforgiving Devices shoes.
* Thoughts upon equipping / being equipped with yokes and straightjackets (based on hook into Magic Effect).
* Tripping over your feet due to bondage boots.

At present there is some content from YPS fashion mod:
* Thoughts upon (recurring) slowdown penalty applied from YPS fashion mod and the current training status (Untrained feet and High Heels novice only so far).
* YPS thoughts being piped through SkyrimNet player-voice system, but with a minimum time in between, so as to not overwhelm the TTS-channel.

At present there is some content from MME Milk mod:
* Thoughts upon using Milk-Mod-Economy furniture Milk Pumps.
* Thoughts upon Milk level status changes (empty, half-full, full).
* Thoughts upon Lactacid level changes (reaching 0 Lactacid, adding fresh Lactacid).
* Thoughts upon reaching a new MME Maid Level.

At present there is some support for Advanced-Nudity-Detection (AND) mod:
* Thoughts upon changing the nakedness for a particular body section for the 17 basic on-off factions of the AND modesty system.
  Unfortunately at present this does not work so well, when items have the flashing-risk-property enabled, but otherise it should be fine.

Upon picking up items:
* 0.5.0:  If there are special keywords, such as a YPS-fashion-level or an AND-flash-risk keyword or a SL-Survival-bikini keyword, thoughts upon that implication will be raised.
* 0.5.1:  Summoning your own bear (and only the bear so far) via creature summoner mod will trigger a response.
* 0.5.1:  Irresistable Attraction effect will be commented upon as well.  This might not be necessary or too much, but at present we just try how it works out.
* 0.5.1:  Finer dirtyness handling, as a swim doesn't give the same cleanliness as a bath with soap.
* 0.5.2:  Filter comments from clothing changes while in a SL scene, because that feels kind of out-of-place there, at least at the start.

There is some sensitivity to overall SkyrimNet talk going on, so as to not babble too much when others are speaking, but on the other hand, give some updates and hopefully immersion and mental self-talk when it's otherwise quiet and alone time in the woods.

# Usage/Configuration
Everything is hard-coded at the moment.  There are no config-files or MCMs or anything like that.
If you want, you can disable e.g. the background thought channel by deleting the SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought.yaml file. You can do that from within SkyrimNet by editing the list of triggers there.

# Contributing guidelines
You really want to help?  Most people have far more skill and competence to work on something far better and more complex.  This noob codebase would only slow them down.  Just in case:  If you send something, please keep it simple and noob-friendly enough, that I can hope to understand it.  Thanks!

There is infinitely much to do, as there are infinitely many mods, that could be SkyrimNet-integreated in this way.  
The priority should be on stuff, that doesn't yet has a standalone SkyrimNet-integration already.

# License
As free as possible. I guess nobody want this code anyway.  And that is referring only to the parts I added to the project, not the template/libraries that I started with.  See their license in the respective places.

# Credits
I took the template from https://github.com/Monitor221hz/CommonLibSSE-NG-Template-Plugin
So all credit goes to whoever contributes there and is credited there.

# ======= The rest below is from the original CommonLibSSE-NG template, so don't ask me about it ============
Thanks!

# CommonLibSSE NG

Because this uses [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG), it supports Skyrim SE, AE, GOG, and VR. 

Hook IDs and offsets must still be found manually for each version.

# Requirements

- [Visual Studio 2022](https://visualstudio.microsoft.com/) (_the free Community edition_) or [Visual Studio Code](https://code.visualstudio.com/)
- [`vcpkg`](https://github.com/microsoft/vcpkg)
  - 1. Clone the repository using git OR [download it as a .zip](https://github.com/microsoft/vcpkg/archive/refs/heads/master.zip)
  - 2. Go into the `vcpkg` folder and double-click on `bootstrap-vcpkg.bat`
  - 3. Edit your system or user Environment Variables and add a new one:
    - Name: `VCPKG_ROOT`  
      Value: `C:\path\to\wherever\your\vcpkg\folder\is`
  - The latest version of vcpkg needs a default repository defined in the json. If you're using an older version of vcpkg, simply delete the default repository definition in `vcpkg-configuration.json`

## Opening the project

Once you have Visual Studio 2022 installed, you can open this folder in basically any C++ editor, e.g. [VS Code](https://code.visualstudio.com/) or [CLion](https://www.jetbrains.com/clion/) or [Visual Studio](https://visualstudio.microsoft.com/)
- > _for VS Code, if you are not automatically prompted to install the [C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions, please install those and then close VS Code and then open this project as a folder in VS Code_

You may need to click `OK` on a few windows, but the project should automatically run CMake!

It will _automatically_ download [CommonLibSSE NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) and everything you need to get started making your new plugin!

# Project setup

By default, when this project compiles it will output a `.dll` for your SKSE plugin into the `build/` folder.

If you want to configure this project to output your plugin files
into your Skyrim Special Edition's "`Data`" folder:

- Set the `SKYRIM_FOLDER` environment variable to the path of your Skyrim installation  
  e.g. `C:\Program Files (x86)\Steam\steamapps\common\Skyrim Special Edition`

If you want to configure this project to output your plugin files
into your "`mods`" folder:  
(_for Mod Organizer 2 or Vortex_)

- Set the `SKYRIM_MODS_FOLDER` environment variable to the path of your mods folder:  
  e.g. `C:\Users\<user>\AppData\Local\ModOrganizer\Skyrim Special Edition\mods`  
  e.g. `C:\Users\<user>\AppData\Roaming\Vortex\skyrimse\mods`

# Debugging
In order to attach a debugger, you must own a legal copy of Skyrim with the exe stripped using Steamless. Note that users with MO2 should have `-forcesteamloader` as an SKSE argument for plugins to load normally with a stub-removed exe.

For VSCode users, they must have a `launch.json` file like the one below:

```json

{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C++ Debugger",
            "type": "cppvsdbg",
            "request": "attach"
        },
        {
            "type": "cmake",
            "request": "launch",
            "name": "Debug portfile(s)",
            "cmakeDebugType": "external",
            "pipeName": "\\\\.\\pipe\\vcpkg_ext_portfile_dbg",
            "preLaunchTask": "Debug vcpkg commands"
        }
    ]
}
```
