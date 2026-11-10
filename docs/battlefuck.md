# BattleFuck events and player thoughts in SNMI

BattleFuck starts SexLab threads with the hook name `BattleFuck` for the struggle (BUT:  The point is, that the struggle itself can transit into the first real sex scene until the end and that can be the more likely full arc of scenes here.) and `BattleFuckRape` for the optional follow-up (which in fact means the follow-up, when the player doesn't exit into unconsciousness and thus other participants and bystanders continue into a new follow-up SL scene without any struggle or QTE stuff there). 
SexLab emits the same lifecycle events for both threads, with the hook name appended. SNMI receives names such as `StageStart_BattleFuck`. BattleFuck's Papyrus listeners use the corresponding `HookStageStart_BattleFuck` form.

Two BattleFuck-specific events can arrive during a scene. The local Papyrus patch adds `_BF_InsertionStarted` when the struggle switches from `IsInserted = false` to `true` in the successful insertion branch, before requesting the next animation. The aggressor is the sender. BattleFuck's existing `_BF_Onlookers_AddNotification` carries the formatted spectator notification as its string argument.

## Typical struggle

| Order | Event | SNMI response |
| --- | --- | --- |
| 1 | `AnimationStarting_BattleFuck` | Mark the struggle active, clear the insertion flag, and request an opening thought about the ambush and fear of being stripped. |
| 2 | First `StageStart_BattleFuck`, then `AnimationStart_BattleFuck` in the observed SexLab P+ sequence | The stage start can trigger a thought about losing clothing or armor and being overpowered. The animation start is consumed without a separate thought. |
| 3 | Repeated `StageEnd_BattleFuck` and `StageStart_BattleFuck` | Stage ends are silent. Stage starts refresh the active status and share a 40-second cooldown across struggle scenes. After insertion has been reported, the stage prompt shifts to the player's feelings about the ongoing assault. |
| Optional, during the struggle | `_BF_InsertionStarted` | Set the insertion flag and request an immediate emotional reaction outside the stage cooldown. Include the aggressor's name if available. |
| Optional, as spectators arrive | `_BF_Onlookers_AddNotification` | Mark the scene active and request a reaction to the spectator watching instead of helping. Use a name if the notification includes one. |
| 4 | `AnimationEnding_BattleFuck` → `AnimationEnd_BattleFuck` | The ending event clears the active status and insertion flag. Neither event triggers a thought. The struggle can end in release, escape, interruption, or defeat with a possible follow-up. |

## Optional follow-up

`AnimationStarting_BattleFuckRape` requests a thought about failing to fend off the attacker. 

`AnimationStart_BattleFuckRape` is consumed silently, because that happens right after `AnimationStarting_BattleFuckRape` and does not require a separate thought.

Each `StageStart_BattleFuckRape` requests an in-the-moment reaction without the struggle's 40-second cooldown.

 `StageEnd_BattleFuckRape` has no BattleFuck-specific handler. The follow-up lifecycle handlers leave the struggle status and insertion flag alone.

 `AnimationEnd_BattleFuckRape` requests a reflection on that animation ending. This sequence can repeat for additional follow-up threads.
 
`AnimationEnding_BattleFuckRape` also has no BattleFuck-specific handler, because that is the same as AnimationEnd_BattleFuckRape for our purposesanyway.

All thoughts listed here use the important TTS channel. See [the BattleFuck handler](../src/handle_battlefuck.cpp) and [the mod-event dispatcher](../src/handle_mod_broadcasts.cpp).
