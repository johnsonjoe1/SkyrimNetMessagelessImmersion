#include "DumpThoughts.h"
#include "handle_active_magic_effect_changes.h"
#include "handle_config_ini_file.h"
#include "handle_iNeed.h"
#include "handle_player_dirt.h"
#include "handle_yps.h"
#include "misc.h"

void run_constant_whining_in_case_of_silence()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return;
	}
	const auto requiredSilence = std::string_view(player->GetName()) == "Lillith" ? 20 : SNMI::GetSettings().silenceRequiredBeforeSpontaneousStatusWhining;
	const auto silenceDuration = std::chrono::steady_clock::now() - DumpThoughts::GetLastSpeechTimestamp();
	if (silenceDuration >= std::chrono::seconds(requiredSilence)) {
		LillithOnlyBox(std::format("run_constant_whining_in_case_of_silence() ran after at least {} seconds without a thought.", requiredSilence));
		if (handle_iNeed::previous_iNeed_fatigue_level == 3) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You are extremely tired. You need a good long night of sleep. Your base Stamina and Magicka are reduced by about 55%. Your movement speed is reduced by 15%. And you can learn new skills 70% slower. Say so in your response and make clear that you are speaking about your fatigue from sleep deprivation.");
		}
		if (handle_iNeed::previous_iNeed_hunger_level == 3) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You are extremely hungry. You need to eat a substantial meal to regain your strength. Your base Health regeneration and damage output are reduced by 70%. And sleeping is less effective by about 20%. Say so in your response and make clear that you are speaking about your hunger.");
		}
		if (handle_iNeed::previous_iNeed_thirst_level == 3) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You are extremely thirsty. You need to drink a substantial amount of water to stay hydrated. Your base Stamina and Magicka regeneration are reduced by about 70%. And sleeping is less effective by about 20%. And also you need 70% more time in between shouts from your dry throat. Say so in your response and make clear that you are speaking about your thirst.");
		}
		if (SNMI::GetSettings().enablePlayerDirtThoughts && handle_player_dirt::is_player_almost_at_filthy_dirt_stage()) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You are very dirty. You need a proper bath with soap to get clean again. Your speechcraft effectiveness is reduced by 50%, because other people find you disgusting. And also your sneak ability is reduced by 25, because adversaries can smell you. And also your disease resistance is reduced by 100%, because you are more susceptible to infections. Say so in your response and make clear that you are speaking about your dirtiness.");
		}
		if (get_current_ataxia_stage() == AtaxiaStage::stage2) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(
				"You are suffering from the most severe stage of Ataxia. The disease is making you move more slowly and deal less damage to enemies. Say so in your response and make clear that these symptoms are caused by your severe Ataxia.");
		}
		for (const auto& sicknessThought : get_current_other_sickness_thoughts()) {
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_with_LILLITH_DEBUG_WINDOW(sicknessThought);
		}
		if (actually_wearing_heels_according_to_yps_thoughts) {
			const char* heelsThought = nullptr;
			switch (handle_yps::get_current_heels_training_status()) {
			case YpsHeelTrainingStatus::untrained_feet:
				heelsThought = "YOU, the player, are currently wearing high heels. You are totally untrained with high heels. You are not even a High Heels Novice yet. So they slow you down massively now. It will not be long before your feet start adapting to them and you get the hang of them and can move a bit faster in them. Say as much in your response.";
				break;
			case YpsHeelTrainingStatus::high_heel_novice:
				heelsThought = "YOU, the player, are currently wearing high heels. You already have some experience with them, but you are still a High Heels Novice, so they still slow you down a bit. It will take maybe another day or two until you get the hang of them and can move a bit faster in them. Say as much in your response.";
				break;
			case YpsHeelTrainingStatus::flexible_feet:
				heelsThought = "YOU, the player, have been wearing high heels quite a while now. Your feet are now bent to the shape of the heels. That means you are just as clumsy without the heels now as you are with the heels. You might just as well continue your high heels training, so that you will be able to move faster again once your feet reach an even more permanently arched state. Say as much in your response.";
				break;
			case YpsHeelTrainingStatus::high_heel_walker:
				heelsThought = "YOU, the player, have been wearing high heels quite a while now. Your feet are now bent to the shape of the heels, more than they are adapted to flat shoes. That means you are now more clumsy without the heels than you are with the heels. You need to continue walking in high heels, because this is a bit faster now, and going without high heels will just slow you down. When you train your feet even more with high heels, you will be faster still. Say as much in your response.";
				break;
			case YpsHeelTrainingStatus::arched_feet:
				heelsThought = "YOU, the player, have trained in high heels so extensively that your feet have become permanently arched. Walking without heels now feels awkward and difficult because your feet are more comfortable in a raised position. Describe how this physical change and dependence on high heels makes you feel. Be sure to mention your arched feet explicitly so the reason for the thought is clear.";
				break;
			case YpsHeelTrainingStatus::bondage_feet:
				heelsThought = "YOU, the player, have completed your high heel training and your feet have become extremely and permanently arched. Walking barefoot or in ordinary high heels is now difficult; only very steep bondage boots properly support the shape of your feet. Describe how this severe physical change and dependence on bondage boots makes you feel. Be sure to mention your bondage feet explicitly so the reason for the thought is clear.";
				break;
			case YpsHeelTrainingStatus::unknown:
				break;
			}

			if (heelsThought) {
				DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(heelsThought);
			}
		}
	}
}
