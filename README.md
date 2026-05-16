
# Project description
This project takes SkyrimNet and adds some additional triggers to trigger player-thoughts.
The idea is to make Skyrim basically messageless, because the player character notices all changes in-game and reflects on them.
So that in an ideal scenario, all widgets and all fonts could be removed from the screen for better immersion.
Of course, this is just a noob project, with so far no real experience with Skyrim modding.
At present, there is only a SKSE plugin and some trigger-yaml files for SkyrimNet.
Nothing is overwritten and no other mods or proceedinds are changed.  That is one of the ideas here:  
Don't change anything about the mechanics of Skyrim or any installed mods.
Just make them messageless, if the user so desires.
Make especially those messages that just scroll by and are easily overlooked into a player-thought (or later maybe some other) reaction,
e.g. when getting infected with a disease, and it's visible in the active effect page, it should also trigger a response thought in-game.
When the disease gets worse, it should trigger another reaction.  
If an items has some magic effects attached to it, that could trigger more responses by the player-character in game.

# Installation instructions
There is currently no release package.  If you really want one, I can make one.  Just say so.

Requirements:  Skyrim AE latest version 1.1170 or something, Skyrim SKSE the latest version, and SkyrimNet (20.2+ version or any version that still has trigger yaml-files, but they don't change that much)
Any other mods, that would benefit from the plugin are not required, as it doesn't change them, it just listens to them and reads out stuff from them. 

# Features list
At present, there are some hooks into the activation of furniture, like Tanning Rack and Blacksmith forge, that should trigger player thought.
Then there are some hooks into the active magic effect process.  Whatever seems reasonable and can easily be interpreted from what is there in the event data will trigger a response.
Also, mod event broadcasts from other mods will be listened to, and, where applicable, trigger a player-character thought response (depending on relevance).
According to importance, some thoughts will be pushed with priority and on a no-cooldown channel.
Other less important and more backgroundish events and sitation will only trigger thought-responses if it has been quiet for a while and nothing is going on.

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
