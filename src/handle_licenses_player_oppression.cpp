#include "handle_licenses_player_oppression.h"

#include "DumpThoughts.h"
#include "handle_config_ini_file.h"
#include "misc.h"

#include <string_view>

namespace
{
	struct LicenseThought
	{
		std::string_view eventName;
		std::string_view message;
	};

	constexpr LicenseThought licenseThoughts[] = {
		{ "BM-LPO_BountyStart", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, can see a guard approaching you. They want to catch you and give you a fine for some ridiculous license regulation or maybe even throw you in jail. Announce that they are coming for you with your response, so that the player is alerted to the situation, and maybe even explain, that it is probably because of license regulation, as the player might otherwise not understand what is going on." },
		{ "BM-LPO_BountyEnd", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, have either outrun them or dealt with them via dialog. We don't know that. So you can simply express your hope now, that they will leave you alone now for a while with their stupid licensing regulations and harassments. Say that or something similar in your response." },
		{ "BM-LPO_WeaponLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new weapons license. Now you can (finally) carry your weapons out in the open as you please, which everybody else seems to be doing without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now carry your weapons in public without being harassed by guards and fined for it." },
		{ "BM-LPO_MagicLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new magic license. Now you can (finally) use magic in public as you please, which everybody else seems to be doing without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now use magic in public without being harassed by guards and fined for it." },
		{ "BM-LPO_ArmorLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new armor license. Now you can (finally) wear your armor in public as you please, which everybody else seems to be doing without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now wear your armor in public without being harassed by guards and fined for it." },
		{ "BM-LPO_BikiniLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new bikini license. Now you can (finally) wear your bikini armor in public as you please, but this may still be a bit degrading, because this license doesn't allow you full armor and real armour, it only allows for skimpy bikini armour, making you a spectacle for male observers. But still, it's better than nothing and the armor still gives you some protection, even if it's not as much as real armor would." },
		{ "BM-LPO_CraftingLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new crafting license. Now you can (finally) craft items in public as you please, which everybody else seems to be doing without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now craft items in public without being harassed by guards and fined for it." },
		{ "BM-LPO_TradingLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new trading license. Now you can (finally) trade items in other than food or jewelry items, which everybody else seems to be doing all the time without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now trade items in public without being harassed by guards and fined for it." },
		{ "BM-LPO_ClothingLicense_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new clothing license. Now you can (finally) wear clothing in public, which everybody else seems to be doing all the time without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now wear clothing in public without being harassed by guards and fined for it." },
		{ "BM-LPO_TravelPermit_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new travel permit. Now you can (finally) travel in and out of town, which everybody else seems to be doing all the time without much of a license, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now travel in and out of town without being harassed by guards and fined for it." },
		{ "BM-LPO_Insurance_20_Activate", "The game has rather weird license regulations now. This is part of player oppression mod. At this point, YOU as the player character, received your new life insurance. You can (finally) go about your business, which everybody else seems to be doing all the time without much of a life insurance policy, or at least you haven't seen one. Could it be that you are being played and just harassed by the guards? But at least you can now have your life insurance and can't be harassed by guards and fined over just living." },
	};
}

// The sequence of License-PlayerOppression mod events is:
// 0.  BM-LPO_ViolationCheck
// 1.  BM-LPO_ViolationFound  (This also happens in dungeons and in the wilderness with no Guard close, it seems, at least recently 20260828)
// 2.  BM-LPO_BountyStart  (4 seconds later)
// 3.  BM-LPO_BountyEnd  (30 seconds later when running away from the guards or guard dialog and punishment is over)
bool handle_licenses_player_oppression::try_handle_mod_event(const SKSE::ModCallbackEvent* a_event)
{
	const std::string_view eventName = a_event->eventName.c_str();
	if (eventName == "BM-LPO_ViolationFound") {
		return true;
	}

	for (const auto& licenseThought : licenseThoughts) {
		if (eventName == licenseThought.eventName) {
			const std::string thoughtMessage{ licenseThought.message };
			if (SNMI::GetSettings().enableLicensesPlayerOppressionThoughts) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thoughtMessage);
			} else {
				SKSE::log::info("Note:  Licensing Player Oppression thought for {} was not delivered because EnableLicensesPlayerOppressionThoughts is disabled.", eventName);
			}
			LillithOnlyBox("Licenses-PlayerOppressionMod: " + thoughtMessage);
			return true;
		}
	}

	return false;
}