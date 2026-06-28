#pragma once

#include "log.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "misc.h"
#include "papyrus_interface.h"
#include "handle_fame.h"
#include <unordered_set>
#include <optional>
#include <string_view>

namespace logger = SKSE::log;

const std::string fame_log_level = "change_only";


// ****************************************************************************************************************
//  We handle changes of the fame-Status here.
//
//  Note to self:  static keyword FOR FUNCTION only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

struct FameGlobal
{
	// index-number
	uint32_t formID;
	const char* name;
	RE::TESGlobal* global;
	float previous_value;
	float current_value;
};

static std::vector<FameGlobal> fameGlobals = {
	{ 0x0080C , "SLSF_Reloaded_CurrentSlutFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0080D , "SLSF_Reloaded_CurrentWhoreFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0080E , "SLSF_Reloaded_CurrentExhibitionistFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0080F , "SLSF_Reloaded_CurrentOralFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00810 , "SLSF_Reloaded_CurrentAnalFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00811 , "SLSF_Reloaded_CurrentNastyFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00812 , "SLSF_Reloaded_CurrentPregnantFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00813 , "SLSF_Reloaded_CurrentDominantFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00814 , "SLSF_Reloaded_CurrentSubmissiveFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00815 , "SLSF_Reloaded_CurrentSadistFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00816 , "SLSF_Reloaded_CurrentMasochistFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00817 , "SLSF_Reloaded_CurrentGentleFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00818 , "SLSF_Reloaded_CurrentMenFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00819 , "SLSF_Reloaded_CurrentWomenFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081A , "SLSF_Reloaded_CurrentOrcFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081B , "SLSF_Reloaded_CurrentKhajiitFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081C , "SLSF_Reloaded_CurrentArgonianFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081D , "SLSF_Reloaded_CurrentBestialityFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081E , "SLSF_Reloaded_CurrentGroupFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0081F , "SLSF_Reloaded_CurrentBoundFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00820 , "SLSF_Reloaded_CurrentTattooFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00821 , "SLSF_Reloaded_CurrentCumDumpFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00822 , "SLSF_Reloaded_CurrentCheatFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00823 , "SLSF_Reloaded_CurrentCuckFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082F , "SLSF_Reloaded_CurrentAirheadFame" , nullptr, -99.0f, -99.0f } , 
	{ 0x00824 , "SLSF_Reloaded_OralCumVisible" , nullptr, -99.0f, -99.0f } , 
	{ 0x00825 , "SLSF_Reloaded_AnalCumVisible" , nullptr, -99.0f, -99.0f } , 
	{ 0x00826 , "SLSF_Reloaded_VaginalCumVisible" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082A , "SLSF_Reloaded_CommentFrequency" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082B , "SLSF_Reloaded_VisiblyBound" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082C , "SLSF_Reloaded_LightlyBound" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082D , "SLSF_Reloaded_HeavilyBound" , nullptr, -99.0f, -99.0f } , 
	{ 0x0082E , "SLSF_Reloaded_Skooma" , nullptr, -99.0f, -99.0f } , 
	{ 0x00830 , "SLSF_Reloaded_IsBelted" , nullptr, -99.0f, -99.0f } , 
	{ 0x00831 , "SLSF_Reloaded_IsCollared" , nullptr, -99.0f, -99.0f } , 
	{ 0x00835 , "SLSF_Reloaded_OralCumCount" , nullptr, -99.0f, -99.0f } , 
	{ 0x00836 , "SLSF_Reloaded_AnalCumCount" , nullptr, -99.0f, -99.0f } , 
	{ 0x00837 , "SLSF_Reloaded_VaginalCumCount" , nullptr, -99.0f, -99.0f } , 
	{ 0x00838 , "SLSF_Reloaded_TotalCumCount" , nullptr, -99.0f, -99.0f } , 
	{ 0x00829 , "SLSF_AllowComment" , nullptr, -99.0f, -99.0f } , 
	{ 0x0080B , "SLSF_Reloaded_NPCScanSucess" , nullptr, -99.0f, -99.0f } , 
	{ 0x00805 , "SLSF_Reloaded_CustomLocationCount" , nullptr, -99.0f, -99.0f } 
};

void handle_fame::handle_SLSF_Reloaded_fame_stuff()
{
	bool some_fame_change_happend = false;
	std::vector<std::string_view> fame_increases;
	std::vector<std::string_view> fame_decreases;	

	for (auto& fame : fameGlobals) {
		if (!fame.global) {
			fame.global = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>( fame.formID, "SLSF Reloaded.esp");
			SKSE::log::info("NEW+NEW+NEW+NEW:   WE HAD TO LOOK UP THE POINTER FOR SLSF- GLOBAL:  {}", fame.name);
			// In this case (game probably), we assign the current value and previous value as well, so there is 'no change'
			fame.previous_value = fame.global->value;
			fame.current_value = fame.global->value;
		} else {
			float cur_value = fame.global->value;

			if ( fame_log_level == "change_only" ) {
				if ( cur_value != fame.previous_value ) {
					SKSE::log::info("SLSF-GLOBALPointer existed:{}  value={}  previous_value={}", fame.name , cur_value, fame.previous_value);
					some_fame_change_happend = true;
				}
			} else {
				SKSE::log::info("SLSF-GLOBALPointer existed:{}  value={}  previous_value={}", fame.name , cur_value, fame.previous_value);
			}
			
			
			// Assign new value and message-box-report value changes for now


			fame.current_value = fame.global->value;
			if (fame.current_value != fame.previous_value) {

				if ( fame.name == "SLSF_Reloaded_NPCScanSucess" ) {
					continue;  // This event is not so interesting
				}
				if ( fame.name == "SLSF_AllowComment" ) {
					continue;  // This event is not so interesting
				}
				if ( fame.name == "SLSF_Reloaded_CurrentTattooFame" ) {
					continue;  // The Tattoo fame seems to increase also from whipping marks and from dirt (which is a SlaveTat in many cases) as well, so this is very prone to error.  Ignore it.
				}
				if ( fame.current_value > fame.previous_value ) {
					// Increase:  say as much
					fame_increases.push_back(fame.name);
					SKSE::log::info("SLSF-Handling: Detected an INCREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
					LillithOnlyBox(std::format("SLSF-Handling: Detected an INCREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
				} else {
					// Decrease:  say as much
					fame_decreases.push_back(fame.name);
					SKSE::log::info("SLSF-Handling: Detected a DECREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
					LillithOnlyBox(std::format("SLSF-Handling: Detected a DECREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
				}

				if ((fame.current_value > 50) && (fame.previous_value <= 50)) {
					LillithOnlyBox(std::format("SLSF-Handling: CROSSING the 50 THRESHOLD:   TRIGGERING A THOUGHT VALUE FOR: {}.", fame.name));
					std::string fame_thought_message = std::format(
R"SKSE(YOU, the player, noticed people starting to talk about you behind your back and whispering.  
Because of your actions, you are now famed in the following category:  {}. 
What are you thinking now based on this? How do you feel about it?  
And let us know from your response, that you speak about your fame in the given category.
)SKSE", fame.name);
					DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(fame_thought_message);
				} 
			}
			fame.previous_value = fame.global->value;
		}
	}
	if (!some_fame_change_happend){
		SKSE::log::info("SLSF-handing:  Checked for changes in all fame-categories, but no changes detected.");
	} else {
		std::string fame_increases_as_string;
		for (std::size_t index = 0; index < fame_increases.size(); ++index) {
			if (index > 0) {
				fame_increases_as_string += ", ";
			}
			fame_increases_as_string += fame_increases[index];
		}
		std::string fame_decreases_as_string;
		for (std::size_t index = 0; index < fame_decreases.size(); ++index) {
			if (index > 0) {
				fame_decreases_as_string += ", ";
			}
			fame_decreases_as_string += fame_decreases[index];
		}

		SKSE::log::info("SLSF-Handling: Fame increases detected in the following categories: {}", fame_increases_as_string);
		SKSE::log::info("SLSF-Handling: Fame decreases detected in the following categories: {}", fame_decreases_as_string);
		std::string fame_thought_message;
		if (!fame_increases.empty()) {
			LillithOnlyBox(std::format("SLSF-Handling: Fame increases detected in the following categories: {}", fame_increases_as_string));
			fame_thought_message = std::format(
R"SKSE(YOU, the player, just entered an area where your sexual reputation in the following categories is noticably higher, 
meaning pleople might be talking about your sexual actions and behaviour.  
But maybe they have forgotten about you and what you did there already. 
The detailed categories where you have increased fame here are: {}.   
What are you thinking now based on this?  How does that make you feel?  
And let us know from your response, that you speak about your reputation as a potentially perverse person in some of the given categories.
)SKSE", fame_increases_as_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(fame_thought_message);
		}
		if (!fame_decreases.empty()) {
			LillithOnlyBox(std::format("SLSF-Handling: Fame decreases detected in the following categories: {}", fame_decreases_as_string));
			fame_thought_message = std::format(
R"SKSE(YOU, the player, just entered an area where your sexual reputation in the following categories is noticably lower, 
maybe because there are no people there to even talk about you, and maybe because nobody knows you here or remembers your actions.  
In any case, people are less likely to speak about your sexual actions and behaviour.  
The detailed categories where you have decreased fame here are: {}.   
What are you thinking now based on this?  How does that make you feel?  
And let us know from your response, that you speak about your reputation as a potentially perverse person in some of the given categories.
)SKSE", fame_decreases_as_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(fame_thought_message);
		}
	}
}




