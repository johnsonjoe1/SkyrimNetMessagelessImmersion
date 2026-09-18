#pragma once

#include "log.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DumpThoughts.h"
#include "handle_yps.h"
#include "misc.h"
#include "papyrus_interface.h"
#include "handle_config_ini_file.h"
#include <algorithm>
#include <array>
#include <unordered_set>
#include <optional>
#include <string_view>

namespace logger = SKSE::log;
bool actually_wearing_heels_according_to_yps_thoughts = false;

namespace
{
	struct YpsFashionState
	{
		std::string colour_or_item;
		int state;

		bool operator==(const YpsFashionState&) const = default;
	};

	constexpr std::array<std::string_view, 21> yps_hair_stage_names{
		"bald", "short stubbles", "long stubbles", "cropped", "ear length", "chin length", "neck length",
		"shoulder length", "armpit length", "bra strap length", "mid-back length", "waist length", "hip length",
		"tailbone length", "classic length", "mid-thigh length", "knee length", "calf length", "ankle length",
		"floor length", "Rapunzel length"
	};

	std::optional<int> previous_yps_hair_stage;
	bool yps_hair_redye_reminder_active = false;
	std::optional<YpsFashionState> previous_yps_lipstick_state;
	std::optional<YpsFashionState> previous_yps_eyeshadow_state;
	std::optional<YpsFashionState> previous_yps_fingernail_state;
	std::optional<YpsFashionState> previous_yps_toenail_state;
	std::optional<YpsFashionState> previous_yps_stockings_state;
	std::array<std::optional<bool>, 13> previous_yps_piercing_states;
	std::array<std::optional<bool>, 4> previous_yps_care_product_states;

	void throw_out_yps_fashion_thought(const std::string& thought)
	{
		LillithOnlyBox(thought);
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(thought);
	}

	std::string nail_polish_thought(bool fingernails, int previous_stage, int stage, const std::string& colour)
	{
		const auto nails = fingernails ? "fingernails" : "toenails";
		const auto professional_service = fingernails ? "manicure" : "pedicure";
		const auto colour_description = colour.empty() ? std::string("polish") : std::format("{} polish", colour);
		switch (stage) {
		case 0:
			if (previous_stage == -10) {
				return "YOU, the player, have just noticed that your damaged fingernails have finally recovered. They can safely be polished or manicured again. Be sure to mention your recovered fingernails explicitly and describe how you feel about being able to care for them normally again.";
			}
			return std::format("YOU, the player, have just noticed that no polish remains on your {} now, whether it was removed or finally chipped away. Notice how bare they look and describe what you think and feel about no longer wearing nail polish there. Be sure to mention your {} explicitly so the reason for the thought is clear.", nails, nails);
		case 20:
			return std::format("YOU, the player, have just finished applying {} to your {}. The fresh polish is still wet and can easily smudge, so you should avoid moving around until it dries. Notice how it looks and let us know how you feel about it.", colour_description, nails);
		case 30:
			return std::format("YOU, the player, have just noticed that the fresh {} on your {} has become visibly smudged, probably because you moved before it dried. React to the spoiled finish and be sure to mention your {} explicitly.", colour_description, nails, nails);
		case 40:
			return std::format("YOU, the player, have just noticed that the {} on your {} has completely dried. You can move normally again without worrying about smudging it. Admire the finished result and mention your {} explicitly.", colour_description, nails, nails);
		case 50:
			return std::format("YOU, the player, now have a finished professional {} on your {} with {}. Notice the durable, carefully finished result and describe how it makes you feel. Be sure to mention your {} explicitly.", professional_service, nails, colour_description, nails);
		case 54:
			return "YOU, the player, have just noticed that the artificial tips of your professionally manicured fingernails are cracked or broken. You now need a refill at a nail salon before the damage becomes worse. Be sure to mention your broken fingernail tips explicitly and react to the spoiled manicure.";
		case 60:
			return std::format("YOU, the player, have just noticed that the {} on your {} is beginning to chip. It is still present, but the finish no longer looks fresh. React to the deterioration and mention your {} explicitly.", colour_description, nails, nails);
		case 70:
			return std::format("YOU, the player, have just noticed that only untidy remnants of {} remain on your {}. They will not disappear cleanly on their own, so you should use nail-polish remover. Mention your {} explicitly and let us know how the neglected finish makes you feel.", colour_description, nails, nails);
		case -10:
			return "YOU, the player, have just noticed that your fingernails are scratched and damaged after losing their artificial nail tips. They need time to recover, and you cannot safely polish them again yet. Be sure to mention your damaged fingernails explicitly and consider visiting a nail salon.";
		default:
			return {};
		}
	}
}

struct ParsedCondition
{
    std::string name;
    std::optional<int> value;
};
ParsedCondition ParseCondition(const std::string& str)
{
	size_t pos = str.size();
	while (pos > 0 &&  (std::isdigit(static_cast<unsigned char>(str[pos - 1])) || (str[pos - 1] == '-'))) {
		--pos;
	}
	ParsedCondition result;
	result.name = str.substr(0, pos);
	if (pos < str.size()) {
		result.value = std::stoi(str.substr(pos));
	}
	return result;
}
std::unordered_set<std::string> ParseConditions(const std::string& str)
{
	actually_wearing_heels_according_to_yps_thoughts = false;
	std::unordered_set<std::string> result;
	size_t start = 0;
	while (start < str.size()) {
		size_t end = str.find('|', start);
		if (end == std::string::npos) {
			end = str.size();
		}
		if (end > start) {

			std::string current_substring;
			current_substring = str.substr(start, end - start);
			// SKSE::log::info("YPS-String-Parsing:  {}", current_substring);
			result.emplace(current_substring);

			// Let's handle some special cases or specially interesting variables
			auto c = ParseCondition(current_substring);

			//	SKSE::log::info("YPS-String-Parsing:  {}", str);

			std::string value_info;
			if (c.value) {
				value_info = std::format("Value = {}", *c.value);
				// logger::info("Value = {}", *c.value);
			} else { value_info = std::format("Value = missing / no value supplied"); }
			SKSE::log::info("YPS-String-Parsing: {} name: {} value: {}", current_substring, c.name, value_info);

			if (c.name == "ypsHeelsWorn") {
				actually_wearing_heels_according_to_yps_thoughts = true;
			}
		}
		start = end + 1;
	}
	return result;
}


// ****************************************************************************************************************
//  We handle changes of the yps-Status here.
//
//  Note to self:  static keyword only belongs in the header, not in the .cpp file.
//  Note to self:  public: private: keywords only belong in the header, not in the .cpp file.  

void handle_yps::reset_hair_stage_tracking()
{
	previous_yps_hair_stage.reset();
}

void handle_yps::reset_hair_dye_tracking()
{
	yps_hair_redye_reminder_active = false;
}

void handle_yps::reset_fashion_tracking()
{
	previous_yps_lipstick_state.reset();
	previous_yps_eyeshadow_state.reset();
	previous_yps_fingernail_state.reset();
	previous_yps_toenail_state.reset();
	previous_yps_stockings_state.reset();
	previous_yps_piercing_states.fill(std::nullopt);
	previous_yps_care_product_states.fill(std::nullopt);
}

void handle_yps::handle_yps_fashion_detection_stuff()
{
	auto* player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		logger::info("SEVERE ERROR: Querying the player failed in the handle_yps_fashion_detection_stuff function!!");
		return;
	}
	// Now, PERIODICALLY, we take care of changes in the values of other mods.  We put this here, because this gets
	// triggered reasonably often.  Not clean, admittedly, but cleaning that up will have to come later.
	std::string current_yps_condition_string = SNMIPapyrus::GetYpsConditionString();
	logger::info("The lates YPS-condition-String was: {}" , current_yps_condition_string);
	SKSE::log::info("Current yps_condition_string: {}", current_yps_condition_string);  
	std::unordered_set<std::string> current_unorderd_yps_set = ParseConditions(current_yps_condition_string);
}


bool handle_yps::try_handle_yps_mod_stuff(const SKSE::ModCallbackEvent* a_event)
{

	// MOD EVENT:  YPS Thoughts:  Those will be pushed to the background channel.
	if ( (std::strcmp(a_event->eventName.c_str() , "YPS_ThoughtEvent") == 0)  ) {
		// std::string  thought_message = std::format(a_event->strArg.c_str());

		// While in the middle of a scene, a fashion thought is often out of place.  We stop them during scenes.
		if (player_is_in_a_SL_scene()) {
			SKSE::log::info("YPS-ThoughtEvent detected, but player is in a scene, so we will not process it.");
			return true;  // In this case it really was a YPS event and that means no further processing necessary in the main mod boadcast module.
		}

		if (!(SNMI::GetSettings().enableDirectPushOfYPSThoughtsToSkyrimNetPlayerThoughts)) {
			SKSE::log::info("YPS-ThoughtEvent detected, but direct push of YPS thoughts to SkyrimNet player thoughts is disabled, so we will not process it.");
			return true;  // If direct push is disabled, we do not handle the YPS thought here.
		}
		DumpThoughts::throw_out_AS_LITTERAL_AS_POSSIBLE_thought_message(a_event->strArg.c_str());   // this shouldn't be overdone, but the background code makes sure of that.
		return true;  // In this case it really was a YPS event and that means no further processing necessary in the main mod boadcast module.
	}

	if (std::strcmp(a_event->eventName.c_str(), "yps_AddictionBuffChange") == 0 ||
		std::strcmp(a_event->eventName.c_str(), "yps_AddictionLevel") == 0) {
		LillithOnlyBox(std::format("YPS mod event detected: {}", a_event->eventName.c_str()));
		SKSE::log::info("YPS addiction event {} detected; the synchronized Papyrus state push will produce the cause-aware thought.", a_event->eventName.c_str());
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "yps_HairDyeColourChange") == 0) {
		LillithOnlyBox("YPS mod event detected: yps_HairDyeColourChange");
		yps_hair_redye_reminder_active = false;
		if (a_event->strArg.empty()) {
			SKSE::log::warn("Ignoring yps_HairDyeColourChange event without a colour name.");
			return true;
		}

		const auto final_thought_string = std::format(
			"YOU, the player, have just had your hair dyed {}. Notice how the new colour changes your appearance and describe what you think and feel about it. Be sure to mention both your hair and its new {} colour explicitly so the reason for the thought is clear.",
			a_event->strArg.c_str(), a_event->strArg.c_str());
		SKSE::log::info("YPS reports that the player's hair was dyed {} (colour value {}).", a_event->strArg.c_str(), a_event->numArg);
		LillithOnlyBox(final_thought_string);
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "yps_ShouldRedyeHairEvent") == 0) {
		LillithOnlyBox("YPS mod event detected: yps_ShouldRedyeHairEvent");
		if (a_event->numArg != 0.0f && a_event->numArg != 1.0f) {
			SKSE::log::warn("Ignoring yps_ShouldRedyeHairEvent with invalid state: {}", a_event->numArg);
			return true;
		}

		const bool should_redye_hair = a_event->numArg == 1.0f;
		if (should_redye_hair && !yps_hair_redye_reminder_active) {
			const std::string final_thought_string =
				"YOU, the player, have just noticed that your dyed hair no longer looks freshly coloured. Either your natural-coloured roots are visibly growing out or your semi-permanent hair colour is beginning to fade. Think about how the change looks and whether you should visit a hairdresser to refresh the dye. Be sure to mention your fading or grown-out hair dye explicitly so the reason for the thought is clear.";
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string);
		}

		yps_hair_redye_reminder_active = should_redye_hair;
		SKSE::log::info("YPS hair-redye reminder is now {}.", should_redye_hair ? "active" : "inactive");
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "yps_CanRedyeHairEvent") == 0 ||
		std::strcmp(a_event->eventName.c_str(), "yps_CanChangeHairColourEvent") == 0) {
		LillithOnlyBox(std::format("YPS mod event detected: {}", a_event->eventName.c_str()));
		SKSE::log::info("YPS hair-colour capability event {} changed to {}; no thought generated.", a_event->eventName.c_str(), a_event->numArg);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "SNMI_YPSLipstickChange") == 0 ||
		std::strcmp(a_event->eventName.c_str(), "SNMI_YPSEyeShadowChange") == 0) {
		LillithOnlyBox(std::format("YPS mod event detected: {}", a_event->eventName.c_str()));
		const auto state = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(state) || state < 0 || state > 3) {
			SKSE::log::warn("Ignoring {} with invalid cosmetic state: {}", a_event->eventName.c_str(), a_event->numArg);
			return true;
		}

		const bool is_lipstick = std::strcmp(a_event->eventName.c_str(), "SNMI_YPSLipstickChange") == 0;
		auto& previous_state = is_lipstick ? previous_yps_lipstick_state : previous_yps_eyeshadow_state;
		const YpsFashionState current_state{ a_event->strArg.c_str(), state };
		if (previous_state && *previous_state == current_state) {
			SKSE::log::info("Ignoring duplicate {} state.", a_event->eventName.c_str());
			return true;
		}

		const bool had_previous_state = previous_state.has_value();
		previous_state = current_state;
		const bool worn = (state & 1) != 0;
		const bool smudged = (state & 2) != 0;
		if (!had_previous_state && !worn) {
			SKSE::log::info("Established initial absent {} state; no thought generated.", is_lipstick ? "lipstick" : "eyeshadow");
			return true;
		}

		const auto cosmetic = is_lipstick ? "lipstick" : "eyeshadow";
		const auto location = is_lipstick ? "lips" : "eyelids";
		const auto colour = current_state.colour_or_item.empty() ? std::string{} : std::format(" {}", current_state.colour_or_item);
		std::string final_thought_string;
		if (smudged) {
			final_thought_string = std::format(
				"YOU, the player, have just noticed that your{} {} is visibly smudged. React to the spoiled makeup and consider whether you need to fix it. Be sure to mention your {} explicitly so the reason for the thought is clear.",
				colour, cosmetic, cosmetic);
		} else if (!worn) {
			final_thought_string = std::format(
				"YOU, the player, have just removed or worn away your {}. Notice how bare your {} look without it and describe what you think and feel about no longer wearing it. Be sure to mention your {} explicitly so the reason for the thought is clear.",
				cosmetic, location, cosmetic);
		} else {
			final_thought_string = std::format(
				"YOU, the player, have just applied or refreshed your{} {}. Notice how the colour changes the appearance of your {} and describe what you think and feel about the result. Be sure to mention your {} explicitly so the reason for the thought is clear.",
				colour, cosmetic, location, cosmetic);
		}

		throw_out_yps_fashion_thought(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "SNMI_YPSFingerNailPolishChange") == 0 ||
		std::strcmp(a_event->eventName.c_str(), "SNMI_YPSToeNailPolishChange") == 0) {
		LillithOnlyBox(std::format("YPS mod event detected: {}", a_event->eventName.c_str()));
		const auto stage = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(stage)) {
			SKSE::log::warn("Ignoring {} with non-integral nail-polish stage: {}", a_event->eventName.c_str(), a_event->numArg);
			return true;
		}

		const bool fingernails = std::strcmp(a_event->eventName.c_str(), "SNMI_YPSFingerNailPolishChange") == 0;
		auto& previous_state = fingernails ? previous_yps_fingernail_state : previous_yps_toenail_state;
		const YpsFashionState current_state{ a_event->strArg.c_str(), stage };
		if (previous_state && *previous_state == current_state) {
			SKSE::log::info("Ignoring duplicate {} state.", a_event->eventName.c_str());
			return true;
		}

		const bool had_previous_state = previous_state.has_value();
		const int previous_stage = had_previous_state ? previous_state->state : 0;
		previous_state = current_state;
		if (!had_previous_state && stage == 0) {
			SKSE::log::info("Established initial unpolished {} state; no thought generated.", fingernails ? "fingernail" : "toenail");
			return true;
		}

		const auto final_thought_string = nail_polish_thought(fingernails, previous_stage, stage, current_state.colour_or_item);
		if (final_thought_string.empty()) {
			SKSE::log::info("YPS {} polish reached stage {}; no thought is configured for this intermediate or special stage.", fingernails ? "fingernail" : "toenail", stage);
			return true;
		}

		throw_out_yps_fashion_thought(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "SNMI_YPSStockingsChange") == 0) {
		LillithOnlyBox("YPS mod event detected: SNMI_YPSStockingsChange");
		const auto state = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(state) || state < 0 || state > 1) {
			SKSE::log::warn("Ignoring SNMI_YPSStockingsChange with invalid state: {}", a_event->numArg);
			return true;
		}

		const YpsFashionState current_state{ a_event->strArg.c_str(), state };
		if (previous_yps_stockings_state && *previous_yps_stockings_state == current_state) {
			SKSE::log::info("Ignoring duplicate YPS stockings state.");
			return true;
		}

		const bool had_previous_state = previous_yps_stockings_state.has_value();
		previous_yps_stockings_state = current_state;
		if (!had_previous_state && state == 0) {
			SKSE::log::info("Established initial absent YPS stockings state; no thought generated.");
			return true;
		}

		const auto final_thought_string = state == 1 ?
			std::format("YOU, the player, have just put on {}. Notice how the stockings look and feel on your legs and describe what you think about wearing them. Be sure to mention the stockings explicitly so the reason for the thought is clear.", current_state.colour_or_item.empty() ? "a pair of stockings" : current_state.colour_or_item) :
			std::string("YOU, the player, have just taken off your stockings. Notice how different your bare legs look and feel now and describe what you think about removing them. Be sure to mention the stockings explicitly so the reason for the thought is clear.");
		throw_out_yps_fashion_thought(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "SNMI_YPSPiercingChange") == 0) {
		LillithOnlyBox("YPS mod event detected: SNMI_YPSPiercingChange");
		const auto signed_slot = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(signed_slot) || signed_slot == 0 || signed_slot < -12 || signed_slot > 12) {
			SKSE::log::warn("Ignoring SNMI_YPSPiercingChange with invalid signed slot: {}", a_event->numArg);
			return true;
		}

		constexpr std::array<std::string_view, 13> piercing_slot_names{
			"unused", "earlobes", "left nostril", "septum", "snake bites", "right labret", "labret",
			"right eyebrow", "nose bridge", "navel", "nipples", "clitoris", "labia"
		};
		const bool equipped = signed_slot > 0;
		const auto slot = static_cast<std::size_t>(equipped ? signed_slot : -signed_slot);
		if (previous_yps_piercing_states[slot] && *previous_yps_piercing_states[slot] == equipped) {
			SKSE::log::info("Ignoring duplicate YPS piercing state for slot {} ({}).", slot, piercing_slot_names[slot]);
			return true;
		}

		const bool had_previous_state = previous_yps_piercing_states[slot].has_value();
		previous_yps_piercing_states[slot] = equipped;
		if (!had_previous_state && !equipped) {
			SKSE::log::info("Established initial absent YPS piercing state for slot {} ({}); no thought generated.", slot, piercing_slot_names[slot]);
			return true;
		}

		const auto final_thought_string = equipped ?
			std::format("YOU, the player, have just put jewellery into the piercing at your {}. Notice the jewellery and how it feels there, and describe what you think about wearing it. Be sure to mention the {} piercing explicitly so the reason for the thought is clear.", piercing_slot_names[slot], piercing_slot_names[slot]) :
			std::format("YOU, the player, have just removed the jewellery from the piercing at your {}. Notice how the empty piercing feels and describe what you think about no longer wearing jewellery there. Be sure to mention the {} piercing explicitly so the reason for the thought is clear.", piercing_slot_names[slot], piercing_slot_names[slot]);
		throw_out_yps_fashion_thought(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str(), "SNMI_YPSCareProductChange") == 0) {
		const auto state = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(state) || state < 0 || state > 1) {
			SKSE::log::warn("Ignoring SNMI_YPSCareProductChange with invalid state: {}", a_event->numArg);
			return true;
		}

		const std::array<std::string_view, 4> product_names{ "LipBalm", "AntiAgingCream", "FingerNailOil", "Perfume" };
		auto product = std::find(product_names.begin(), product_names.end(), a_event->strArg.c_str());
		if (product == product_names.end()) {
			SKSE::log::warn("Ignoring SNMI_YPSCareProductChange for unknown product: {}", a_event->strArg.c_str());
			return true;
		}

		const auto product_index = static_cast<std::size_t>(std::distance(product_names.begin(), product));
		auto& previous_state = previous_yps_care_product_states[product_index];
		const bool applied = state == 1;
		if (!previous_state) {
			previous_state = applied;
			SKSE::log::info("Established initial YPS {} state as {}; no thought generated.", a_event->strArg.c_str(), applied);
			return true;
		}
		if (*previous_state == applied) {
			return true;
		}
		previous_state = applied;

		LillithOnlyBox(std::format("YPS mod event detected: SNMI_YPSCareProductChange ({})", a_event->strArg.c_str()));
		std::string final_thought_string;
		switch (product_index) {
		case 0:
			if (applied) {
				final_thought_string = "YOU, the player, have just gently spread lip balm over your lips. Notice their softer, smoother feeling and subtle sheen, and describe how this small act of care makes you feel. Be sure to mention the lip balm explicitly so the reason for the thought is clear.";
			} else if (previous_yps_lipstick_state && previous_yps_lipstick_state->state > 0) {
				final_thought_string = "YOU, the player, are no longer wearing lip balm because it has been replaced by lipstick. Notice the change from simple lip care to visible colour and describe how the more decorative look makes you feel. Be sure to mention both the lip balm and lipstick explicitly so the reason for the thought is clear.";
			} else {
				final_thought_string = "YOU, the player, have just noticed that your lip balm has faded away. Your lips no longer feel as freshly softened or protected, so consider whether you want to apply it again. Be sure to mention the faded lip balm explicitly so the reason for the thought is clear.";
			}
			break;
		case 1:
			if (applied) {
				final_thought_string = "YOU, the player, have just gently applied anti-aging cream around your eyes. Notice the cared-for feeling of the skin there and describe how tending to your appearance makes you feel. Be sure to mention the anti-aging cream explicitly so the reason for the thought is clear.";
			} else if (previous_yps_eyeshadow_state && previous_yps_eyeshadow_state->state > 0) {
				final_thought_string = "YOU, the player, are no longer wearing anti-aging cream because it has been replaced by eyeshadow. Notice the change from understated skin care to visible eye makeup and describe how the more decorative look makes you feel. Be sure to mention both the cream and eyeshadow explicitly so the reason for the thought is clear.";
			} else {
				final_thought_string = "YOU, the player, have just noticed that the anti-aging cream around your eyes has faded away. The cared-for effect no longer feels fresh, so consider whether you want to apply it again. Be sure to mention the faded anti-aging cream explicitly so the reason for the thought is clear.";
			}
			break;
		case 2:
			if (applied) {
				final_thought_string = "YOU, the player, have just gently spread nail oil over your fingernails. Notice their freshly cared-for sheen and smooth feeling, and describe how this simple manicure care makes you feel. Be sure to mention the fingernail oil explicitly so the reason for the thought is clear.";
			} else if (previous_yps_fingernail_state && previous_yps_fingernail_state->state > 0) {
				final_thought_string = "YOU, the player, are no longer wearing fingernail oil because it has been replaced by nail polish. Notice the change from a natural cared-for sheen to a visibly polished manicure and describe how it makes you feel. Be sure to mention both the nail oil and nail polish explicitly so the reason for the thought is clear.";
			} else {
				final_thought_string = "YOU, the player, have just noticed that the oil on your fingernails has faded away. They no longer have that freshly cared-for sheen, so consider whether you want to oil them again. Be sure to mention the faded fingernail oil explicitly so the reason for the thought is clear.";
			}
			break;
		case 3:
			final_thought_string = applied ?
				"YOU, the player, have just applied perfume to your body. Notice the fragrance surrounding you and describe how wearing this scent affects your mood and sense of style. Be sure to mention the perfume explicitly so the reason for the thought is clear." :
				"YOU, the player, have just noticed that your perfume has faded away. The fragrance that accompanied you is gone now, so consider whether you miss it or want to apply another scent. Be sure to mention the faded perfume explicitly so the reason for the thought is clear.";
			break;
		default:
			return true;
		}

		throw_out_yps_fashion_thought(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str() , "yps_HairStageChange") == 0) {			
		LillithOnlyBox("YPS mod event detected: yps_HairStageChange");
		const auto hair_stage = static_cast<int>(a_event->numArg);
		if (a_event->numArg != static_cast<float>(hair_stage) || hair_stage < 1 || hair_stage > static_cast<int>(yps_hair_stage_names.size())) {
			SKSE::log::warn("Ignoring yps_HairStageChange event with invalid stage: {}", a_event->numArg);
			return true;
		}

		if (!previous_yps_hair_stage) {
			previous_yps_hair_stage = hair_stage;
			SKSE::log::info("Established initial YPS hair stage {} ({}); no thought generated.", hair_stage, yps_hair_stage_names[hair_stage - 1]);
			return true;
		}

		if (*previous_yps_hair_stage == hair_stage) {
			SKSE::log::info("Ignoring duplicate yps_HairStageChange event for stage {} ({}).", hair_stage, yps_hair_stage_names[hair_stage - 1]);
			return true;
		}

		const auto previous_stage = *previous_yps_hair_stage;
		previous_yps_hair_stage = hair_stage;
		std::string final_thought_string;
		if (hair_stage > previous_stage) {
			final_thought_string = std::format(
				"YOU, the player, have just noticed that your hair has grown from {} to {}. Describe how the new length looks and feels, and let us know what you think about your visibly longer hair. Be sure to mention your hair explicitly so the reason for the thought is clear.",
				yps_hair_stage_names[previous_stage - 1], yps_hair_stage_names[hair_stage - 1]);
		} else {
			final_thought_string = std::format(
				"YOU, the player, have just noticed that your hair length has changed from {} to {}, probably because it was cut. Describe how the shorter style looks and feels, and let us know what you think about it. Be sure to mention your hair explicitly so the reason for the thought is clear.",
				yps_hair_stage_names[previous_stage - 1], yps_hair_stage_names[hair_stage - 1]);
		}

		SKSE::log::info("YPS hair stage changed from {} ({}) to {} ({}).", previous_stage, yps_hair_stage_names[previous_stage - 1], hair_stage, yps_hair_stage_names[hair_stage - 1]);
		LillithOnlyBox(final_thought_string);
		DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string);
		return true;
	}

	if (std::strcmp(a_event->eventName.c_str() , "yps_ArmpitHairStageChange") == 0) {			
		SKSE::log::info("YPS-ArmpitHairStageChange event detected.");
		if (a_event->numArg == 0) {
			std::string final_thought_string = std::format("YOU, the player, just noticed, that your armpit hair is completely gone, probably because you shaved it, which is MUCH more fashionable for a woman than having armpit hair.  Be sure to mention your armpit hair explicitly in your response, because the player wouldn't otherwise know what you are talking about, and let us know how the armpit hair situation makes you feel.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;
		} else {
			std::string final_thought_string = std::format("YOU, the player, just noticed, that your armpit hair has grown even more.  Out of the 5 stages from 0 = shaved to 5 = fully grown, you have now reached stage {}.  Describe the new state of your armpit hair in your response.  Be sure to mention armpit hair explicitly in your response, because the player wouldn't otherwise know what you are talking about, and let us know how the armpit hair situation makes you feel.", a_event->numArg);
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;		
		}
		return true;
	}
	
	if (std::strcmp(a_event->eventName.c_str() , "yps_PubicHairStageChange") == 0) {			
		SKSE::log::info("YPS-PubicHairStageChange event detected.");
		if (a_event->numArg == 0) {
			std::string final_thought_string = std::format("YOU, the player, just noticed, that your pubic hair is completely gone, probably because you shaved it, which is MUCH more fashionable for a woman than having pubic hair.  Be sure to mention your pubic hair explicitly in your response, because the player wouldn't otherwise know what you are talking about, and let us know how the pubic hair situation makes you feel.");
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;
		} else {
			std::string final_thought_string = std::format("YOU, the player, just noticed, that your pubic hair has grown even more.  Out of the 5 stages from 0 = shaved to 5 = fully grown, you have now reached stage {}.  Describe the new state of your pubic hair in your response.  Be sure to mention pubic hair explicitly in your response, because the player wouldn't otherwise know what you are talking about, and let us know how the pubic hair situation makes you feel.", a_event->numArg);
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_IMPORTANT_TTS_thought_message(final_thought_string); //  + standard_thought_instruction;
		}
		return true;
	}	




	if ( (std::strcmp(a_event->eventName.c_str() , "yps_FashionChange") == 0)  ) {			
		LillithOnlyBox(std::format("YPS mod event detected: yps_FashionChange ({})", a_event->strArg.c_str()));
		return true;
	}
	return false;

}


void handle_yps::handle_yps_magic_effect_stuff(const RE::TESActiveEffectApplyRemoveEvent* a_event, RE::ActiveEffect* effect)
{
	logger::info("*********************YPS-MAGIC-EFFECT-HANDLER STARTED!!!**********************************");
	// We got a magic effect.  We check once again everything, even though it has been checked in the calling function.

	auto* base = effect->GetBaseObject();
	auto* source = effect->spell;
	auto base_name = base->GetName();

	// For the protocol, we give some debug information.
	/* 	
	HeelTrainingStatusName[0] = "Untrained Feet"
	HeelTrainingStatusName[1] = "High Heel Novice"
	HeelTrainingStatusName[2] = "Flexible Feet"
	HeelTrainingStatusName[3] = "High Heel Walker"
	HeelTrainingStatusName[4] = "Arched Feet"
	HeelTrainingStatusName[5] = "Bondage Feet"  */

	SKSE::log::info("The MAGIC EFFECT Source->GetName() of the YPS-heels move penalty gave:  {} with magnitude:  {}", source->GetName(), effect->magnitude);

	// Let's try to track YPS-Fashion-High-Heels-slowdown-effect:  this gets activated and deactivated all the time, so we have to keep our messages in check
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "High Heel Novice") == 0)  ) )
	{
		if ( (a_event->isApplied) )
		{
			// Due to what seems like a bug to me, there will be slowdown effect even when not wearing heels.  In that case of course we don't complain about the heels.  So we have to catch that here.
			if ( (effect->magnitude < 0) && actually_wearing_heels_according_to_yps_thoughts) {
				SKSE::log::info("xxxxxxxxxxx High Heels Novice MAGIC EFFECT PROPERLY DETECTED");
				DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You already have some experience with them, but you are still a High Heels Novice, so they still slow you down a bit. It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			}
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		} else {  //  i.e. if (!a_event->isApplied) {
			// We do nothing here, as we just have stopped moving, nothing else.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of High Heels Novice MAGIC EFFECT.");
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Untrained Feet") == 0)  ) ) 
	{
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Untrained Feet' (high heels) MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS UNTRAINED FEET! ", base_name);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, are currently wearing high heels. You are totally untrained with high heels. You are not even a High Heels Novice yet. So they slow you down massively now.  It will take maybe another day or two until you get the hang of them and can move a bit faster in them.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
		if ( (!a_event->isApplied) ) {
			// We do nothing here, as we just have stopped moving, nothing else.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Untrained Feet MAGIC EFFECT.");
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Flexible Feet") == 0)  ) ) 
	{
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Flexible Feet' (high heels) MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS FLEXIBLE FEET! ", base_name);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, have been wearing high heels quite a while now. Your feet are now bent to the shape of the heels.  That means you are just as clumsy without the heels now than you are with the heels.  This means you might just as well continue your high heels training, so that you will be able to move faster again once your feet permanently reach even more permanently arched state.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
		if ( (!a_event->isApplied) ) {
			// We do nothing here, as we just have stopped moving, nothing else.
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Flexible Feet MAGIC EFFECT.");
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "High Heel Walker") == 0)  ) ) 
	{
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'High Heel Walker' (high heels) MAGIC EFFECT PROPERLY DETECTED");
			std::string stomach_rot_status = std::format("{} Magic Event Effect Handler for YPS HIGH HEEL WALKER! ", base_name);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(std::format("YOU, the player, have been wearing high heels quite a while now. Your feet are now bent to the shape of the heels, more than they are adapted to flat shoes.  That means you are now more clumsy without the heels than you are with the heels.  This means you need to continue walking in high heels, because this is a bit faster now, and going without high heels will just slow you down.  And when you train your feet even more with high heels, you will be faster still.  Say as much in your response.")); //  + standard_thought_instruction;);   // this shouldn't be overdone, but hte background code makes sure of that.
			// NOTE:  Return-Control from Effect Handler will be done outside in the calling function!!!!   return RE::BSEventNotifyControl::kContinue;
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Arched Feet") == 0)  ) )
	{
		LillithOnlyBox("YPS magic effect detected: Arched Feet");
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Arched Feet' MAGIC EFFECT PROPERLY DETECTED");
			const std::string final_thought_string = "YOU, the player, have trained in high heels so extensively that your feet have become permanently arched. Walking without heels now feels awkward and difficult because your feet are more comfortable in a raised position. Describe how this physical change and dependence on high heels makes you feel. Be sure to mention your arched feet explicitly so the reason for the thought is clear.";
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(final_thought_string);
		}
		if ( (!a_event->isApplied) ) {
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Arched Feet MAGIC EFFECT.");
		}
	}
	if (base && ( (std::strcmp(base->GetName(), "Movement Speed Penalty") == 0)  ) && ( (std::strcmp(source->GetName(), "Bondage Feet") == 0)  ) )
	{
		LillithOnlyBox("YPS magic effect detected: Bondage Feet");
		if ((a_event->isApplied) && (effect->magnitude < 0) )
		{
			SKSE::log::info("xxxxxxxxxxx YPS 'Bondage Feet' MAGIC EFFECT PROPERLY DETECTED");
			const std::string final_thought_string = "YOU, the player, have completed your high heel training and your feet have become extremely and permanently arched. Walking barefoot or in ordinary high heels is now difficult; only very steep bondage boots properly support the shape of your feet. Describe how this severe physical change and dependence on bondage boots makes you feel. Be sure to mention your bondage feet explicitly so the reason for the thought is clear.";
			LillithOnlyBox(final_thought_string);
			DumpThoughts::throw_out_BACKGROUND_TTS_thought_message(final_thought_string);
		}
		if ( (!a_event->isApplied) ) {
			SKSE::log::info("xxxxxxxxxxx SKIPPING:  IT's REMOVAL of Bondage Feet MAGIC EFFECT.");
		}
	}


	logger::info("*********************YPS-MAGIC-EFFECT-HANDLER FINISHED!!!**********************************");
}
