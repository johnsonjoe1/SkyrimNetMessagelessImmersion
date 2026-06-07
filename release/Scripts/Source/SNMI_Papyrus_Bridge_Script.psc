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
int LVSK_IsLovesick = 0

Event OnInit()
    RegisterForSingleUpdate(10.0)
	Debug.Notification("[SNMI] INITIAL ONINIT FOR THE Periodic 10 second update FINISHED.")

EndEvent

Event OnUpdate()

    keepalive_value += 1.0
    current_milk_value =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkCount")      ; FROM MME_Storage.psc
    max_milk_value     =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkMaximum")    ; FROM MME_Storage.psc
    current_lactacid   =  StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.LactacidCount")  ; FROM MME_Storage.psc
	max_lactacid       = (StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.Level") + 2) / 2 + 4           ; FROM MME_Storage.psc
	mme_maid_level     = StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.Level")           ; FROM MME_Storage.psc:  NOTE THAT THIS is stored as a float, even though only int values make sense
	; Beware:
	;   'MaidLevel' is stored as a float value for historical reasons,
	;   but fractional values are invalid and potentially break things.
	;    -> Provide an integer value instead.
	; int function getMaidLevel(actor akActor) global
	; 	Debug.Trace("MME_Storage: Triggered getMaidLevel() for actor " + akActor.GetLeveledActorBase().GetName())
	; 	return StorageUtil.GetFloatValue(akActor, "MME.MilkMaid.Level") as int
	; endfunction

	; Let's also track the lovesickness exposed variables
	LVSK_IsLovesick = StorageUtil.GetIntValue(Game.GetPlayer(), "LVSK_IsLovesick", 0); 1 = true, 0 = false
	LVSK_Euphoria = StorageUtil.GetFloatValue(Game.GetPlayer(), "LVSK_Euphoria", 0.0); percentage, updated every game hour


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
    SNMI_Native.SetKeepaliveLevel(keepalive_value)
	SNMI_Native.SetMaidLevel(mme_maid_level)

    ; Only the DEBUG-Player will receive these extra notifications, so as to make them invisible for players in the release.
    if Game.GetPlayer().GetLeveledActorBase().GetName() == "Lillith"
         Debug.Notification("[SNMI] 10-sec-update: cur_milk: " + current_milk_value + ", max_milk: " + MilkMax + ", cur_lactacid: " + current_lactacid + ", mme_maid_level: " + mme_maid_level + ", Counter: " + Math.Floor(keepalive_value) + ", Lovesick: " + LVSK_IsLovesick + ", Euphoria: " + LVSK_Euphoria)
	Else
        Debug.Trace("[SNMI] 10-sec-update: cur_milk: " + current_milk_value + ", max_milk: " + MilkMax + ", cur_lactacid: " + current_lactacid + ", mme_maid_level: " + mme_maid_level + ", Counter: " + Math.Floor(keepalive_value) + ", Lovesick: " + LVSK_IsLovesick + ", Euphoria: " + LVSK_Euphoria)
    endif 
    
    RegisterForSingleUpdate(10.0)
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

