#pragma once

#include "log.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "misc.h"
#include "papyrus_interface.h"
#include "handle_fame.h"
#include <unordered_set>
#include <optional>


namespace logger = SKSE::log;

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
	for (auto& fame : fameGlobals) {
		if (!fame.global) {
			fame.global = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>( fame.formID, "SLSF Reloaded.esp");
			SKSE::log::info("NEW+NEW+NEW+NEW:   WE HAD TO LOOK UP THE POINTER FOR SLSF- GLOBAL:  {}", fame.name);
			// In this case (game probably), we assign the current value and previous value as well, so there is 'no change'
			fame.previous_value = fame.global->value;
			fame.current_value = fame.global->value;
		} else {
			float cur_value = fame.global->value;
			SKSE::log::info("NEW+NEW+NEW+NEW:   SLSF-GLOBALPointer was already there for: {}  Value was :  {}", fame.name , cur_value);
			
			// Assign new value and message-box-report value changes for now
			fame.current_value = fame.global->value;
			if (fame.current_value != fame.previous_value) {
				if ( fame.current_value > fame.previous_value ) {
					// Increase:  say as much
					SKSE::log::info("SLSF-Handling: Detected and INCREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
					LillithOnlyBox(std::format("SLSF-Handling: Detected and INCREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
				} else {
					// Decrease:  say as much
					SKSE::log::info("SLSF-Handling: Detected and INCREASE in: {}  from {} to {}.", fame.name, fame.previous_value, fame.current_value);
					LillithOnlyBox(std::format("SLSF-Handling: Detected and DECREASE in: {} from {} to {}." , fame.name , fame.previous_value , fame.current_value));
				}
			}
			fame.previous_value = fame.global->value;
		}
	}
}


