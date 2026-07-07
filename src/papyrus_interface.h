#pragma once

#include <string>   //  ChatGPT suggested this might be needed?????

// ****************************************************************************************************************
//  Now some utility stuff:  The basic message dumping and queuing for thoughts occurs in different classes, so we refactor it onto a new class here.
class SNMIPapyrus
{
public:
    static bool Register(RE::BSScript::IVirtualMachine*);
    static void SetMilkLevel(RE::StaticFunctionTag*, float a_value);
	static void SetMilkMax(RE::StaticFunctionTag*, float a_value);
	static void SetLactacidLevel(RE::StaticFunctionTag*, float a_value);
	static void SetLactacidMax(RE::StaticFunctionTag*, float a_value);
	static void SetMilkString(RE::StaticFunctionTag*, std::string a_value);
	static void SetMaidLevel(RE::StaticFunctionTag*, float a_value);

	static void set_yps_AddictionLevel(RE::StaticFunctionTag*, float a_value);
	static void set_yps_AddictionBuff(RE::StaticFunctionTag*, float a_value);	
	static void set_yps_HeelsWorn(RE::StaticFunctionTag*, float a_value);

	static void SetYpsConditionString(RE::StaticFunctionTag*, std::string a_value);

	static void set_Apropos2Vstate(RE::StaticFunctionTag*, float a_value);
	static void set_Apropos2Astate(RE::StaticFunctionTag*, float a_value);
	static void set_Apropos2Ostate(RE::StaticFunctionTag*, float a_value);

	static void set_lovesickness_flag(RE::StaticFunctionTag*, float a_value);
	static void set_lovesickness_euphoria(RE::StaticFunctionTag*, float a_value);

    static float GetMilkLevel();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
	static float GetMilkMax();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
	static float GetMaidLevel();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.

    static std::string GetMilkString();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.
    static void SetKeepaliveLevel(RE::StaticFunctionTag*, float a_value);
    static float GetKeepaliveLevel();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.

	static std::string GetYpsConditionString();    // make this static, so that we can call it without an instance of the class SNMIPapyrus.


	static void handle_mme_milk_value_changes_and_produce_thoughts_from_them();

	inline static float previous_milk_level = 0;
	inline static float previous_lactacid_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
	inline static float previous_lactacid_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
	inline static float previous_milk_max_level = 1000000;  // simply don't speak of an increase at game start (given no saved values from previous save)
	inline static float previous_maid_level = -1.0f; 

	inline static float previous_yps_AddictionLevel = 0;
	inline static float previous_yps_AddictionBuff = 0;
	inline static float previous_yps_HeelsWorn = 0;

	private:
    static inline float _milkLevel = 124356.0f;
	static inline float _milkMax = 123456.0f;
	static inline float _lactacidLevel = 0.0f;
	static inline float _lactacidMax = 0.0f;
	static inline std::string _milkString = "No milk string defined IN PLUGIN yet!";
	static inline float _keepaliveLevel = 0.0f;
	static inline float _maidLevel = 1.0f;

	static inline float _yps_AddictionLevel = 0;
	static inline float _yps_AddictionBuff = 0;
	static inline float _yps_HeelsWorn = 0;
	
    static inline float _Apropos2Vstate = 0;
    static inline float _Apropos2Astate = 0;
    static inline float _Apropos2Ostate = 0;

	static inline float _lovesickness_flag=0;
	static inline float previous_lovesickness_flag=0;
	static inline float _lovesickness_euphoria=0;

	static inline std::string _yps_condition_string = "No yps condition string defined IN PLUGIN yet!";
};

