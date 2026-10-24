#pragma once

#include <string>

void install_debug_notification_hook();
void check_for_relevant_notifications(const char* notification);
std::string get_recent_hud_notification_context();
