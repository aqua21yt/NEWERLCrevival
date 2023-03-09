#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>

const char* SoundEffects[] = {
	
	//////// ADDED BY NSMLW //////////////
	
	"sfx/GoldFlower_hit",			//2000
	"sfx/GoldFlower_out",			//2001
	"sfx/GoldFlower_get",			//2002
	"sfx/GoldFlower_throw",			//2003
	"sfx/GoldFlower_get",			//2004
	"sfx/FlipPanel",				//2005
	
	//////// ADDED BY NewerLC ////////////
	
	"stream/STRM_BGM_BARREL",		//2006

	"sfx/5#_KING_TERESA_APPEAR",		//2007
	"sfx/5#_KING_TERESA_ATTACK",		//2008
	"sfx/5#_KING_TERESA_DAMAGE_LAST",	//2009
	"sfx/5#_KING_TERESA_DIALOG1",		//2010
	"sfx/5#_KING_TERESA_DIALOG2",		//2011
	"sfx/5#_KING_TERESA_DIALOG3",		//2012
	"sfx/5#_KING_TERESA_DIALOG4",		//2013
	"sfx/5#_KING_TERESA_LAUGH",			//2014
	"sfx/5#_KING_TERESA_STOMP",			//2015
	"sfx/5#_KING_TERESA_STOMPED",		//2016
	
	"sfx/#6_BUNBUN_ANGRY",				//2017
	"sfx/#6_BUNBUN_DAMAGE_LAST",		//2018
	"sfx/#6_BUNBUN_DAMAGE1",			//2019
	"sfx/#6_BUNBUN_GUESS_MARIO",		//2020
	"sfx/#6_BUNBUN_INTRO",				//2021
	"sfx/#6_BUNBUN_JUMP1",				//2022
	"sfx/#6_BUNBUN_JUMP2",				//2023
	"sfx/#6_BUNBUN_RETRY1",				//2024
	"sfx/#6_BUNBUN_RETRY2",				//2025
	"sfx/#6_BUNBUN_SPIN",				//2026
	"sfx/#6_BUNBUN_SPIN_DONE1",			//2027
	"sfx/#6_BUNBUN_SPIN_DONE2",			//2028
	"sfx/#6_BUNBUN_SPIN_START1",		//2029
	"sfx/#6_BUNBUN_SPIN_START2",		//2030
	"sfx/#6_BUNBUN_STOMPED1",			//2031
	"sfx/#6_BUNBUN_STOMPED2",			//2032
	"sfx/#6_BUNBUN_STOMPED3",			//2033
	
	"sfx/8#0_AMBIENCE_MORI_NIGHT",		//2034
	"sfx/8#1_AMBIENCE_SCENE",			//2035
	"sfx/8#2_AMBIENCE_CHIKA",			//2036
	"sfx/8#3_AMBIENCE_SANBASHI_STORM",	//2037
	"sfx/8#4_AMBIENCE_YUKI",			//2038
	"sfx/8#5_AMBIENCE_OBAKE",			//2039
	"sfx/8#6_AMBIENCE_SORA",			//2040
	"sfx/8#7_AMBIENCE_GAKE",			//2041
	
	"sfx/#7_BLOCK_SPIN",				//2042
	
	"sfx/10#_GOLD_RING_GET",			//2043
	"sfx/10#_GOLD_RING_EFFECT",			//2044
	
	"sfx/QuestionPanel",				//2045
	
	"sfx/SE_BV_POLTA_ARM_DOWN",			//2046
	"sfx/SE_BV_POLTA_CORE_APPEAR",		//2047
	"sfx/SE_BV_POLTA_CORE_DAMAGE",		//2048
	"sfx/SE_BV_POLTA_DAMAGE_ARM",		//2049
	"sfx/SE_BV_POLTA_DAMAGE_BODY",		//2050
	"sfx/SE_BV_POLTA_DAMAGE_LAST",		//2051
	"sfx/SE_BV_POLTA_DIE",				//2052
	"sfx/SE_BV_POLTA_ENTER",			//2053
	"sfx/SE_BV_POLTA_GEN_GND_ROCK",		//2054
	"sfx/SE_BV_POLTA_GEN_ROCK",			//2055
	"sfx/SE_BV_POLTA_INTO_GROUND_STR",	//2056
	"sfx/SE_BV_POLTA_INTO_GROUND_WRK",	//2057
	"sfx/SE_BV_POLTA_LAUGH_STRONG",		//2058
	"sfx/SE_BV_POLTA_LOSE_ARM",			//2059
	"sfx/SE_BV_POLTA_LOSE_SHELL",		//2060
	"sfx/SE_BV_POLTA_OUT_GROUND_STR",	//2061
	"sfx/SE_BV_POLTA_OUT_GROUND_WRK",	//2062
	"sfx/SE_BV_POLTA_PREP_ARM_DOWN",	//2063
	"sfx/SE_BV_POLTA_PREP_GND_ROCK",	//2064
	"sfx/SE_BV_POLTA_PREP_PUNCH",		//2065
	"sfx/SE_BV_POLTA_PUNCH",			//2066
	"sfx/SE_BV_POLTA_RUN_AWAY",			//2067
	"sfx/SE_BV_POLTA_SHOUT",			//2068
	"sfx/SE_BV_POLTA_SHOUT_ENTER"		//2069
	
};

extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);
static nw4r::snd::StrmSoundHandle handlee;

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID, bool doTheResetThing);

int NewSFXPlayer(int id, bool withFunctionB4 = false) {
	if (handlee.Exists())
		handlee.Stop(0);

	int sID;
	hijackMusicWithSongName(SoundEffects[id], -1, false, 2, 1, &sID, true);
	OSReport("sID: %d\n", sID);
	if(withFunctionB4) PlaySoundWithFunctionB4(SoundRelatedClass, &handlee, sID, 1);
	return sID;
}

void newPlaySoundFromPosition(SoundPlayingClass *pointer, int ID, Vec2 *pos, u32 flags) {
	int sID = NewSFXPlayer(ID - 2000);
	pointer->PlaySoundAtPosition(sID, pos, flags);
}

void newPlaySoundFromPosition2(SoundPlayingClass *pointer, int ID, u32 dunno, u32 *handle) {
	int sID = NewSFXPlayer(ID - 2000);
	pointer->PlaySoundForSoundPlayingClass(sID, dunno, handle);
}