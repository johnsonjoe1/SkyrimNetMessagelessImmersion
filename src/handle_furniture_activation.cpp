#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include "DumpThoughts.h"
#include <array>
#include <string_view>  
#include <unordered_set>

std::string general_word_on_milk_pumps = R"SKSE(
A general word on Milk Pumps: Milk pumps in Skyrim are milking stalls, much like for a milk cow, only that this device is designed for human women.
The position is the same as a cow though, on all fours, and milk pumps attach from below and suck the milk from the woman's breasts.
The milk pump is a heavy, intricate contraption of dark wood, leather, and polished brass, built with a strange mix of craftsmanship and clinical purpose. 
Thick straps and padded supports hold the body steady while slender tubes and softly pulsing pumps work with relentless rhythm, drawing milk into waiting glass containers below. 
Valves hiss quietly, gears click in measured cadence, and the whole machine seems almost alive in the way it breathes and churns with mechanical patience. 
Despite its intimidating appearance, every curve and restraint is shaped for long use, worn smooth by countless sessions beneath the pump’s steady, inescapable pull.
)SKSE";

std::string standard_thought_instruction = R"SKSE(
Now your task is this:  Comment on this, with thoughts of YOU as the player. How does the situation make you feel? What are you thinking now?";
)SKSE";

std::string milk_pump_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are lowering yourself into a so-called milk pump as a subject for extraction of milk from your breasts. 
Milk pumps in Skyrim are milking stalls, much like for a milk cow, only that this device is designed for human women.
The position is the same as a cow though, on all fours, and milk pumps attach from below and suck the milk from the woman's breasts.
Moreover, you will be fittet with a so-called Milking Cuirass, which is like a piece of armor, that is designed to be suck milk from your breasts and also features humiliating cow design
and leaves your body almost completely naked otherwise.
)SKSE" + general_word_on_milk_pumps + standard_thought_instruction;

std::string milk_pump_prompt_2 = R"SKSE(
Now the situation is this: YOU, the player, just finished subjecting yourself into a so-called milk pump, as a subject for extraction of milk from your breasts. 
Your mana and stamina have probably been completely drained. And there is probably no more milk left in your breasts, so you are probably feeling empty and exhausted. 
Also, there might be residual pain in your nipples, and of course the humiliation of being treated like a cow and milked in this contraption might be overwhelming.
The Milking Cuirass you were fitted with, which is like a piece of armor, that is designed to be suck milk from your breasts and also features humiliating cow design 
is removed and you get back into your outfit from before.
)SKSE" + general_word_on_milk_pumps + standard_thought_instruction;

std::string blacksmith_forge_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Blacksmith Forge. 
Blacksmith Forges allow you to craft entire new weapons and armor or also other small useful items for everyday use, but you must have the right materials 
and the nescessary smithing skill to do so.
)SKSE" + standard_thought_instruction;

std::string grindstone_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Grindstone. 
Grindstones allow you to sharpen and improve your weapons.  You must have the right materials 
and the necessary smithing skill to do so.  It is not possible to create new items here. The Grindstone is only for improvements.
)SKSE" + standard_thought_instruction;

std::string workbench_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Workbench. 
Workbenches allow you to improve your armour items.  You must have the right materials 
and the necessary smithing skill to do so.  It is not possible to create new items here. The Workbench is only for improvements.
)SKSE" + standard_thought_instruction;

std::string tanning_rack_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Tanning Rack. 
Tanning Racks allow you to break down items from your inventory into basic leather or leather stripes or simple components like that.
If you have raw hides or old leather items, you can make basic leather components from them. You may also be able to craft clothing items.
)SKSE" + standard_thought_instruction;

std::string smelter_prompt_1 = R"SKSE(
Now the situation is this: YOU, the player, are about to use a Smelter. 
Here you can turn in your raw ores, weapons or armor to smelt and disassemble them into fresh raw materials to be used later as a source for new items or improvements at the Blacksmith forge, the Workbench or the Grindstone.
You must have the right materials to do so.
)SKSE" + standard_thought_instruction;

struct FurniturePrompt
{
	std::string_view trigger;
	std::string prompt;
	std::chrono::steady_clock::time_point last_thought_timestamp = std::chrono::steady_clock::now();
};

constexpr int furniture_thought_cooldown_seconds = 120;

std::array<FurniturePrompt, 36> furniture_prompts = {{
	{ "Blacksmith Forge", blacksmith_forge_prompt_1 },
	{ "Grindstone", grindstone_prompt_1 },
	{ "Workbench", workbench_prompt_1 },
	{ "Tanning Rack", tanning_rack_prompt_1 },
	{ "Smelter", smelter_prompt_1 },   // Arcane Enchanter  // Alchemist's Retort  // Alchemy Lab  // Staff Enchanter // Bench

	{ "Arcane Enchanter", R"SKSE(Now the situation is this: YOU, the player, are about to use an Arcane Enchanter. )SKSE" + standard_thought_instruction },
	{ "Alchemist's Retort", R"SKSE(Now the situation is this: YOU, the player, are about to use an Alchemist's Retort. )SKSE" + standard_thought_instruction },
	{ "Alchemy Lab", R"SKSE(Now the situation is this: YOU, the player, are about to use an Alchemy Lab. )SKSE" + standard_thought_instruction },
	{ "Staff Enchanter", R"SKSE(Now the situation is this: YOU, the player, are about to use a Staff Enchanter. )SKSE" + standard_thought_instruction },
	{ "Bench", R"SKSE(Now the situation is this: YOU, the player, are about to use a Bench. )SKSE" + standard_thought_instruction },
	{ "Wood Chopping Block", R"SKSE(Now the situation is this: YOU, the player, are about to use a Wood Chopping Block. )SKSE" + standard_thought_instruction },
	{ "Altar", R"SKSE(Now the situation is this: YOU, the player, are about to use an Altar, that is in the style of the Altars of Molag Bal. )SKSE" + standard_thought_instruction },
	{ "Altar of Dibella", R"SKSE(Now the situation is this: YOU, the player, are about to use an Altar of Dibella. )SKSE" + standard_thought_instruction },
	{ "Anvil", R"SKSE(Now the situation is this: YOU, the player, are about to use an Anvil. )SKSE" + standard_thought_instruction },
	{ "Bed", R"SKSE(Now the situation is this: YOU, the player, are about to use a Bed. )SKSE" + standard_thought_instruction },
	{ "Chair", R"SKSE(Now the situation is this: YOU, the player, are about to use a Chair. )SKSE" + standard_thought_instruction },
	{ "Cooking Pot", R"SKSE(Now the situation is this: YOU, the player, are about to use a Cooking Pot. )SKSE" + standard_thought_instruction },
	{ "Cooking Spit", R"SKSE(Now the situation is this: YOU, the player, are about to use a Cooking Spit. )SKSE" + standard_thought_instruction },
	{ "Cot", R"SKSE(Now the situation is this: YOU, the player, are about to use a Cot. )SKSE" + standard_thought_instruction },
	{ "Grain Mill", R"SKSE(Now the situation is this: YOU, the player, are about to use a Grain Mill. )SKSE" + standard_thought_instruction },
	{ "Hay Pile", R"SKSE(Now the situation is this: YOU, the player, are about to use a Hay Pile (for sleeping on that). )SKSE" + standard_thought_instruction },
	{ "Healing Altar", R"SKSE(Now the situation is this: YOU, the player, are about to use a Healing Altar. )SKSE" + standard_thought_instruction },
	{ "Lumber Pile", R"SKSE(Now the situation is this: YOU, the player, are about to use a Lumber Pile. )SKSE" + standard_thought_instruction },
	{ "Table", R"SKSE(Now the situation is this: YOU, the player, are about to use a Table. )SKSE" + standard_thought_instruction },
	{ "Carriage", R"SKSE(Now the situation is this: YOU, the player, are about to use a Carriage. )SKSE" + standard_thought_instruction },
	// Note:  Pull Chain may be in the list, but will be captured and suppressed before it is used.  Don't make philosophical comments on levers and pullchains.
	{ "Pull Chain", R"SKSE(Now the situation is this: YOU, the player, are about to use a Pull Chain. )SKSE" + standard_thought_instruction },
	// Note:  Lever may be in the list, but will be captured and suppressed before it is used.  Don't make philosophical comments on levers and pullchains.
	{ "Lever", R"SKSE(Now the situation is this: YOU, the player, are about to use a Lever. )SKSE" + standard_thought_instruction },
	{ "This should not be visible", R"SKSE(Now the situation is this: YOU, the player, are about to TRY and use a mining spot, where you can mine ores, but only if you have a pickaxe, otherwise this will fail and you can do nothing to mine the ores here." )SKSE" + standard_thought_instruction },
	{ "The Mournful Throne", R"SKSE(Now the situation is this: YOU, the player, are about to use the throne in Markath, which is also called The Mournful Throne. )SKSE" + standard_thought_instruction },
	{ "Shor’s Throne", R"SKSE(Now the situation is this: YOU, the player, are about to use Shor’s Throne. )SKSE" + standard_thought_instruction },
	{ "Skyforge", R"SKSE(Now the situation is this: YOU, the player, are about to use Skyforge. )SKSE" + standard_thought_instruction },
	{ "Sarcophagus", R"SKSE(Now the situation is this: YOU, the player, are about to use a Sarcophagus. )SKSE" + standard_thought_instruction },
	// Note:  Throne may be in the list here, but each throne in the holds has a special ID, so maybe we can differentiate from the ID in a special handling.  But that would be done later.
	{ "Throne", R"SKSE(Now the situation is this: YOU, the player, are about to use a Throne. )SKSE" + standard_thought_instruction },
	{ "Stone Bed", R"SKSE(Now the situation is this: YOU, the player, are about to use a Stone Bed. )SKSE" + standard_thought_instruction },
	{ "Stone Bench", R"SKSE(Now the situation is this: YOU, the player, are about to use a Stone Bench. )SKSE" + standard_thought_instruction },
	{ "Stone Chair", R"SKSE(Now the situation is this: YOU, the player, are about to use a Stone Chair. )SKSE" + standard_thought_instruction },
}};	

	/*
Shortened list from:  https://www.wolf-hund.org/lists-furniture-containers-books/#furniture
	000BAD0C : Alchemy Lab (CraftingAlchemyWorkbench)
000D54FF : Alchemy Lab (CraftingAlchemyWorkbenchTabletop)
000EC970 : Altar (AltarOfMolagBalFurniture01)
000F11F1 : Altar of Dibella (BlessingKneelMarker)
000F9AA1 : Altar of Dibella (BlessingKneelSandboxMarker)
0001A2AD : Anvil (CraftingBlacksmithAnvil)
000BAD0D : Arcane Enchanter (CraftingEnchantingWorkbench)
000D5501 : Arcane Enchanter (CraftingEnchantingWorkbenchTabletop)
00030091 : Bed (CommonBed01)
0003B430 : Bed (UpperBedSingle01R)
00036ED3 : Bed Roll (Bedroll01)
000B8371 : Bed Roll Ground (BedrollGround)
0002E6CF : Bench (CommonBench01)
000CAE0B : Blacksmith Forge (CraftingBlackSmithForge)
000BF9E1 : Blacksmith Forge (CraftingBlackSmithForgeWR)
000A19DF : BoundCaptiveMarker (BoundCaptiveMarker)
00103445 : Carriage (CartFurniturePassenger)
0002EC1C : Chair (CommonChair01)
00104110 : Cooking Pot (CraftingCookingPotLG)
00068ADB : Cooking Spit (CraftingCookingFireSpit)
0006D2D3 : Cot (CivilWarCot01)
0009C6DF : Grain Mill (GrainMill)
0006E9C2 : Grindstone (CraftingBlacksmithSharpeningWheel)
0001899D : Hay Pile (BedrollHay01)
000D4848 : Healing Altar (WRTempleHealingAltar01)
0006411B : Invisible Chair Marker (InvisibleChairMarker)
000A035C : Invisible Chair Marker (InvisibleChairMarkerChild)
00037A1E : Invisible Chair Marker (InvisibleChairMarkerF)
00017041 : Invisible Chair Marker (InvisibleChairMarkerFChild)
00037A1F : Invisible Chair Marker (InvisibleChairMarkerFL)
00037A29 : Invisible Chair Marker (InvisibleChairMarkerFR)
00037A2E : Invisible Chair Marker (InvisibleChairMarkerLR)
000D7AFA : Invisible Shelf Marker (InvisibleShelfMarker)
00038A49 : Lay (CreatureAlcoveMarker)
00073128 : Lever (LeverPushAnimating)
00071C47 : Lumber Pile (ResourceObjectSawmill)
0010528D : MeadBarrel (TG03MeadBarrel)
000C4E54 : Nest (DragonLayNest)
00083040 : Pillar of Sacrifice (DA02Pillar)
00084D1C : Pot (SpitPotClosed01NOTCRAFTING)
00109A7D : Pull Chain (GenPullChainAnim01NoPlayer)
00106513 : Pull Chain (NorPullChainAnim01NoPlayer)
0007CC0D : Sarcophagus (DBSancSarcophagusBedroll)
0005E511 : Shor’s Throne (SOVThrone01)
0003F7C0 : Sit (ChairInvisibleSingle)
00104B9E : Sit (ChairInvisibleSingleR)
00037A47 : Sit (InvisibleChairDoubleMarker)
00045D99 : Sit (WebFurnitureMarker)
000BBCF1 : Skyforge (CraftingBlackSmithForgeSkyforge)
0009C6CE : Smelter (CraftingSmelterMarker1)
000FF134 : Stone Bed (DweFurnitureBedChildSingle01L)
00066015 : Stone Bench (DweFurnitureBench01)
00063DF0 : Stone Chair (DweFurnitureChair01)
000B0105 : Table (CommonTableOneBench)
000727A1 : Tanning Rack (CraftingTanningRackMarker)
0001E16D : test (BucketCarryFillMarker)
000E1965 : test (dunNightcallerSleepMarker01)
000EDF3B : TG08_CeremonyMarker (TG08_CeremonyMarker)
001075FE : The Mournful Throne (MarkarthKeepThrone)
000BD15D : This should not be visible (MS02PickaxeMiningFloorMarker)
000F9AF8 : This should not be visible (MS02PickaxeMiningTableMarker)
000BD15C : This should not be visible (MS02PickaxeMiningWallMarker)
000613A6 : This should not be visible (PickaxeMiningFloorMarker)
000613A7 : This should not be visible (PickaxeMiningTableMarker)
000E2BC7 : This should not be visible (PickaxeMiningWallMarker)
000267D3 : Throne (NorThroneShadow)
000985C2 : Throne (ShipKatariahThrone01)
0007FBC2 : Throne (SolitudeThrone)
00107354 : Throne (ThroneDawnstar01)
00107352 : Throne (ThroneFalkreath01)
00107351 : Throne (ThroneHjaalmarch01)
00107355 : Throne (ThroneRiften01)
00107353 : Throne (ThroneWinterhold01)
0010F636 : Throne (WindhelmThrone)
000B244B : Throne (WRThrone01)
00102010 : Underforge Fountain (DrinkUnderForgeFountainMarker)
0007022E : Wood Chopping Block (WoodChoppingBlock)
00074EC6 : Wooden Bar Stool (WoodenBarStool)
000D932F : Workbench (CraftingBlacksmithArmorWorkbench)
000EA33B : WriteOnTable (WriteTableStanding)
00107D99 : (AltarOfMolagBalFurniture01NoName)
000E743E : (BoethiahPillarFurniture01)
0001E342 : (BucketCarryPourMarker)
00090048 : (CartFurniture)
00103442 : (CartFurnitureDriver)
00105D4D : (CartFurnitureHorse)
00105828 : (CommonWritingChair01L)
000EAA4C : (CommonWritingChair01LR)
00105829 : (CommonWritingChair01R)
000F507A : (CounterBarLeanMarker)
0006CF36 : (CounterLeanMarker)
0010BFE3 : (CraftingCookingPotInvisible)
00046B81 : (CreatureAlcoveBgMarker)
0007DD71 : (CreatureSitMarker01)
0008CDDE : (CreatureSitMarker02)
0008CDE7 : (CreatureSitMarker03)
000E605D : (DA11AltarInvisibleBed)
000E605C : (DA16LayDownMarker01)
000CDFDF : (DA16LayDownMarker01F)
000CDFDC : (DA16LayDownMarker01L)
000CDFDD : (DA16LayDownMarker01LR)
000CDFDE : (DA16LayDownMarker01R)
000E7AEA : (DisenchantmentFont01)
000CAB95 : (DragonMound)
000AA934 : (DragonPerchRockL02)
0001E595 : (DragonPerchTower)
0001CC4A : (DragonPerchWordwall)
0006CF46 : (DragonTowerHole01)
000D6C36 : (dunLostValleyRedoubtAltar01L)
00023635 : (DweCenturionAmbush01)
00070386 : (DwePipeCapSpiderExHorz)
00070387 : (DwePipeCapSpiderExVert)
00071E61 : (DweSphereCenturionPort01)
0002E8EB : (ExecutionerChoppingBlock)
000BCF21 : (FalmerWallPod01)
00042D31 : (FrostSpiderAmbush01)
000F688D : (GreybeardMeditate)
0010ACAA : (HadvarWriteLedger)
000C4328 : (HammerTableMarker)
000C4323 : (HammerWallMarker)
0007A37D : (HideLMarker)
00063F28 : (HideRMarker)
0004F007 : (IceWraithHAMarker01)
00033DCF : (IceWraithVAMarker01)
000E8E4E : (KneelingMarker)
000C482E : (LayDownMarker)
000C4EF1 : (LeanTableMarker)
000ADE08 : (LightSconceMarker)
000F5891 : (MQ203_EsbernTable)
000FBA5F : (MQ206_FelldirRitual)
0001C59B : (NMCoffinFurnitureMarker01)
00101A75 : (NocturnalMarker)
000F9ACC : (PickaxeMiningTableMarkerNonPlayer)
000F11E9 : (PrayCrouchedMarker)
000F9AA2 : (PrayCrouchedSandboxMarker)
00070EA1 : (RailLeanMarker)
000EFC74 : (SearchChest)
000EFC71 : (SearchTableStanding)
000C4820 : (ShackleWallMarker)
000C4EFD : (SitCrossLeggedMarker)
00108D3C : (SitCrossLeggedMarkerNoSandbox)
000C4827 : (SitLedgeMarker)
000BFB04 : (SoldierWallIdle)
000F9AFF : (SoldierWallIdleSandbox)
000E49A7 : (SprigganAmbushMarker01)
000E8E3F : (StatueClimbMarker01)
000F6CD1 : (TG06TranslateBook)
000EAB24 : (VampireCoffin01)
000F09FE : (WalkThroughWallMarker)
00105291 : (WallCornerPeekL)
00105299 : (WallCornerPeekR)
00052FF5 : (WallLeanMarker)
000FE549 : (WEExecutionerChoppingBlock2Seat)
001018EC : (WerewolfEscapeMarker)
0001A315 : (WhiteRunDragonTrap01Furniture)
000EF957 : (WispAmbush)
000BB959 : (WoodPile)
000D8738 : (Wounded01FloorMarker)
000D873D : (Wounded02FloorMarker)
00107EFA : (Wounded02FloorMarkerJorrvaskr)
000D873E : (Wounded03FloorMarker)
*/





FurniturePrompt* find_furniture_prompt(std::string_view trigger)
{
	for (auto& furniture_prompt : furniture_prompts)
	{
		if (trigger == furniture_prompt.trigger)
		{
			return &furniture_prompt;
		}
	}
	return nullptr;
}

void handle_furniture_item_activation(RE::TESBoundObject *base)
{
	SKSE::log::info("That activated object seems to be furniture, so we can proceed.");

	// We want to broadcast mod events.  So we need this event source.
	auto eventSource = SKSE::GetModCallbackEventSource();

	// Since the activation event happend, we can start broadcasting the mod event right away.
	if (eventSource)
	{
		// auto furniture_name = base->GetName();
		const char* furniture_name = base->GetName();
		if (!furniture_name) {
			SKSE::log::error("[SkyrimNetMessagelessImmersion] The furniture name was null!  Emergency abort!");
			return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
		}
		std::string  mod_event_name = "Nothing so far";
		std::string  mod_event_string_arg = "Mod event string not set yet";
		FurniturePrompt* selected_furniture_prompt = nullptr;

		if (std::strcmp(furniture_name , "Milk Pump") == 0) {
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated the Milk Pump!  THIS GETS A SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is using furniture
				mod_event_string_arg = milk_pump_prompt_2;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 2nd Event for Milk Pump! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 2nd Event for ACTIVATION of Milk Pump!");
			}		else {
				mod_event_string_arg = milk_pump_prompt_1;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for Milk Pump! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for ACTIVATION of Milk Pump!");
			}
			mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
		} else if (auto* furniture_prompt = find_furniture_prompt(furniture_name)) {
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a piece of furniture that is in our list of special furniture!  This gets a SPECIAL TREATMENT VIA A DIFFERENT TRIGGER!!!!");
		
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is already using furniture, so this is probably the second event.  No comment on this for all the furnitures in this standard list.
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated a furniture from the list of special furniture, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
			} else if (!cooldown_has_passed(furniture_prompt->last_thought_timestamp, furniture_thought_cooldown_seconds)) {
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Skipping furniture thought for {} because its cooldown has not elapsed.", furniture_prompt->trigger);
				return;
			} else {
				mod_event_string_arg = std::string(furniture_prompt->prompt);
				selected_furniture_prompt = furniture_prompt;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for this furnitures item (because player not in a furniture already! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for this furniture item (because player not in a furniture already)! {} " , mod_event_string_arg );
			}
			mod_event_name = "SNMI_JustPumpMyStringToPlayerThought";
		} else if (std::strcmp(furniture_name , "Lever") == 0 || std::strcmp(furniture_name , "Pull Chain") == 0) {
			// We do nothing for the lever and the pull chain, because that's just some nobrainer in the dungeon, but we must return in order to prevent event generation.
			SKSE::log::info("[SkyrimNetMessagelessImmersion] SKIPPING FURNITURE ACTIVATION EVENT FOR LEVER OR PULL CHAIN, AS THIS SOMETIMES GETS IN THE WAY OF IMPORTANT THINGS.");
			return;
		} else {

			mod_event_name = "SNMI_PlayerActivatedSomething";
			mod_event_string_arg = furniture_name;
			SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated SOMETHING THAT IS NOT A MILK PUMP AT ALL!!  This gets normal treatment.");
			auto* player = RE::PlayerCharacter::GetSingleton();
			auto furniture = player->GetOccupiedFurniture().get();
			if (furniture) {
				// player is already using furniture, so this is probably the second event.  No comment on this in the blacksmith forge case.
				SKSE::log::info("[SkyrimNetMessagelessImmersion] Player just activated SOME FURNITURE ITEM, but this is probably the second furniture event, so we EXIT without no message and no further ado now!!!!");
				return;  // This will then be done in the calling function:   return RE::BSEventNotifyControl::kContinue;
			}	else {

				std::string generic_furniture_prompt_1 = R"SKSE(Now the situation is this: YOU, the player, are about to use a )SKSE";
				generic_furniture_prompt_1 = generic_furniture_prompt_1 + furniture_name;
				generic_furniture_prompt_1 = generic_furniture_prompt_1 + R"SKSE(. )SKSE" + standard_thought_instruction;

				mod_event_string_arg = generic_furniture_prompt_1;
				// DEBUG-ONLY:  RE::DebugMessageBox(("THIS IS THE 1st Event for SOME FURNITURE ITEM! " + mod_event_string_arg ).c_str());
				SKSE::log::info("THIS IS THE 1st Event for SOME FURNITURE ITEM!");
			}				
		}
		
		SKSE::ModCallbackEvent my_event(
			mod_event_name,                        // event name
			mod_event_string_arg,                  // arbitrary string argument 
			123.0f,                                // arbitrary float argument
			RE::PlayerCharacter::GetSingleton()    // sender "Form" argument, can be any form, but here I use the player character as the sender
		);
		eventSource->SendEvent(&my_event);
		if (selected_furniture_prompt) {
			selected_furniture_prompt->last_thought_timestamp = std::chrono::steady_clock::now();
		}
		// This seems to have worked, so we say as much in the log.
		spdlog::info("[SkyrimNetMessagelessImmersion] Mod-event string:  {}", base->GetName());
		spdlog::info("[SkyrimNetMessagelessImmersion] Mod-event sender:  {}", RE::PlayerCharacter::GetSingleton()->GetName());
		spdlog::info("[SkyrimNetMessagelessImmersion] ********************** Sent mod event: {} completed.", mod_event_name);
	} else {
		SKSE::log::error("[SkyrimNetMessagelessImmersion] Failed to get mod event broadcast callback event source!");
	};
}
