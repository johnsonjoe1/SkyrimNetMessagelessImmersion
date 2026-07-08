#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????

// void LillithOnlyBox(const char* a_message);
void LillithOnlyBox(std::string_view a_message);
void set_current_animation_status(std::string_view a_status);
bool player_is_in_a_SL_scene();
void handle_timeout_for_stale_scenes();

void PrintSlots(std::uint32_t mask);
