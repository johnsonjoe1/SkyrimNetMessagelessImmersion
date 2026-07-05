Scriptname SNMI_Papyrus_Bridge_Script extends Quest  
{SNMI_Papyrus_Bridge_Script}

float keepalive_value = 1.01
float current_milk_value = 15.5
float max_milk_value = 15.5
float current_lactacid = 13.3
float max_lactacid = 13.3
float mme_maid_level = 1.0
string milk_string = "No milk_string defined yet!"

float LVSK_Euphoria = 0.0
float LVSK_IsLovesick = 0.0


; ypsPiercingTicker Property PETicker Auto
; 
; int Function GetYpsFashionLevel()
;    if PETicker
;        return PETicker.CurrentFashionLevel()
;    endif
;    return 0
; EndFunction



Event OnInit()
    RegisterForSingleUpdate(10.0)
	Debug.Notification("[SNMI] INITIAL ONINIT FOR THE Periodic 10 second update FINISHED.")
EndEvent

;Appropos
;--------------------------------------

ReferenceAlias Function GetAproposAlias(Actor akTarget, Quest apropos2Quest ) Global
	; Search Apropos2 actor aliases as the player my_alias is not set in stone
	ReferenceAlias aproposTwoAlias = None
	Int i = 0
	ReferenceAlias AliasSelect
	Int aliasesInt = apropos2Quest.GetNumAliases() 
	;slw_log.WriteLog("Apropos2 actor count" + aliasesInt)
	While i < aliasesInt 
		AliasSelect = apropos2Quest.GetNthAlias(i) as ReferenceAlias
		If AliasSelect.GetReference() as Actor == akTarget
			;slw_log.WriteLog("Apropos2 player found")
			aproposTwoAlias = AliasSelect
			Return aproposTwoAlias
		EndIf
		i += 1
	EndWhile

	;if aproposTwoAlias == None
		;String akActorName = akTarget.GetLeveledActorBase().GetName()
		;slw_log.WriteLog("Actor "+ akActorName + " is not yet registered in Apropos2")
	;EndIf
	Return aproposTwoAlias
EndFunction
	

Int Function GetWearStateAnal(Actor akTarget,  Quest apropos2Quest) Global
	ReferenceAlias aproposTwoAlias = GetAproposAlias(akTarget, apropos2Quest) 
	if aproposTwoAlias != None
		Int damage =  (aproposTwoAlias as Apropos2ActorAlias).AnalWearTearState - 1
		If damage < 0
			return 0
		Elseif  damage <= 8
			return damage
		else
			return 8
		EndIf
	Else
		return 0
	Endif
EndFunction

Int Function GetWearStateVaginal(Actor akTarget,  Quest apropos2Quest) Global 
	ReferenceAlias aproposTwoAlias = GetAproposAlias(akTarget, apropos2Quest)
	if aproposTwoAlias != None
		Int damage = (aproposTwoAlias as Apropos2ActorAlias).VaginalWearTearState - 1
		If damage < 0
			return 0
		Elseif  damage <= 8
			return damage
		else
			return 8
		EndIf
	Else
		return 0
	Endif
EndFunction

Int Function GetWearStateOral(Actor akTarget, Quest apropos2Quest) Global
	ReferenceAlias aproposTwoAlias = GetAproposAlias(akTarget, apropos2Quest)
	if aproposTwoAlias != None
		Int damage = (aproposTwoAlias as Apropos2ActorAlias).OralWearTearState - 1
		If damage < 0
			return 0
		Elseif  damage <= 8
			return damage
		else
			return 8
		EndIf
	Else
		return 0
	Endif
EndFunction


Function TestApropos()
    Actor player = Game.GetPlayer()
	Quest aproposQuest = Quest.GetQuest("Apropos2Actors")
	if aproposQuest == None
		Debug.Notification("SNMI:  SNMI_Papyrus_Brige_Script.psc:  Apropos quest not found")
		return
	endif
	ReferenceAlias my_alias = GetAproposAlias(Game.GetPlayer(), aproposQuest)
    if my_alias == None
        Debug.Notification("SNMI:  SNMI_Papyrus_Brige_Script.psc:  No Apropos my_alias found")
        Debug.Trace("[SNMI] No Apropos my_alias found")
        return
    endif
    ; Debug.Notification("SNMI:  SNMI_Papyrus_Brige_Script.psc:  Alias found")
    Apropos2ActorAlias ap = my_alias as Apropos2ActorAlias
    if ap == None
        Debug.Notification("SNMI:  SNMI_Papyrus_Brige_Script.psc:  Cast failed")
        Debug.Trace("[SNMI] Cast failed")
        return
    endif
    ; Debug.Notification("SNMI:  SNMI_Papyrus_Brige_Script.psc:  Cast OK")
    Debug.Trace("[SNMI] Vaginal  = " + ap.VaginalWearTearState)
    Debug.Trace("[SNMI] Anal     = " + ap.AnalWearTearState)
    Debug.Trace("[SNMI] Oral     = " + ap.OralWearTearState)
	; Final status can be printed as a notification for now.
	if Game.GetPlayer().GetLeveledActorBase().GetName() == "Lillith"
    	Debug.Notification("Apropos-Status:  V:" + ap.VaginalWearTearState + " A:" + ap.AnalWearTearState + " O:" + ap.OralWearTearState)
	endif
EndFunction

Function TestANDFlashClothing()
	; Here we query the current flash-clothing-keywords

endfunction


function push_all_MME_variables_to_the_plugin()
    current_milk_value =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkCount")      ; FROM MME_Storage.psc
    max_milk_value     =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkMaximum")    ; FROM MME_Storage.psc
    current_lactacid   =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.LactacidCount")  ; FROM MME_Storage.psc
	max_lactacid       = (StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.Level") + 2) / 2 + 4           ; FROM MME_Storage.psc
	mme_maid_level     =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.Level")           ; FROM MME_Storage.psc:  NOTE THAT THIS is stored as a float, even though only int values make sense
	; Beware:
	;   'MaidLevel' is stored as a float value for historical reasons,
	;   but fractional values are invalid and potentially break things.
	;    -> Provide an integer value instead.
	; int function getMaidLevel(actor akActor) global
	; 	Debug.Trace("MME_Storage: Triggered getMaidLevel() for actor " + akActor.GetLeveledActorBase().GetName())
	; 	return StorageUtil.GetFloatValue(akActor, "MME.MilkMaid.Level") as int
	; endfunction

    Int MilkCnt = Math.Floor(current_milk_value)
	Int MilkMax = Math.Floor(max_milk_value)
    Int MilkStageStrings = JsonUtil.StringListCount("/MME/Strings_Milkstage", "milkstage") - 1

    if MilkCnt >= MilkMax 
        milk_string = (formatString(JsonUtil.StringListGet("/MME/Strings_Milkstage", "milkstage", MilkStageStrings), Game.GetPlayer().GetLeveledActorBase().GetName()))
    elseif MilkCnt == (MilkMax - 1)
        milk_string = (formatString(JsonUtil.StringListGet("/MME/Strings_Milkstage", "milkstage", MilkStageStrings - 1), Game.GetPlayer().GetLeveledActorBase().GetName()))
    elseif MilkCnt == (MilkMax - 2)
        milk_string = (formatString(JsonUtil.StringListGet("/MME/Strings_Milkstage", "milkstage", MilkStageStrings - 2), Game.GetPlayer().GetLeveledActorBase().GetName()))
    else
        milk_string = (formatString(JsonUtil.StringListGet("/MME/Strings_Milkstage", "milkstage", MilkCnt), Game.GetPlayer().GetLeveledActorBase().GetName()))
    endif

    ; Let's try to get that milk stage from the MME mod, without any overhead
    ; milk_string = JsonUtil.StringListGet("/MME/Strings_Milkstage", "milkstage", MilkCnt)
    ; debug.Notification(formatString(milk_string, Game.GetPlayer().GetLeveledActorBase().GetName()))
    ; debug.Notification(milk_string)

    SNMI_Native.SetMilkLevel(current_milk_value)
    SNMI_Native.SetMilkMax(max_milk_value)
	SNMI_Native.SetLactacidLevel(current_lactacid)
	SNMI_Native.SetLactacidMax(max_lactacid)
    SNMI_Native.SetMilkString(milk_string)
	SNMI_Native.SetMaidLevel(mme_maid_level)

    if False ;  Game.GetPlayer().GetLeveledActorBase().GetName() == "Lillith"
         Debug.Notification("[SNMI] 10-sec-update: cur_milk: " + current_milk_value + ", max_milk: " + MilkMax + ", cur_lactacid: " + current_lactacid + ", mme_maid_level: " + mme_maid_level )
	Else
        Debug.Trace("[SNMI] 10-sec-update: cur_milk: " + current_milk_value + ", max_milk: " + MilkMax + ", cur_lactacid: " + current_lactacid + ", mme_maid_level: " + mme_maid_level )
    endif 

endfunction


function push_all_YPS_variables_to_the_plugin()

	; We take the list of current YPS-Conditions (for thoughts) and put them all into a single string, so that we can then push it to the C++ plugin
	int count = StorageUtil.StringListCount(None, "ypsActiveConditionsList")
	; Debug.Notification("Conditions: " + count)
	string allConditions = ""
	int i = 0
	while i < count
		string condition = StorageUtil.StringListGet(None, "ypsActiveConditionsList", i)
		Debug.Trace("YPS Condition[" + i + "] = " + condition)
		; For testing:
		; Debug.Notification(condition)
		if i > 0
			allConditions += "|"
		endif
		allConditions += condition
		i += 1
	endWhile	
	; Debug.Notification("Final YPS condition list: " + allConditions )
	Debug.Trace("[SNMI]  Final YPS condition list: " + allConditions )
    SNMI_Native.SetYpsConditionString(allConditions)

	float yps_AddictionLevel = StorageUtil.GetIntValue(None, "yps_AddictionLevel") ; current Fashion Addiction level (0-11)
	float yps_AddictionBuff = StorageUtil.GetIntValue(None, "yps_AddictionBuff")   ; level of current Fashion Addiction buff

	float yps_HeelsWorn = StorageUtil.GetIntValue(None, "ypsHeelsWorn")            ;

	SNMI_Native.set_yps_AddictionLevel(yps_AddictionLevel)
	SNMI_Native.set_yps_AddictionBuff(yps_AddictionBuff)	

	if Game.GetPlayer().GetLeveledActorBase().GetName() == "Lillith"
		Debug.Notification("YPS Addiction Buff: " + yps_AddictionBuff)
		Debug.Notification("YPS Heels Worn: " + yps_HeelsWorn)
    endif 



	; NOTE:  The Fashion buff levels are:  3=well fashioned.
endfunction

function push_lovesick_variables_to_the_plugin()
	; Let's also track the lovesickness exposed variables
	LVSK_IsLovesick = StorageUtil.GetIntValue(Game.GetPlayer(), "LVSK_IsLovesick", 0); 1 = true, 0 = false
	LVSK_Euphoria = StorageUtil.GetFloatValue(Game.GetPlayer(), "LVSK_Euphoria", 0.0); percentage, updated every game hour

    ; Only the DEBUG-Player will receive these extra notifications, so as to make them invisible for players in the release.
    if Game.GetPlayer().GetLeveledActorBase().GetName() == "Lillith"
         ; Debug.Notification("[SNMI] 10-sec-update: Counter: " + Math.Floor(keepalive_value) + ", Lovesick: " + LVSK_IsLovesick + ", Euphoria: " + LVSK_Euphoria)
		 Debug.Trace("[SNMI] 10-sec-update: Counter: " + Math.Floor(keepalive_value) + ", Lovesick: " + LVSK_IsLovesick + ", Euphoria: " + LVSK_Euphoria)
	Else
        Debug.Trace("[SNMI] 10-sec-update: Counter: " + Math.Floor(keepalive_value) + ", Lovesick: " + LVSK_IsLovesick + ", Euphoria: " + LVSK_Euphoria)
    endif 
	SNMI_Native.set_lovesickness_flag(LVSK_IsLovesick)
	SNMI_Native.set_lovesickness_euphoria(LVSK_Euphoria)

endfunction

Event OnUpdate()

    keepalive_value += 1.0    ; This is just an internal counter, that will count the number of times this has run so far
    SNMI_Native.SetKeepaliveLevel(keepalive_value)

	push_all_MME_variables_to_the_plugin()

	push_all_YPS_variables_to_the_plugin()

	push_lovesick_variables_to_the_plugin()
	    
	TestApropos()

	TestANDFlashClothing()

    RegisterForSingleUpdate(10.0)  ; Restart the same function in 10 seconds
EndEvent



float function getMilkCurrent(actor akActor) global
	; Debug.Trace("MME_Storage: Triggered getMilkCurrent() for actor " + akActor.GetLeveledActorBase().GetName())
	return StorageUtil.GetFloatValue(akActor, "MME.MilkMaid.MilkCount")
		
	Debug.Trace("[SNMI] SNMI-MME_Storage-Version:::: Triggered getMilkCurrent() for actor " + akActor.GetLeveledActorBase().GetName())

endfunction

float function getMilkMaximum(actor akActor) global
;	Debug.Trace("MME_Storage: Triggered getMilkMaximum() for actor " + akActor.GetLeveledActorBase().GetName())
;
;	; intentionally not using 'missing = ...' in order to avoid computing
;	; a default value that's immediately thrown away in almost every case
;	if StorageUtil.HasFloatValue(akActor, "MME.MilkMaid.MilkMaximum")
;		return StorageUtil.GetFloatValue(akActor, "MME.MilkMaid.MilkMaximum")
;	else
;		; should only trigger when upgrading an existing savegame
;		Debug.Trace("MME_Storage.getMilkMaximum(): Reinitializing default value for actor " + akActor.GetLeveledActorBase().GetName())
;		float MaidLevel = StorageUtil.GetFloatValue(akActor, "MME.MilkMaid.Level")
;		float MilkMax   = calculateMilkLimit(akActor, MaidLevel)
;		StorageUtil.SetFloatValue(akActor, "MME.MilkMaid.MilkMaximum", MilkMax)
;		return MilkMax
;	endif
endfunction



;  THIS FUNCTION IS COPIED 1:1 FROM MILKQUEST.psc, TO AVOID DEPENDENCY ON THEIR FILES.
String Function formatString(String src, String part1 = "", String part2 = "", String part3 = "", String part4 = "", String part5 = "")
	;Debug.Messagebox("json source: " + src)
	int pos1 = StringUtil.find(src, "%text1")
	if pos1 == 0
		src = StringUtil.substring("", 0, pos1) + part1 + StringUtil.substring(src, pos1+6)
	elseif pos1 != -1
		src = StringUtil.substring(src, 0, pos1) + part1 + StringUtil.substring(src, pos1+6)
	endIf
	int pos2 = StringUtil.find(src, "%text2")
	if pos2 != -1
		src = StringUtil.substring(src, 0, pos2) + part2 + StringUtil.substring(src, pos2+6)
	endIf
	int pos3 = StringUtil.find(src, "%text3")
	if pos3 != -1
		src = StringUtil.substring(src, 0, pos3) + part3 + StringUtil.substring(src, pos3+6)
	endIf
	int pos4 = StringUtil.find(src, "%text4")
	if pos4 != -1
		src = StringUtil.substring(src, 0, pos4) + part4 + StringUtil.substring(src, pos4+6)
	endIf
	int pos5 = StringUtil.find(src, "%text5")
	if pos5 != -1
		src = StringUtil.substring(src, 0, pos5) + part5 + StringUtil.substring(src, pos5+6)
	endIf
	return src
EndFunction

