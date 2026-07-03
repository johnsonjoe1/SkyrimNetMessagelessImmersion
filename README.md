
# Project description
Ever had to open the Skyrim Magic Effect screen to see what is going on with your character?
Ever overlooked a notification message that flashed by but was important for gameplay?

This project takes SkyrimNet and adds some additional triggers to trigger player-thoughts that should resolve that.
The idea is to make Skyrim basically messageless, because the player character notices all changes in-game and reflects on them.
So that in an ideal scenario, all widgets and all fonts could be removed from the screen for better immersion.

The idea is, not to change anything about what you do in game.  The only change should be more immersion and more responsiveness concerning SkyrimNet player-thoughts, which in turn should be enough to make all other prompts also aware of relevant proceedings.
So everything should play the same, just with an potentially messageless and widgetless interface if you want it that way.

# Installation instructions and technicallities

This is mainly an SKSE plugin with CommonLibSSE-NG, meaning that is *should* work with Skyrim version 1.6.1170, as well as the latest GOG versions.
There are *no* hard requirements for other mods (except for SkyrimNet, which is required).
Any other mods, that would benefit from the plugin are not required, as it doesn't change them, it just listens to them and reads out stuff from them. 

How does it work?  It's a noob project.  It just hooks into magic effect changes and mod broadcasts from other mods (or the base game), and if something
relevant shows up, we trigger a player-thought response.  
Any other prompts from SkyrimNet are aware of the player-thoughts anyway, and therefore are also informed.  So SkyrimNet might pick up on the additional
information as well, without extra code from then on.  So minimal invasiveness and a gentle presence in your load order is the goal.

Other mods that are being picked on (at least in minimal amounts) when they are present:
* YPS fashion mod:  YPS thoughts piped to SkyrimNet, YPS heels and feet training status and slowdown effects, comments on activation of clothing with YPS keywords, 
  (https://www.loverslab.com/files/file/2583-immersive-hair-growth-and-styling-yps-devious-immersive-fashion-2025-06-08/),
* MME Milk mod:  Full milk, halfway full, empty, lactacid level rise, milk pump usage, maid level rise,  (https://www.loverslab.com/files/file/6103-milk-mod-economy-se/),
* Advanced Nudity Detection (AND) mod:  Change of (partial) nudity state, (https://www.nexusmods.com/skyrimspecialedition/mods/165289),
* SLSF:  Major changes of fame,  (https://www.loverslab.com/files/file/35874-sexlab-sexual-fame-reloaded/),
* Battlefuck:  Start of struggle comments, (https://www.loverslab.com/files/file/18241-battle-fuck/),
* BodySearch:  Start of search comments, (https://www.loverslab.com/files/file/9318-sexlab-body-search/),
* Unforgiving Devices / Unforgiving Skyrim:  Application and removal of certain devices, 
* SLAC:  Start of scenes and end of scenes, (https://www.loverslab.com/files/file/6022-sexlab-aroused-creatures-se-2026-02-20/),
* SL Survival 0.685 Beta SE: barefoot effect, (https://www.loverslab.com/blogs/entry/20175-sl-survival/)
* STA v4.8 BETA SE: run-up-and-spank mod broadcast, (https://www.loverslab.com/blogs/entry/20176-spank-that-ass/)
* The Ancient Profession:  Generic freelance work scene. (https://www.loverslab.com/files/file/11556-the-ancient-profession-2024-06-24/),
* Vanilla Skyrim:  disease application and cure (partial, not everything), furniture use.

Note, that as said, there are only minimal gentle changes, nothing big or invasive.  And none of them are required in any form.  I add more stuff, as I play along and find something is missing and more response from SkyrimNet would be appropriate here and there.  Not a very systematic or completionist approach, but rather picking up stuff here and there.

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
* 0.5.2:  Added basic support for Body-Search Mod:  Comments on scene start.
* 0.5.2:  Added basic support for BattleFuck Mod.
* 0.5.2:  yps Flexible Feet training status now handled properly.
* 0.5.2:  All creature sommonings from creature summoner mod now handled properly, with a comment on the summoning.

There is some sensitivity to overall SkyrimNet talk going on, so as to not babble too much when others are speaking, but on the other hand, give some updates and hopefully immersion and mental self-talk when it's otherwise quiet and alone time in the woods.

# Usage/Configuration
Everything is hard-coded at the moment.  There are no config-files or MCMs or anything like that.
If you want, you can disable e.g. the background thought channel by deleting the SNMI_Pump_BACKGROUNDCHANNEL_PlayerThought.yaml file. You can do that from within SkyrimNet by editing the list of triggers there.

# Contributing guidelines
You really want to help?  Great!  There are no rules.  Do what you want and can.  

# License
As free as possible. I guess nobody want this code anyway.  And that is referring only to the parts I added to the project, not the template/libraries that I started with.  See their license in the respective places.

# Credits
I took the template from https://github.com/Monitor221hz/CommonLibSSE-NG-Template-Plugin
So all credit goes to whoever contributes there and is credited there.

# Technical information for mod developers on how to compile this stuff

See the template from https://github.com/Monitor221hz/CommonLibSSE-NG-Template-Plugin

Thanks!

