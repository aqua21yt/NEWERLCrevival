.text
#include <asm_setup.S>

.extern doneWithGameDisplayBRLAN
.extern doneWithGameDisplayGroups

.global GameDisplayBRLAN
GameDisplayBRLAN:
	lis r4, PCBRLAN@h
	ori r4, r4, PCBRLAN@l
	li r5, 9
	b doneWithGameDisplayBRLAN
	
.global GameDisplayGroups
GameDisplayGroups:
	lis r4, PCGroups@h
	ori r4, r4, PCGroups@l
	lis r5, PCGroupsIDs@h
	ori r5, r5, PCGroupsIDs@l
	li r6, 9
	b doneWithGameDisplayGroups
	
.data

.global PCBRLAN
PCBRLAN:
	.long Anim_countdownCount
	.long Anim_countdownGo
	.long Anim_inMarioCoin
	.long Anim_collectionPartCoin
	.long Anim_fragmentCoinAppear
	.long Anim_fragmentCoinDisapp
	.long Anim_timeAlarm
	.long Anim_timeOutRedSign
	.long Anim_timeAppear
	.long Anim_timerRed
	.long Anim_timerSignal
	.long Anim_timerSignalRed
	.long Anim_timerWait
	
.global PCGroups
PCGroups:
	.long Group_inMarioCoin
	.long Group_PurpleCoin
	.long Group_Countdown
	.long Group_Countdown
	.long Group_TimeLimit
	.long Group_TimeLimit
	.long Group_TimeLimit
	.long Group_TimeLimit
	.long Group_TimeLimit

.global PCGroupsIDs
PCGroupsIDs:
	.long 0
	.long 1
	.long 2
	.long 3
	.long 4
	.long 5
	.long 6
	.long 7
	.long 8




.global Anim_countdownCount
Anim_countdownCount:
	.string "gameScene_37_countdownCount.brlan"

.global Anim_countdownGo
Anim_countdownGo:
	.string "gameScene_37_countdownGo.brlan"
	
.global Anim_inMarioCoin
Anim_inMarioCoin:
	.string "gameScene_37_inMarioCoin.brlan"

.global Anim_collectionPartCoin
Anim_collectionPartCoin:
	.string "gameScene_37_collectionPartCoin.brlan"

.global Anim_fragmentCoinAppear
Anim_fragmentCoinAppear:
	.string "gameScene_37_fragmentCoinAppear.brlan"
	
.global Anim_fragmentCoinDisapp
Anim_fragmentCoinDisapp:
	.string "gameScene_37_fragmentCoinDisapp.brlan"
	
.global Anim_timeAlarm
Anim_timeAlarm:
	.string "gameScene_37_timeAlarm.brlan"
	
.global Anim_timeOutRedSign
Anim_timeOutRedSign:
	.string "gameScene_37_timeOutRedSign.brlan"
	
.global Anim_timeAppear
Anim_timeAppear:
	.string "gameScene_37_timerAppear.brlan"
	
.global Anim_timerRed
Anim_timerRed:
	.string "gameScene_37_timerRed.brlan"
	
.global Anim_timerSignal
Anim_timerSignal:
	.string "gameScene_37_timerSignal.brlan"
	
.global Anim_timerSignalRed
Anim_timerSignalRed:
	.string "gameScene_37_timerSignalRed.brlan"

.global Anim_timerWait
Anim_timerWait:
	.string "gameScene_37_timerWait.brlan"

.global Group_inMarioCoin
Group_inMarioCoin:
	.string "C00_inMarioCoin"

.global Group_PurpleCoin
Group_PurpleCoin:
	.string "C01_PurpleCoin"

.global Group_Countdown
Group_Countdown:
	.string "C02_Countdown"

.global Group_TimeLimit
Group_TimeLimit:
	.string "C03_TimeLimit"