#include <common.h>
#include <game.h>
#include <stage.h>
#include <g3dhax.h>
#include <sfx.h>
#include <gamescene.h> //It contains the dGameDisplay_c, which was RE'd by Grop (because i was a noob back when he did it), thanks to him !
#include "boss.h"
#include "levelinfo.h"

/******************************************************************************/
/**ALL THE STUFF BELOW IS TESTS, DEBUG MODE AND IS UNRELATED TO ANYTHING ELSE**/
/******************************************************************************/

extern int RedCoinsCollected;
extern int caughtRedCoins;
extern int gameDisplayTimer;
extern int newTimeCounter;
extern int frameTimer;
extern int CurrentWorld;
extern int CurrentLevel;
extern int GlobalRedCoinsToCatch;

u16 redFrames[15] = {0x17, 0x27, 0x38, 0x48, 0x59, 0x69, 0x7A, 0x8A, 0x9B, 0xAC, 0xBC, 0xCD, 0xDD, 0xEE, 0xFF};
u16 greenFrames[15] = {0x74, 0x6B, 0x63, 0x5B, 0x52, 0x4A, 0x42, 0x3A, 0x31, 0x29, 0x21, 0x18, 0x10, 0x8, 0};
u16 blueFrames[15] = {0xD3, 0xC3, 0xB4, 0xA5, 0x96, 0x87, 0x78, 0x69, 0x5A, 0x4B, 0x3C, 0x2D, 0x1E, 0xF, 0};

/***************************/
/**Nybble getting function**/
/***************************/

extern "C" float pow(float num, float power);

int getNybbleValue(u32 settings, int fromNybble, int toNybble, bool doOSReport) { //This function is to get a nybble from a sprite's settings. It's unused but i used it for some tests, and i don't want to remove it :c
	int numberOfNybble = (toNybble  - fromNybble) + 1;               //gets how many nybbles are used for the process (example: nybbles 4-6 -> there's nybbles 4, 5 and 6 used -> numberOfNybble = 3) 
	int valueToUse = 48 - (4 * toNybble);                            //gets the value to use with the bitshift at the end 
	int fShit = pow(16, numberOfNybble) - 1;                         //gets the value to use with the "&" operator at the end 
	if(doOSReport) {
		OSReport("-> (settings >> %d) & 0x%x) => ", valueToUse, fShit);  //debugging
	}
	return ((settings >> valueToUse) & fShit);                       //uses everything to make the nybble value 
}

/*********************************************************/
/**Global variables declaring, externs, and lots of shit**/
/*********************************************************/

extern bool enableDebugMode;																											//Is Debug Mode enabled ?
extern bool enableCollisionMode;																										//Is the Collision Viewer enabled ?
extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup); 	//Powerup Changing Functin | Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
extern "C" int CheckExistingPowerup(void * Player);																						//Check the powerup of a player
static nw4r::snd::StrmSoundHandle s_handle;																								//sound handling storing
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);									//play sound function
u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);	//hijacking
// extern "C" u32 dword_803521BC[4];	//Tests from when i was debugging things
// extern "C" u32 dword_803521A8[4];	//Tests from when i was debugging things
// extern "C" u32 dword_8042A5B0[4];	//Tests from when i was debugging things
int minuscounter;						//Counts how much time the minus button was pressed (to enable the debug mode)
extern bool isTrailerMode;				//Is the Trailer Mode enabled ?
bool isLuigiActive;						//Is Luigi an active player ?
bool isKinoBActive;						//Is Blue Toad an active player ?
bool isKinoYActive;						//Is Yellow Toad an active player ?
bool isSC1Active;						//Is the Star Coin 1 collected ?
bool isSC2Active;						//Is the Star Coin 2 collected ?
bool isSC3Active;						//Is the Star Coin 3 collected ?
int pActivityDoOneTime;					//Related to a Trailer Mode removed feature

/***************************************************************/
/**gameScene's onExecute adds for the debug mode and some shit**/
/***************************************************************/

int dGameDisplay_c::doWaitCheck() { //So the reason i'm doing this in the gameScene's onExecute is because it's executed every frame in every level, wherever the player are, so it's nice to check for things wherever level you're in and wherever the players are
	int orig_val = this->onExecute_orig();						//Execute the original onExecute, store it in orig_val
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());	//Get the current pressed buttons
	// OSReport("dword_803521BC[0] = %d | dword_803521BC[1] = %d | dword_803521BC[2] = %d | dword_803521BC[3] = %d\n", dword_803521BC[0], dword_803521BC[1], dword_803521BC[2], dword_803521BC[3]);	//Just tests, don't pay attention to it
	// OSReport("dword_803521A8[0] = %d | dword_803521A8[1] = %d | dword_803521A8[2] = %d | dword_803521A8[3] = %d\n", dword_803521A8[0], dword_803521A8[1], dword_803521A8[2], dword_803521A8[3]);	//Just tests, don't pay attention to it
	// OSReport("dword_8042A5B0[0] = %d | dword_8042A5B0[1] = %d | dword_8042A5B0[2] = %d | dword_8042A5B0[3] = %d\n", dword_8042A5B0[0], dword_8042A5B0[1], dword_8042A5B0[2], dword_8042A5B0[3]);	//Just tests, don't pay attention to it
	if(enableDebugMode) {	//If the debug mode is enabled
		dAcPy_c *player = dAcPy_c::findByID(0);	//Get the Player 1
		if ((GetActiveRemocon()->heldButtons == 0x402) && (nowPressed & 0x402)) { // B + UP // Changes the player's powerup
			int playerPowerup = ((CheckExistingPowerup(player) == 7) ? 0 : (CheckExistingPowerup(player) + 1));          //increment the player's powerup id by 1. If 7 -> 0
			dAcPy_c__ChangePowerupWithAnimation(player, playerPowerup);                                                  //change it
		}
		if ((GetActiveRemocon()->heldButtons == 0x401) && (nowPressed & 0x401)) { // B + DOWN // Beats the current level
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);                                                             //exit the current stage and beat it
		}
		/*
		if ((GetActiveRemocon()->heldButtons == 0x408) && (nowPressed & 0x408)) { // B + LEFT // Turn On/Off the collision viewer
			enableCollisionMode = !enableCollisionMode;                                                                  //enable or disable to collision debugger
		}
		*/
		if ((GetActiveRemocon()->heldButtons == 0x404) && (nowPressed & 0x404)) { // B + RIGHT // Spawns a Star
			int enitemsettings = 0 | (1 << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (8 << 16);                            //setting the settings
			dStageActor_c *Star = dStageActor_c::create(EN_ITEM, enitemsettings, &player->pos, 0, 0);                    //creates the Star
		}
		if ((GetActiveRemocon()->heldButtons == 0x600) && (nowPressed & 0x600)) { // B + ONE // Enables the Trailer Mode (Makes the music stops)
			isTrailerMode = !isTrailerMode;                                                                              //disable or enable switching
			ExitStage(WORLD_MAP, 0, EXIT_LEVEL, MARIO_WIPE);                                                             //exit the current stage
			/* Uncomment the following lines to make the gameScene layout disappear when B + ONE is pressed*/

				//Commented because Mullkaw//
			/*nw4r::lyt::Picture *_1 = dGameDisplay_c::instance->layout.findPictureByName("P_baseS_01");                   //getting the panes, too lazy to name them all
			nw4r::lyt::Picture *_2 = dGameDisplay_c::instance->layout.findPictureByName("P_base_01");
			nw4r::lyt::Picture *_3 = dGameDisplay_c::instance->layout.findPictureByName("P_otasukeIcon_00");
			nw4r::lyt::Picture *_4 = dGameDisplay_c::instance->layout.findPictureByName("P_marioIcon_00");
			nw4r::lyt::Picture *_5 = dGameDisplay_c::instance->layout.findPictureByName("P_luijiIcon_00");
			nw4r::lyt::Picture *_6 = dGameDisplay_c::instance->layout.findPictureByName("P_kinoB_00");
			nw4r::lyt::Picture *_7 = dGameDisplay_c::instance->layout.findPictureByName("P_kinoY_00");
			nw4r::lyt::Picture *_31 = dGameDisplay_c::instance->layout.findPictureByName("P_baseS_00");
			nw4r::lyt::Picture *_8 = dGameDisplay_c::instance->layout.findPictureByName("P_base_00");
			nw4r::lyt::Picture *_9 = dGameDisplay_c::instance->layout.findPictureByName("P_coin_00");
			nw4r::lyt::Picture *_10 = dGameDisplay_c::instance->layout.findPictureByName("P_collectOff_00");
			nw4r::lyt::Picture *_11 = dGameDisplay_c::instance->layout.findPictureByName("P_collectOff_01");
			nw4r::lyt::Picture *_12 = dGameDisplay_c::instance->layout.findPictureByName("P_collectOff_02");
			nw4r::lyt::Picture *_13 = dGameDisplay_c::instance->layout.findPictureByName("P_collection_00");
			nw4r::lyt::Picture *_14 = dGameDisplay_c::instance->layout.findPictureByName("P_collection_01");
			nw4r::lyt::Picture *_15 = dGameDisplay_c::instance->layout.findPictureByName("P_collection_02");
			nw4r::lyt::Picture *_16 = dGameDisplay_c::instance->layout.findPictureByName("P_timer_00");
			nw4r::lyt::TextBox *_18 = dGameDisplay_c::instance->layout.findTextBoxByName("T_otaChuS_00");
			nw4r::lyt::TextBox *_19 = dGameDisplay_c::instance->layout.findTextBoxByName("T_otaChu_00");
			nw4r::lyt::TextBox *_20 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_01");
			nw4r::lyt::TextBox *_21 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_02");
			nw4r::lyt::TextBox *_22 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_03");
			nw4r::lyt::TextBox *_23 = dGameDisplay_c::instance->layout.findTextBoxByName("T_x_04");
			nw4r::lyt::TextBox *_24 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_00");
			nw4r::lyt::TextBox *_25 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_01");
			nw4r::lyt::TextBox *_26 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_02");
			nw4r::lyt::TextBox *_27 = dGameDisplay_c::instance->layout.findTextBoxByName("T_left_03");
			nw4r::lyt::TextBox *_28 = dGameDisplay_c::instance->layout.findTextBoxByName("T_coin_00");
			nw4r::lyt::TextBox *_29 = dGameDisplay_c::instance->layout.findTextBoxByName("T_score_00");
			nw4r::lyt::TextBox *_30 = dGameDisplay_c::instance->layout.findTextBoxByName("T_time_00");
			nw4r::lyt::Pane *_32 = dGameDisplay_c::instance->layout.findPaneByName("N_collection_00");
			int visibility = !isTrailerMode;                                                                              //store the visibility bool

			if(pActivityDoOneTime == 0) {                                                                                //get which players are active only a single time
				if(_5->flag == 1) {                                                                                      //if luigi's pane is visible, luigi is active
					isLuigiActive = true;
				}
				if(_6->flag == 1) {                                                                                      //if kinob's pane is visible, kinob is active
					isKinoBActive = true;
				}
				if(_7->flag == 1) {                                                                                      //if kinoy's pane is visible, kinoy is active
					isKinoYActive = true;
				}
				pActivityDoOneTime = 1;                                                                                  //don't redo that process next time, cuz useless
			}

			_1->SetVisible(visibility);                                                                                  //set the panes visibilities
			_2->SetVisible(visibility);
			_3->SetVisible(visibility);
			_4->SetVisible(visibility);

			if(isLuigiActive) {                                                                                          //if luigi is active, affect his panes
				_5->SetVisible(visibility);
				_21->SetVisible(visibility);
				_25->SetVisible(visibility);
			}
			
			if(isKinoBActive) {                                                                                          //if kinob is active, affect his panes
				_6->SetVisible(visibility);
				_22->SetVisible(visibility);
				_26->SetVisible(visibility);
			}
			
			if(isKinoYActive) {                                                                                          //if kinoy is active, affect his panes
				_7->SetVisible(visibility);
				_23->SetVisible(visibility);
				_27->SetVisible(visibility);
			}

			_8->SetVisible(visibility);
			_9->SetVisible(visibility);
			_10->SetVisible(visibility);
			_11->SetVisible(visibility);
			_12->SetVisible(visibility);
			_13->SetVisible(visibility);
			_14->SetVisible(visibility);
			_15->SetVisible(visibility);
			_16->SetVisible(visibility);
			_18->SetVisible(visibility);
			_19->SetVisible(visibility);
			_20->SetVisible(visibility);
			_24->SetVisible(visibility);
			_28->SetVisible(visibility);
			_29->SetVisible(visibility);
			_30->SetVisible(visibility);
			_31->SetVisible(visibility);
			_32->SetVisible(visibility);*/
		}		
	}
	if ((GetActiveRemocon()->heldButtons == 0xC00) && (nowPressed & 0xC00)) { // B + A // Print Checkpoint-related values
		//I used that to get which values in GameMgrP->eight could help me to detect if the checkpoint was triggered, so i can make the preGame pictures change after the checkpoint is took, see quickrestart.cpp
		OSReport("GameMgrP->eight._4 = %d\n", GameMgrP->eight._4);
		OSReport("GameMgrP->eight.checkpointEntranceXPos = %f\n", GameMgrP->eight.checkpointEntranceXPos);	//This value used to be unnamed, i named it after figuring out what it was
		OSReport("GameMgrP->eight.checkpointEntranceYPos = %f\n", GameMgrP->eight.checkpointEntranceYPos);	//This value used to be unnamed, i named it after figuring out what it was
		OSReport("GameMgrP->eight._10 = %d\n", GameMgrP->eight._10);
		OSReport("GameMgrP->eight._14 = %d\n", GameMgrP->eight._14);
		OSReport("GameMgrP->eight.checkpointWorldID = %d\n", GameMgrP->eight.checkpointWorldID);			//This value used to be unnamed, i named it after figuring out what it was
		OSReport("GameMgrP->eight.checkpointLevelID = %d\n", GameMgrP->eight.checkpointLevelID);			//This value used to be unnamed, i named it after figuring out what it was
		OSReport("GameMgrP->eight._1A = %d\n", GameMgrP->eight._1A);
		OSReport("GameMgrP->eight.checkpointEntranceID = %d\n", GameMgrP->eight.checkpointEntranceID);		//This value used to be unnamed, i named it after figuring out what it was
		OSReport("GameMgrP->eight._1C = %d\n", GameMgrP->eight._1C);
		OSReport("GameMgrP->eight._20 = %d\n", GameMgrP->eight._20);
		OSReport("GameMgrP->eight._24 = %d\n", GameMgrP->eight._24);
		OSReport("GameMgrP->eight._28 = %d\n", GameMgrP->eight._28);
		OSReport("GameMgrP->eight._2C = %d\n", GameMgrP->eight._2C);
		OSReport("GameMgrP->eight._30 = %d\n", GameMgrP->eight._30);
	}
	
	enableCollisionMode = true;
	int frames;
	
	if (caughtRedCoins > 0) {
		char countString[7];
		wchar_t wcountString_t[7];

		sprintf(countString, "%03dX%03d", caughtRedCoins, GlobalRedCoinsToCatch);

		wcountString_t[0] = 50 + countString[0];
		wcountString_t[1] = 50 + countString[1];
		wcountString_t[2] = 50 + countString[2];
		wcountString_t[3] = 95;
		wcountString_t[4] = 50 + countString[4];
		wcountString_t[5] = 50 + countString[5];
		wcountString_t[6] = 50 + countString[6];
		wcountString_t[7] = 0;
		
		this->layout.findTextBoxByName("TxtNumber")->SetString(wcountString_t, 7);
		this->layout.findTextBoxByName("ShaNumber")->SetString(wcountString_t, 7);
		
		this->layout.findPaneByName("Coin")->SetVisible(true);
		this->layout.findPictureByName("PicIconHundred")->SetVisible(true);
		this->layout.findPictureByName("ShaIconHundred")->SetVisible(true);
		
		if (caughtRedCoins == 1) {
			//this->layout.enableNonLoopAnim(4, false);
		}
		
		if (caughtRedCoins == GlobalRedCoinsToCatch) {
			//this->layout.enableNonLoopAnim(5, false);
			this->layout.findPictureByName("PicIconHundred")->SetVisible(false);
			this->layout.findPictureByName("ShaIconHundred")->SetVisible(false);
		}
	} else {
		this->layout.findPaneByName("Coin")->SetVisible(false);
		this->layout.findPictureByName("PicIconHundred")->SetVisible(false);
		this->layout.findPictureByName("ShaIconHundred")->SetVisible(false);
	}
	
	if (caughtRedCoins == 0) {
		char letterString[7];
		wchar_t wletterString_t[7];
		
		if (!(TimeKeeper::instance->paused)) {gameDisplayTimer++;}
		
		int chrMin = gameDisplayTimer % 3600;
		int Min = (gameDisplayTimer - chrMin) / 3600;
		int chrSec = gameDisplayTimer % 60;
		int sec = ((gameDisplayTimer - chrSec) / 60) % 60;
		/*
		chrMinute = gameDisplayTimer / 60;
		chrSec = gameDisplayTimer % 60;
		chrCent = gameDisplayTimer % 100.0;
		*/
		if (Min < 1) {sprintf(letterString, "%02dX%02dX%02d", 00, sec, chrSec);}
		{sprintf(letterString, "%02dX%02dX%02d", Min, sec, chrSec);}

		wletterString_t[0] = 50 + letterString[0];
		wletterString_t[1] = 50 + letterString[1];
		wletterString_t[2] = 108;

		wletterString_t[3] = 50 + letterString[3];
		wletterString_t[4] = 50 + letterString[4];
		wletterString_t[5] = 109;
		
		wletterString_t[6] = 50 + letterString[6];
		wletterString_t[7] = 50 + letterString[7];
		//wletterString_t[8] = 0;
		
		this->layout.findTextBoxByName("TimerTxtNumber")->SetString(wletterString_t, 7);
		this->layout.findTextBoxByName("TimerShaNumber")->SetString(wletterString_t, 7);
		this->layout.findPaneByName("TimerNumber")->SetVisible(true);
		
		this->P_TxtTick_00 = this->layout.findPaneByName("P_TxtTick_00");
		this->P_TxtTick_00->rotate.z = 360.0f / 3600.0f * chrMin;
		
	} else {
		this->layout.findPaneByName("TimerNumber")->SetVisible(false);
	}
	
	if (TimeKeeper::instance->timeLessThan100) {
		
		//this->layout.enableLoopAnim(7);
		//this->layout.enableNonLoopAnim(6, false);
		
		if (newTimeCounter < 11) {
			PlaySoundWithFunctionB4(SoundRelatedClass, &s_handle, STRM_BGM_YUKI, 1);
		}
		
	} else {
		//this->layout.disableAnim(7);
	}
	// Removed in v1.1, use the star coin menu instead
	// if(nowPressed & WPAD_MINUS) {                                                                                        //minus pressing counter
	// 	minuscounter++;                                                                                                  //increment it by one every press
	// 	if(minuscounter >= 16) {                                                                                         //if pressed 16 times
	// 		minuscounter = 0;                                                                                            //set it back to 0
	// 		enableDebugMode = !enableDebugMode;                                                                          //enable/disable the debug mode
	// 		//nw4r::lyt::TextBox *debugText = dGameDisplay_c::instance->layout.findTextBoxByName("T_debug_00");            //get the debugging textbox
	// 		//debugText->SetString(((enableDebugMode) ? L"Debug Mode" : L" "));                                            //make it visible or not
	// 		this->doHexCoin();                                                                                           //refresh the coin counter
	// 	}
	// }
	//*/
	return orig_val; //Return what the original onExecute was supposed to return
}

/****************************/
/**Hexadecimal coin counter**/
/****************************/

void dGameDisplay_c::doHexCoin() { //Make the coin counter display in hexadecimal if the debug mode is enabled. Why ? Because how do i know if it's enabled otherwise ?
	nw4r::lyt::TextBox *stupidcoin = dGameDisplay_c::instance->layout.findTextBoxByName("T_coin_00");					//get the coin textbox
	char str[2];																										//make a char that will contain the text to display
	sprintf(str, ((enableDebugMode && !isTrailerMode) ? "%02X" : "%02d"), dGameDisplay_c::instance->coins);				//insert the number in the char win three characters
	wchar_t nyeh[2];																									//make a wchar_t
	nyeh[0] = str[0];																									//put the char's content in the wchar_t
	nyeh[1] = str[1];
	stupidcoin->SetString(nyeh, 0, 2);																					//write it to the textbox
}