#include "handle_debug_notifications.h"

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "misc.h"

namespace
{
	using ProcessMessage = RE::UI_MESSAGE_RESULTS (*)(RE::HUDMenu*, RE::UIMessage&);
	REL::Relocation<ProcessMessage> original_process_message;
	static auto last_gag_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_surrender_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_pickaxe_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_debttired_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_fishing_rod_equipped_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_nothing_on_line_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);
	static auto last_reel_in_too_early_notification_thought_timestamp = std::chrono::steady_clock::now() - std::chrono::hours(1);

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
	if (strcmp(notification, "You can't eat or drink while wearing this gag.") == 0) {
		if (!cooldown_has_passed(last_gag_notification_thought_timestamp, 10)) {
			return;
		}
		// LillithOnlyBox("Notification detected: You can't eat or drink while wearing this gag.!");
		std::string  thought_message = std::format("YOU, the player, just tried to eat or drink something, but the gag you are wearing prevented it.  This is not only about thirst or hunger, but also about drinking potions for magic effect, so don't assume you know which item the attempt to consume was about.  The fact is, that you just tried hard, but you couldn't manage to get anything into your mouth or throat with that gag still locked onto you.  Say so in your response.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_gag_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You can't eat or drink while wearing this gag. Non-Relevant: {}", notification);
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
		SKSE::log::info("Test failed.  This isnt: You are surrendering! Non-Relevant: {}", notification);
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
		SKSE::log::info("Test failed.  This isnt: You need a pickaxe to use this. Non-Relevant: {}", notification);
	}	

	if (strcmp(notification, "$DF_DEBTTIRED_NOTI") == 0) {
		if (!cooldown_has_passed(last_debttired_notification_thought_timestamp, 15)) {  //  15 seconds should be more than enough to prevent spamming this notification.
			return;
		}
		LillithOnlyBox("Notification detected: $DF_DEBTTIRED_NOTI");
		std::string  thought_message = std::format("YOU, the player, are being charged extra money by your devious follower. The reason is that you follower is tired and also time has passed, and he is working on the clock for you, charging you by the hour as well as when feeling tired.  In any case, your follower has added some debt to your bill.Say so in your response and let us know how you feel about it.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_debttired_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: $DF_DEBTTIRED_NOTI Non-Relevant: {}", notification);
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
		SKSE::log::info("Test failed.  This isnt: You reeled in your line too early. Non-Relevant: {}", notification);
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
		SKSE::log::info("Test failed.  This isnt: You must have a fishing rod equipped to use this. Non-Relevant: {}", notification);
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
		SKSE::log::info("Test failed.  This isnt: There was nothing on your line. Non-Relevant: {}", notification);
	}
}