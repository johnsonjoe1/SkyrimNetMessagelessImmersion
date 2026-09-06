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

		// RE::DebugMessageBox("Notification detected: You can't eat or drink while wearing this gag.!");
		std::string  thought_message = std::format("YOU, the player, just tried to eat or drink something, but the gag you are wearing prevented it.  This is not only about thirst or hunger, but also about drinking potions for magic effect, so don't assume you know which item the attempt to consume was about.  The fact is, that you just tried hard, but you couldn't manage to get anything into your mouth or throat with that gag still locked onto you.  Say so in your response.");
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought_message);   // this should be rare enough to use the important TTS thought channel.
		last_gag_notification_thought_timestamp = std::chrono::steady_clock::now();
	} else {
		SKSE::log::info("Test failed.  This isnt: You can't eat or drink while wearing this gag. Non-Relevant: {}", notification);
	}
}