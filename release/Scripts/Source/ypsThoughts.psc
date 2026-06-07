Scriptname ypsThoughts extends Quest  
{YPS Player Thought Framework Script}

string ThoughtConditionsFilename = "../ypsThoughts/ypsThought_conditions.json"
string MaleThoughtDatabaseFilename = "../ypsThoughts/ypsThought_male_database.json"
string FemaleThoughtDatabaseFilename = "../ypsThoughts/ypsThought_female_database.json"

function Playthought(bool TruncateLongThoughts = false, bool SplitLongThoughts = true, bool MessageboxLongThoughts = true, int LongThoughtLimit = 80, float soundvolume = 0.5, bool playsubtitles = true)
; how to deal with thoughts that have more characters than <LongThoughtLimit>:
;	Truncate: show only the first <LongThoughtLimit> characters
;	Split: split them into 2 chunks (is overridden by 'truncate')
;	Messagebox: show them in a messagebox instead of notifications (is overridden by 'truncate' or 'split')
;
  if Game.IsLookingControlsEnabled() && !UI.IsMenuOpen("Dialogue Menu") && !(StorageUtil.GetIntValue(None, "ytfThoughtsDisabled") == 1 )  ; don't play anything during dialogue
	string ThoughtDatabaseFilename
	if Game.GetPlayer().GetActorBase().GetSex() == 1
		ThoughtDatabaseFilename = FemaleThoughtDatabaseFilename
	else
		ThoughtDatabaseFilename = MaleThoughtDatabaseFilename
	endif

;string AllConditions = "" ; for debugging: display all conditions seen by the script as valid
	Location kCurrentLoc = Game.GetPlayer().GetCurrentLocation()

	StorageUtil.StringListClear(none,"ypsActiveConditionsList")
	if !Game.GetPlayer().isincombat() ; no normal and loctype conditions shown in combat
		JsonUtil.Load(ThoughtConditionsFilename)
		int PapyrusvaluesCount = JsonUtil.StringListCount(ThoughtConditionsFilename,"thoughtpapyrusvalues")
		int j=0
		while j < PapyrusvaluesCount
			string PapyrusValue = JsonUtil.StringListGet(ThoughtConditionsFilename,"thoughtpapyrusvalues",j)
			if StorageUtil.GetIntValue(None, PapyrusValue) 
				if JsonUtil.StringListCount(ThoughtDatabaseFilename,PapyrusValue) >= 1
					StorageUtil.StringListAdd(none,"ypsActiveConditionsList",PapyrusValue)
;AllConditions += PapyrusValue + " "
				endif
			endif
			j += 1
		endwhile

		if (kCurrentLoc != none)
			int LocTypesCount = JsonUtil.StringListCount(ThoughtConditionsFilename,"thoughtlocationtypes")
			j = 0
			while j < LocTypesCount
				string LocTypeString = JsonUtil.StringListGet(ThoughtConditionsFilename,"thoughtlocationtypes",j)
				if JsonUtil.StringListCount(ThoughtDatabaseFilename,LocTypeString) >= 1
					Keyword LocationKW = GetLocTypeKeyword(LocTypeString)
					if kCurrentLoc.HasKeyword(LocationKW)
						StorageUtil.StringListAdd(none,"ypsActiveConditionsList",LocTypeString)
;AllConditions += LocTypeString + " "
					endif
				endif
				j += 1
			endwhile
		else
			StorageUtil.StringListAdd(none,"ypsActiveConditionsList","loctypewilderness")
;AllConditions += "loctypewilderness" + " "
		endif
		
		int WornKeywordsCount = JsonUtil.StringListCount(ThoughtConditionsFilename,"thoughtwornkeywords")
		j = 0
		while j < WornKeywordsCount
			string WornKeywordsString = JsonUtil.StringListGet(ThoughtConditionsFilename,"thoughtwornkeywords",j)
			if JsonUtil.StringListCount(ThoughtDatabaseFilename,WornKeywordsString) >= 1
				Keyword WornKW = Keyword.GetKeyword(StringUtil.Substring(WornKeywordsString,1)) ; remove the leading # sign
				if Game.GetPlayer().WornHasKeyword(WornKW)
					StorageUtil.StringListAdd(none,"ypsActiveConditionsList",WornKeywordsString)
;AllConditions += WornKeywordsString + " "
				endif
			endif
			j += 1
		endwhile

	endif
	
	int PlayerConditionsCount = JsonUtil.StringListCount(ThoughtConditionsFilename,"thoughtplayerconditions")
	int j = 0
	while j < PlayerConditionsCount
		string PlayerConditionString = JsonUtil.StringListGet(ThoughtConditionsFilename,"thoughtplayerconditions",j)
		if ((PlayerConditionString == "$isincombat") ||  !Game.GetPlayer().isincombat()) && (JsonUtil.StringListCount(ThoughtDatabaseFilename,PlayerConditionString) >= 1) && checkplayercondition(PlayerConditionString)
			StorageUtil.StringListAdd(none,"ypsActiveConditionsList",PlayerConditionString)
;AllConditions += PlayerConditionString + " "
		endif
		j += 1
	endwhile
	
	int ConditionsCount = JsonUtil.StringListCount(ThoughtConditionsFilename,"thoughtconditions")
	j = 0
	while j < ConditionsCount
		string ConditionsString = JsonUtil.StringListGet(ThoughtConditionsFilename,"thoughtconditions",j)
		if (!Game.GetPlayer().isincombat() || (StringUtil.Find(ConditionsString,"$isincombat") == 0)) && JsonUtil.StringListCount(ThoughtDatabaseFilename,ConditionsString) >= 1
			string ConditionsStringLeft = ConditionsString
			bool ConditionCheckRunning = true
			while ConditionCheckRunning && (ConditionsStringLeft != "")
				int AndPosition = StringUtil.Find(ConditionsStringLeft,"&")
				string ConditionToCheck = ConditionsStringLeft
				if AndPosition != -1 ; some & sign found
					ConditionToCheck = StringUtil.Substring(ConditionsStringLeft,0,AndPosition)
					ConditionsStringLeft = StringUtil.Substring(ConditionsStringLeft,AndPosition + 1)
				else
					ConditionsStringLeft = ""
				endif
				bool ValidOrConditionFound = false
				while !ValidOrConditionFound && (ConditionToCheck != "")
					int OrPosition = StringUtil.Find(ConditionToCheck,"|")
					string SingleConditionToCheck
					if OrPosition != -1 ; some | sign found
						SingleConditionToCheck = StringUtil.Substring(ConditionToCheck,0,OrPosition)
						ConditionToCheck = StringUtil.Substring(ConditionToCheck,OrPosition + 1)
					else
						SingleConditionToCheck = ConditionToCheck
						ConditionToCheck = ""
					endif
					bool NegatedCondition = StringUtil.Find(SingleConditionToCheck,"!") == 0
					if NegatedCondition
						SingleConditionToCheck = StringUtil.Substring(SingleConditionToCheck,1)
					endif
					if StringUtil.Find(SingleConditionToCheck,"$") == 0 ; playerconditions
						ValidOrConditionFound = checkplayercondition(SingleConditionToCheck)
					elseif StringUtil.Find(SingleConditionToCheck,"LocType") == 0
						if SingleConditionToCheck == "loctypewilderness"
							ValidOrConditionFound = (kCurrentLoc == NONE)
						elseif JsonUtil.StringListCount(ThoughtDatabaseFilename,SingleConditionToCheck) >= 1
							ValidOrConditionFound = kCurrentLoc.HasKeyword(GetLocTypeKeyword(SingleConditionToCheck))
						endif
					elseif StringUtil.Find(SingleConditionToCheck,"#") == 0 ; wornkeyword		
						Keyword WornKW = Keyword.GetKeyword(StringUtil.Substring(SingleConditionToCheck,1)) ; remove the leading # sign
						ValidOrConditionFound = Game.GetPlayer().WornHasKeyword(WornKW)
					else ; papyrusvalue
						ValidOrConditionFound = StorageUtil.GetIntValue(None, SingleConditionToCheck) 
					endif
					if NegatedCondition
						ValidOrConditionFound = !ValidOrConditionFound
					endif
				endwhile
				ConditionCheckRunning = ValidOrConditionFound
			endwhile
			if ConditionCheckRunning ; still no negative found
				StorageUtil.StringListAdd(none,"ypsActiveConditionsList",ConditionsString)
;AllConditions += ConditionsString + " "
			endif			
		endif
		j += 1
	endwhile


	int ThoughtChoicesCount = StorageUtil.StringListCount(none,"ypsActiveConditionsList")
	if ThoughtChoicesCount >= 1
		int ThoughtChoice = Utility.RandomInt(1,ThoughtChoicesCount) - 1
		string ThoughtChoiceString = StorageUtil.StringListGet(none,"ypsActiveConditionsList", ThoughtChoice)
;Debug.Messagebox(ThoughtChoicesCount+" "+ThoughtChoice+" "+AllConditions)
		playthoughtchoice(ThoughtDatabaseFilename,ThoughtChoiceString,TruncateLongThoughts,SplitLongThoughts,MessageboxLongThoughts,LongThoughtLimit,soundvolume,10.0,playsubtitles)
	endif

  endif
  
endfunction

bool soundisplaying = false ; to avoid that multiple sounds are played simultaneously

function playthoughtchoice(string ThoughtDatabaseFilename,string ThoughtChoiceString,bool TruncateLongThoughts = false, bool SplitLongThoughts = true, bool MessageboxLongThoughts = true, int LongThoughtLimit = 80, float soundvolume = 0.5, float soundduration = 10.0, bool playsubtitles = true)

; 		ThoughtDatabaseFilename: path to the thoughts database json
;	 	ThoughtChoiceString: selection marker in the json file, will play a random string
; 		TruncateLongThoughts,SplitLongThoughts,MessageboxLongThoughts: how to handle thought strings longer than LongThoughtLimit
;		soundduration: duration in seconds, after which sound playback will be stopped. Make sure this is at least as long as the longest thought wav file you want to play. But making it too long is causing delays when playing the next sound.
; 		playsubtitles: will show thoughts as notifications/messageboxes, even when a sound is played

	if ThoughtDatabaseFilename == ""
		if Game.GetPlayer().GetActorBase().GetSex() == 1
			ThoughtDatabaseFilename = FemaleThoughtDatabaseFilename
		else
			ThoughtDatabaseFilename = MaleThoughtDatabaseFilename
		endif
	endif
	JsonUtil.Load(ThoughtDatabaseFilename)	
	int ThoughtPosition = Utility.RandomInt(0, JsonUtil.StringListCount(ThoughtDatabaseFilename,ThoughtChoiceString) - 1)
	string ThoughtMessage = JsonUtil.StringListGet(ThoughtDatabaseFilename,ThoughtChoiceString,ThoughtPosition)
	if ThoughtMessage != ""
		sound soundtoplay = NONE
		bool playsound = false
		if (StringUtil.Find(ThoughtMessage, "[") == 0) && (StringUtil.Find(ThoughtMessage, "]") == 6) ; sound number detected
			if (Game.GetModByName("ypssounds.esp") != 255) && (!soundisplaying)
				int soundnumber = 0 ; calculate sound number from thought string:
				soundnumber += 10000*(StringUtil.AsOrd(StringUtil.Substring(ThoughtMessage,1,1)) - 48) ; 48 = ord("0")
				soundnumber += 1000*(StringUtil.AsOrd(StringUtil.Substring(ThoughtMessage,2,1)) - 48)
				soundnumber += 100*(StringUtil.AsOrd(StringUtil.Substring(ThoughtMessage,3,1)) - 48)
				soundnumber += 10*(StringUtil.AsOrd(StringUtil.Substring(ThoughtMessage,4,1)) - 48)
				soundnumber += StringUtil.AsOrd(StringUtil.Substring(ThoughtMessage,5,1)) - 48
				int soundid = 3425 + (2*soundnumber) ; 0x0d61 = 3425 (offset of soundmarkers in esp)
				soundtoplay = Game.GetFormFromFile(soundid,"ypssounds.esp") as sound
				playsound = (soundtoplay != NONE)
				if playsound
					soundisplaying = true ; lock
				endif
			endif
			ThoughtMessage = StringUtil.Substring(ThoughtMessage,7,0) ; cut away the [number]		
		endif
		if (ThoughtMessage != "") && (playsubtitles || !soundtoplay) ; show thought string --- thought might be empty after sound is cut away
			if StringUtil.GetLength(ThoughtMessage) > LongThoughtLimit
				if TruncateLongThoughts
					string ThoughtToShow = StringUtil.Substring(ThoughtMessage,0,LongThoughtLimit)
					Debug.Notification(ThoughtToShow+"...")					
				elseif SplitLongThoughts
					int SplitPosition = (StringUtil.GetLength(ThoughtMessage) / 2) - 3
					if SplitPosition < 30
						SplitPosition = 30
					endif
					bool SpaceFound = false
					while !SpaceFound && (SplitPosition < StringUtil.GetLength(ThoughtMessage))
						SpaceFound = (StringUtil.Substring(ThoughtMessage,SplitPosition,1) == " ")
						if !SpaceFound
							SplitPosition += 1
						endif
					endwhile
					Debug.Notification(StringUtil.Substring(ThoughtMessage,0,SplitPosition))
					Debug.Notification(StringUtil.Substring(ThoughtMessage,SplitPosition + 1))
				elseif MessageboxLongThoughts
					Debug.Messagebox(ThoughtMessage)
				else
					Debug.Notification(ThoughtMessage)
				endif
			else
				Debug.Notification(ThoughtMessage)
			endif
		endif

		; Just this one line was added, everything else is original ypsThoughts code, Version 6.9.2
		SendModEvent("YPS_ThoughtEvent", ThoughtMessage, 0) ;

		if playsound
			int instanceID = soundtoplay.play(Game.Getplayer())  ; play sound from self
			Sound.SetInstanceVolume(instanceID, soundvolume) 
			Utility.Wait(soundduration)
			Sound.StopInstance(instanceID)
			soundisplaying = false ; release lock
		endif
	endif

endfunction


keyword property LocTypeAnimalDen auto
keyword property LocTypeBanditCamp auto
keyword property LocTypeBarracks auto
keyword property LocTypeCastle auto
keyword property LocTypeCemetery auto
keyword property LocTypeCity auto
keyword property LocTypeClearable auto
keyword property LocTypeDragonLair auto
keyword property LocTypeDragonPriestLair auto
keyword property LocTypeDraugrCrypt auto
keyword property LocTypeDungeon auto
keyword property LocTypeDwarvenAutomatons auto
keyword property LocTypeDwelling auto
keyword property LocTypeFalmerHive auto
keyword property LocTypeFarm auto
keyword property LocTypeForswornCamp auto
keyword property LocTypeGiantCamp auto
keyword property LocTypeGuild auto
keyword property LocTypeHabitation auto
keyword property LocTypeHabitationHasInn auto
keyword property LocTypeHagravenNest auto
keyword property LocTypeHold auto
keyword property LocTypeHoldCapital auto
keyword property LocTypeHoldMajor auto
keyword property LocTypeHoldMinor auto
keyword property LocTypeHouse auto
keyword property LocTypeInn auto
keyword property LocTypeJail auto
keyword property LocTypeLumberMill auto
keyword property LocTypeMilitaryCamp auto
keyword property LocTypeMilitaryFort auto
keyword property LocTypeMine auto
keyword property LocTypeOrcStronghold auto
keyword property LocTypePlayerHouse auto
keyword property LocTypeSettlement auto
keyword property LocTypeShip auto
keyword property LocTypeShipwreck auto
keyword property LocTypeSprigganGrove auto
keyword property LocTypeStewardsDwelling auto
keyword property LocTypeStore auto
keyword property LocTypeTemple auto
keyword property LocTypeTown auto
keyword property LocTypeVampireLair auto
keyword property LocTypeWarlockLair auto
keyword property LocTypeWerebearLair auto
keyword property LocTypeWerewolfLair auto

keyword function GetLocTypeKeyword(string LocTypeString)
	if LocTypeString == "LocTypeAnimalDen"
		return LocTypeAnimalDen
	elseif LocTypeString == "LocTypeBanditCamp"
		return LocTypeBanditCamp
	elseif LocTypeString == "LocTypeBarracks"
		return LocTypeBarracks
	elseif LocTypeString == "LocTypeCastle"
		return LocTypeCastle
	elseif LocTypeString == "LocTypeCemetery"
		return LocTypeCemetery
	elseif LocTypeString == "LocTypeCity"
		return LocTypeCity
	elseif LocTypeString == "LocTypeClearable"
		return LocTypeClearable
	elseif LocTypeString == "LocTypeDragonLair"
		return LocTypeDragonLair
	elseif LocTypeString == "LocTypeDragonPriestLair"
		return LocTypeDragonPriestLair
	elseif LocTypeString == "LocTypeDraugrCrypt"
		return LocTypeDraugrCrypt
	elseif LocTypeString == "LocTypeDungeon"
		return LocTypeDungeon
	elseif LocTypeString == "LocTypeDwarvenAutomatons"
		return LocTypeDwarvenAutomatons
	elseif LocTypeString == "LocTypeDwelling"
		return LocTypeDwelling
	elseif LocTypeString == "LocTypeFalmerHive"
		return LocTypeFalmerHive
	elseif LocTypeString == "LocTypeFarm"
		return LocTypeFarm
	elseif LocTypeString == "LocTypeForswornCamp"
		return LocTypeForswornCamp
	elseif LocTypeString == "LocTypeGiantCamp"
		return LocTypeGiantCamp
	elseif LocTypeString == "LocTypeGuild"
		return LocTypeGuild
	elseif LocTypeString == "LocTypeHabitation"
		return LocTypeHabitation
	elseif LocTypeString == "LocTypeHabitationHasInn"
		return LocTypeHabitationHasInn
	elseif LocTypeString == "LocTypeHagravenNest"
		return LocTypeHagravenNest
	elseif LocTypeString == "LocTypeHold"
		return LocTypeHold
	elseif LocTypeString == "LocTypeHoldCapital"
		return LocTypeHoldCapital
	elseif LocTypeString == "LocTypeHoldMajor"
		return LocTypeHoldMajor
	elseif LocTypeString == "LocTypeHoldMinor"
		return LocTypeHoldMinor
	elseif LocTypeString == "LocTypeHouse"
		return LocTypeHouse
	elseif LocTypeString == "LocTypeInn"
		return LocTypeInn
	elseif LocTypeString == "LocTypeJail"
		return LocTypeJail
	elseif LocTypeString == "LocTypeLumberMill"
		return LocTypeLumberMill
	elseif LocTypeString == "LocTypeMilitaryCamp"
		return LocTypeMilitaryCamp
	elseif LocTypeString == "LocTypeMilitaryFort"
		return LocTypeMilitaryFort
	elseif LocTypeString == "LocTypeMine"
		return LocTypeMine
	elseif LocTypeString == "LocTypeOrcStronghold"
		return LocTypeOrcStronghold
	elseif LocTypeString == "LocTypePlayerHouse"
		return LocTypePlayerHouse
	elseif LocTypeString == "LocTypeSettlement"
		return LocTypeSettlement
	elseif LocTypeString == "LocTypeShip"
		return LocTypeShip
	elseif LocTypeString == "LocTypeShipwreck"
		return LocTypeShipwreck
	elseif LocTypeString == "LocTypeSprigganGrove"
		return LocTypeSprigganGrove
	elseif LocTypeString == "LocTypeStewardsDwelling"
		return LocTypeStewardsDwelling
	elseif LocTypeString == "LocTypeStore"
		return LocTypeStore
	elseif LocTypeString == "LocTypeTemple"
		return LocTypeTemple
	elseif LocTypeString == "LocTypeTown"
		return LocTypeTown
	elseif LocTypeString == "LocTypeVampireLair"
		return LocTypeVampireLair
	elseif LocTypeString == "LocTypeWarlockLair"
		return LocTypeWarlockLair
	elseif LocTypeString == "LocTypeWerebearLair"
		return LocTypeWerebearLair
	elseif LocTypeString == "LocTypeWerewolfLair"
		return LocTypeWerewolfLair
	else
		return NONE
	endif
endfunction

globalvariable property PlayerFollowerCount auto

bool function checkplayercondition(string conditiontype)
	if conditiontype == "$isflying"
		return Game.GetPlayer().isflying()
	elseif conditiontype == "$isincombat"
		return Game.GetPlayer().isincombat()
	elseif conditiontype == "$isonmount"
		return Game.GetPlayer().isonmount()
	elseif conditiontype == "$isrunning"
		return Game.GetPlayer().isrunning()
	elseif conditiontype == "$issneaking"
		return Game.GetPlayer().issneaking()
	elseif conditiontype == "$issprinting"
		return Game.GetPlayer().issprinting()
	elseif conditiontype == "$istrespassing"
		return Game.GetPlayer().istrespassing()
	elseif conditiontype == "$isweapondrawn"
		return Game.GetPlayer().isweapondrawn()
	elseif conditiontype == "$isswimming"
		return Game.GetPlayer().isswimming()
	elseif conditiontype == "$issitting"
		return Game.GetPlayer().GetSitState() == 3
	elseif conditiontype == "$hasnogold"
		return Game.GetPlayer().GetItemCount(Gold001) == 0
	elseif conditiontype == "$hasfollower"
		return PlayerFollowerCount.GetValueInt() >= 1
	elseif conditiontype == "$isoverencumbered"
		return Game.GetPlayer().GetActorValue("InventoryWeight") > Game.GetPlayer().GetActorValue("CarryWeight")
	else
		return false
	endif
endfunction	


MiscObject Property Gold001 Auto

