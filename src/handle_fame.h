#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class handle_fame
{
public:

	static void handle_SLSF_Reloaded_fame_stuff();

};

