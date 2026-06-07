#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "misc.h"
#include <string_view>

std::string  final_lillith_message;
// void LillithOnlyBox(const char* a_message)

void LillithOnlyBox(std::string_view a_message)
{
	/*  ORIGINAL:
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(("SNMI:  An unhandled mod-event was discovered: " + debug_message).c_str());
	} else {
		SKSE::log::info("SNMI:  An unhandled mod-event was discovered: {}", debug_message);
	}  
	*/
	final_lillith_message = std::string("SNMI: LILLITH: ");
	final_lillith_message += a_message;
	if (strcmp(RE::PlayerCharacter::GetSingleton()->GetName() , "Lillith") == 0)
	{
		RE::DebugMessageBox(final_lillith_message.c_str());  // Show the message in a message box if the player's name is Lillith.
	} else {
		SKSE::log::info("LILLITH-MESSAGEBOX-REDIRECTED-TO-LOG: {}", a_message);  // Otherwise, log the message to the console.
	}  
}
