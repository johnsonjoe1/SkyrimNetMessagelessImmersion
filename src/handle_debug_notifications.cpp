#include "handle_debug_notifications.h"

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "misc.h"

#include <array>
#include <charconv>
#include <string_view>
#include <unordered_map>

namespace
{
	struct DeviousFollowersNotificationThought
	{
		std::string_view notification;
		std::string_view thought;
		bool important;
	};

	constexpr std::array deviousFollowersNotificationThoughts = {
		DeviousFollowersNotificationThought{
			"$DF_DEBTADD_NOTI",
			"Your devious follower has just added more gold to your debt. This is not merely an ordinary wage: unpaid debt gives your follower leverage to demand humiliating deals, control your money, punish you, or eventually treat your body as collateral. Respond in first person and make clear that your debt has just increased.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_DEBTTIRED_NOTI",
			"Your devious follower is tired and has just added more gold to your debt because of it. Their tiredness therefore costs you money and gives them more leverage over you. Respond in first person and make clear why your debt has just increased.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_NOREST_NOTI",
			"Your devious follower believes they have not received enough rest and has just added debt as compensation. Respond in first person, making clear that their need for rest has increased what you owe them and strengthened their leverage over you.",
			false },
		DeviousFollowersNotificationThought{
			"$DFDANGER",
			"Your debt to your devious follower has reached a dangerous level. If it keeps growing, they may use it to impose deals, take control of your gold, punish you, or claim your body as collateral. Respond urgently in first person and make clear that the danger comes from your mounting follower debt.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_DEBTOVER_NOTI",
			"Your devious follower is angry because you have not paid what you owe. Their anger is a warning that payment, another deal, punishment, or a worse consequence may soon be demanded. Respond in first person and make clear that your unpaid follower debt caused this.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_BORED_FOLLOWER",
			"Your devious follower is becoming bored because you have accepted fewer deals than they currently expect. In Devious Followers, boredom is dangerous: it can increase what they charge and make them more inclined to play cruel tricks or impose new humiliations. Respond in first person and explain this realization naturally.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_CONFIDENT_FOLLOWER",
			"Your devious follower has grown more confident and demanding. Even though you had enough deals to keep them entertained, they now expect still more from you in the future. Respond in first person and make clear that satisfying them has raised their expectations rather than making you safe.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_WAKEWILLPREST_NOTI",
			"After sleeping, you have had time to reflect and your willpower to resist your Devious Followers has been restored. This makes it harder for your follower to control you for now, although they can still build debt and wear down your resistance again. Respond in first person with relief or renewed determination and make clear what the sleep restored.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_PUNDEBT",
			"Your devious follower has just increased your debt specifically as a punishment. This is not part of the normal daily fee; it is a penalty that also gives them more leverage over you. Respond in first person and make the punitive reason for the added debt clear.",
			true },
		DeviousFollowersNotificationThought{
			"$DFDEALDAYINC",
			"Your devious follower has just extended one of your current deals by another full day. You must obey that deal for longer before it becomes normally eligible for repayment. Respond in first person and make clear that a random existing deal was extended, even though you do not know which one yet.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_FOLLOWER_TAKE",
			"Your devious follower is deliberately confiscating some of the items from your inventory. They have not vanished by accident; your follower is taking them as another exercise of control over you. Respond immediately in first person and make clear what is happening.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_KEYS_ALL_TAKEN",
			"Your devious follower has just taken every restraint key you were carrying. Escaping or unlocking restraints will now depend much more on their permission or help. Respond in first person and make clear that all of your keys have been confiscated.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_ZERO_LIVES",
			"Your devious follower has lost all of their remaining lives and is angry about being bound or incapacitated. They will offer little useful help until they have slept for at least six uninterrupted hours. Respond in first person and make that practical consequence understandable.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_SLEEPTIED_NOTI",
			"You have just awakened to discover that your devious follower tied you up while you slept and is now grinning about it. Respond in first person to the restraints, the betrayal of your vulnerability during sleep, and your follower's obvious satisfaction.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_SLEEPCOUGHT_NOTI",
			"While you slept, your devious follower tried to tie you up, but you woke in time and caught them before they succeeded. Respond in first person to the attempted betrayal and your narrow escape from waking in restraints.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_DEALSPULLED",
			"Your devious follower is physically dragging you toward a client because of your prostitution deal. The encounter is not random: fulfilling that deal is why you are being taken to this person. Respond urgently in first person and make that connection clear.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_DEALSTHIEF",
			"During the encounter required by your prostitution deal, the client noticed your gold and stole some of it. Respond in first person, making clear that the client took advantage of you during the encounter and that your money is now gone.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_MILKING_NOTICE",
			"Your devious follower has noticed that you were milked. If a milking deal is active, they care about how much milk you produced and may claim some of it. Respond in first person to the uncomfortable realization that your follower is monitoring your milk production.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_MILKING_NO_MILKS",
			"You were milked but produced no bottles, and your devious follower considers that a failure under the milking deal. Respond in first person to being judged as an unproductive milk cow and make clear that no milk bottles were produced.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_MILKING_ONE_MILK",
			"You produced only one bottle when you were milked, and your devious follower considers that disappointing under the milking deal. Respond in first person to being judged by your follower for your poor milk production.",
			false },
		DeviousFollowersNotificationThought{
			"$DF_MILKING_FOLLOWER_DRINKS",
			"Your devious follower has just taken and drunk some of the milk produced from your breasts. Respond in first person to seeing them claim your breast milk as something that belongs to them under the milking arrangement.",
			true },
		DeviousFollowersNotificationThought{
			"$DF_MILKING_BOTTLE_TAKEN",
			"Your devious follower has just confiscated one bottle of milk produced from your breasts. Respond in first person and make clear that the bottle was deliberately claimed by your follower as part of their control over your milk production.",
			true }
	};

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastDeviousFollowersThoughtByNotification;

	void emit_devious_followers_thought(
		std::string_view a_notification,
		std::string a_thought,
		bool a_important)
	{
		auto& lastThought = lastDeviousFollowersThoughtByNotification[std::string(a_notification)];
		if (!cooldown_has_passed(lastThought, 15)) {
			SKSE::log::info("Skipping repeated Devious Followers notification during cooldown: {}", a_notification);
			return;
		}

		if (a_important) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(std::move(a_thought));
		} else {
			DumpThoughts::throw_out_TTS_thought_message(std::move(a_thought));
		}
		lastThought = std::chrono::steady_clock::now();
	}

	bool handle_devious_followers_notification(std::string_view a_notification)
	{
		constexpr std::string_view willpowerPrefix = "$DF_WILLPOWER";
		constexpr std::string_view willpowerSuffix = "_NOTI";

		if (a_notification.starts_with(willpowerPrefix) && a_notification.ends_with(willpowerSuffix)) {
			const auto levelText = a_notification.substr(
				willpowerPrefix.size(),
				a_notification.size() - willpowerPrefix.size() - willpowerSuffix.size());
			int level = -1;
			const auto parseResult = std::from_chars(levelText.data(), levelText.data() + levelText.size(), level);
			if (parseResult.ec == std::errc{} && parseResult.ptr == levelText.data() + levelText.size() && level >= 0 && level <= 10) {
				std::string consequence;
				if (level == 0) {
					consequence = "You have no willpower left, giving your follower their greatest degree of control over you.";
				} else if (level <= 3) {
					consequence = "Your willpower is dangerously low, so your follower has much more power to control and pressure you.";
				} else if (level <= 6) {
					consequence = "Your willpower is only moderate, leaving your follower with significant leverage over you.";
				} else if (level < 10) {
					consequence = "You still retain substantial willpower, although your follower has begun to erode it.";
				} else {
					consequence = "Your willpower is fully restored, limiting your follower mainly to adding debt until they wear you down again.";
				}

				emit_devious_followers_thought(
					a_notification,
					std::format(
						"Your Devious Followers willpower has just changed to {} out of 10. {} Resistance is the buffer protecting this stat: whenever resistance is completely exhausted, you lose another point of willpower and resistance resets. Respond in first person and make this change and its meaning clear.",
						level,
						consequence),
					true);
				return true;
			}
		}

		for (const auto& entry : deviousFollowersNotificationThoughts) {
			if (a_notification == entry.notification) {
				emit_devious_followers_thought(entry.notification, std::string(entry.thought), entry.important);
				return true;
			}
		}

		return false;
	}

	using ProcessMessage = RE::UI_MESSAGE_RESULTS (*)(RE::HUDMenu*, RE::UIMessage&);
	REL::Relocation<ProcessMessage> original_process_message;
	static auto last_gag_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_surrender_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_pickaxe_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_fishing_rod_equipped_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_nothing_on_line_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_reel_in_too_early_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_not_enough_gold_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

	RE::UI_MESSAGE_RESULTS process_hud_message(RE::HUDMenu* a_menu, RE::UIMessage& a_message)
	{
		if (a_message.data) {
			if (const auto hud_data = skyrim_cast<RE::HUDData*>(a_message.data);
				hud_data && hud_data->type == RE::HUDData::Type::kNotification) {
				const char* notification = hud_data->text.c_str();
				if (notification && notification[0] != '\0') {
					SKSE::log::info("HUD notification: {}", notification);
					check_for_relevant_notifications(notification);
				}
			}
		}

		return original_process_message(a_menu, a_message);
	}
}

void install_debug_notification_hook()
{
	REL::Relocation<std::uintptr_t> hud_menu_vtable{ RE::VTABLE_HUDMenu[0] };
	original_process_message = hud_menu_vtable.write_vfunc(0x04, process_hud_message);
	SKSE::log::info("Installed HUD notification logging hook");
}



void check_for_relevant_notifications(const char* notification)
{
	SKSE::log::info("Checking for relevant notification: {}", notification);
	if (handle_devious_followers_notification(notification)) {
		return;
	}

	if (strcmp(notification, "You can't eat or drink while wearing this gag.") == 0) {
		if (!cooldown_has_passed(last_gag_notification_thought_timestamp, 10)) {
			return;
		}
		// LillithOnlyBox("Notification detected: You can't eat or drink while wearing this gag.!");
		std::string  thought_message = std::format("YOU, the player, just tried to eat or drink something, but the gag you are wearing prevented it.  This is not only about thirst or hunger, but also about drinking potions for magic effect, so don't assume you know which item the attempt to consume was about.  The fact is, that you just tried hard, but you couldn't manage to get anything into your mouth or throat with that gag still locked onto you.  Say so in your response.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_gag_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You can't eat or drink while wearing this gag.");
	}

	if (strcmp(notification, "You are surrendering!") == 0) {
		if (!cooldown_has_passed(last_surrender_notification_thought_timestamp, 10)) {
			return;
		}
		// RE::DebugMessageBox("Notification detected: You are surrendering!");
		std::string  thought_message = std::format("YOU, the player, don't want to die and chose to just surrender to your enemies.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_surrender_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You are surrendering!");
	}
	
	if (strcmp(notification, "You need a pickaxe to use this.") == 0) {
		if (!cooldown_has_passed(last_pickaxe_notification_thought_timestamp, 6)) {
			return;
		}
		LillithOnlyBox("Notification detected: You need a pickaxe to use this.");
		std::string  thought_message = std::format("YOU, the player, were trying to use a mining spot for mining ores, but you just found out, you don't have a pickaxe on you.  Without a pickaxe you can't mine anything, it's as simple as that.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_pickaxe_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You need a pickaxe to use this.");
	}	

	if (strcmp(notification, "You reeled in your line too early.") == 0) {
		if (!cooldown_has_passed(last_reel_in_too_early_notification_thought_timestamp, 60)) {
			return;
		}
		LillithOnlyBox("Notification detected: You reeled in your line too early.");
		std::string  thought_message = std::format("YOU, the player, reeled in your fishing line just a tiny bit too early.  So the fish was close to biting, but then it got away.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		last_reel_in_too_early_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You reeled in your line too early.");
	}
	
	if (strcmp(notification, "You must have a fishing rod equipped to use this.") == 0) {
		if (!cooldown_has_passed(last_fishing_rod_equipped_notification_thought_timestamp, 60)) {
			return;
		}
		LillithOnlyBox("Notification detected: You must have a fishing rod equipped to use this.");
		std::string  thought_message = std::format("YOU, the player, tried to fish without having a fishing rod equipped.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		last_fishing_rod_equipped_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You must have a fishing rod equipped to use this.");
	}

	if (strcmp(notification, "There was nothing on your line.") == 0) {
		if (!cooldown_has_passed(last_nothing_on_line_notification_thought_timestamp, 60)) {
			return;
		}
		LillithOnlyBox("Notification detected: There was nothing on your line.");
		std::string  thought_message = std::format("YOU, the player, reeled in your fishing line much too early.  So the fish wasn't even close to biting.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);
		last_nothing_on_line_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: There was nothing on your line.");
	}

	if (strcmp(notification, "You don't have enough gold.") == 0) {
		if (!cooldown_has_passed(last_not_enough_gold_notification_thought_timestamp, 60)) {
			return;
		}
		LillithOnlyBox("Notification detected: You don't have enough gold.");
		std::string  thought_message = std::format("YOU, the player, tried to make a purchase but didn't have enough gold.  Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message, DumpThoughts::DialogueHandling::kProcessImmediately);
		last_not_enough_gold_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You don't have enough gold.");
	}


	
}
