#pragma once
#include <string>   

void handle_mod_event_broadcasts(const SKSE::ModCallbackEvent* a_event);
void handle_dialogue_menu_event(const RE::MenuOpenCloseEvent* a_event);
void reset_devious_followers_dialogue_tracking();
