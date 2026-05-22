Scriptname SNMI_Papyrus_Bridge_Script extends Quest  
{SNMI_Papyrus_Bridge_Script}

float keepalive_value = 1.01
float current_milk_value = 15.5
float max_milk_value = 15.5

Event OnInit()
    RegisterForSingleUpdate(10.0)
	Debug.Notification("[SNMI] INITIAL ONINIT FOR THE Periodic 10 second update FINISHED.")

EndEvent

Event OnUpdate()
    ; SNMI_Bridge.UpdateMilkLevel()


    keepalive_value += 1.0
    current_milk_value = StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkCount")
    max_milk_value     = StorageUtil.GetFloatValue(Game.GetPlayer(), "MME.MilkMaid.MilkMaximum")

    SNMI_Native.SetMilkLevel(current_milk_value)
    SNMI_Native.SetKeepaliveLevel(keepalive_value)
	Debug.Notification("[SNMI] Periodic 10 sec update: current_milk_value: " + current_milk_value + ", max_milk_value: " + max_milk_value + ", keepalive_value: " + keepalive_value)
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


