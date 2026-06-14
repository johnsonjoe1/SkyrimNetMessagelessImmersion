#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????


// void LillithOnlyBox(const char* a_message);
void LillithOnlyBox(std::string_view a_message);
bool is_known_useless_event(std::string event_name);

