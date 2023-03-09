#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include "music.h"
#include "gameDisplay.h"


const char* FlagsSArcList [] = { "finish_ring", NULL };

//Course Clear Function
class dCourseClear_c : public dBase_c {
public:
	m2d::EmbedLayout_c layout;
	u8 data[0x330 - 0x208];
	dStateWrapper_c<dCourseClear_c> acState;

	static dCourseClear_c *instance;

	int onCreate_orig();
	int newOnCreate();

	int onExecute_orig();
	int newOnExecute();
};

class dScEndFlag : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	/* void bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimTexSrt_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimTexSrtA_and_setUpdateRate(const char* name, int unk, float unk2, float rate); */
	void setupModels();

	u8 exiting;
	
	int timer;
	bool isGlowing;
	bool visible;
	bool touched;
	int effTimer;
	/* void FinishRingSoundPlay(int id);
	
	const char RingsSounds[2] = {
		"stream/STRM_BGM_CHALLENGE_CLEAR"
		"stream/couse_clear_lr.n.32"
	}; */
	
	static dActor_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther); 
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c fogModel;
	m3d::anmChr_c animationChr;
	//m3d::anmChr_c animationChrA;
	/* m3d::anmChr_c animationSrt;
	m3d::anmChr_c animationSrtA; */
	nw4r::g3d::ResFile resFile;

	USING_STATES(dScEndFlag);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Touched);
	DECLARE_STATE(Anim);
};

CREATE_STATE(dScEndFlag, Wait);
CREATE_STATE(dScEndFlag, Touched);
CREATE_STATE(dScEndFlag, Anim);

//"Extern" functions
extern "C" void *StopBGMMusic();
//extern "C" void *StartBGMMusic();   ---Probably useful---- :)
extern void *SoundRelatedClass; //maybe unused

void dScEndFlag::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	fogModel.setDrawMatrix(matrix);
	fogModel.setScale(&scale);
	fogModel.calcWorld(false);
	
	/* flagModel.setDrawMatrix(matrix);
	flagModel.setScale(&scale);
	flagModel.calcWorld(false); */
	
}


void dScEndFlag::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

/* void dScEndFlag::bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmA = this->resFile.GetResAnmChr(name);
	this->animationChrA.bind(&this->flagModel, anmA, unk);
	this->flagModel.bindAnim(&this->animationChrA, unk2);
	this->animationChrA.setUpdateRate(rate);
}

void dScEndFlag::bindAnimTexSrt_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt(name);
	this->animationSrt.bind(&this->bodyModel, anmSrt, unk);
	this->bodyModel.bindAnim(&this->animationSrt, unk2);
	this->animationSrt.setUpdateRate(rate);
}

void dScEndFlag::bindAnimTexSrtA_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmTexSrt anmSrtA = this->resFile.GetResAnmTexSrt(name);
	this->animationSrtA.bind(&this->fogModel, anmSrtA, unk);
	this->fogModel.bindAnim(&this->animationSrtA, unk2);
	this->animationSrtA.setUpdateRate(rate);
} */

void dScEndFlag::setupModels() {
	
	resFile.data = getResource("finish_ring", "g3d/finish_ring.brres");
	//Load all models from "finish_ring.arc" and load its animations
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("goal_odai");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);
	
	nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("green_ring_fog");
	fogModel.setup(mdlA, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&fogModel, 0);
	
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("goal_odai");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	
/*	nw4r::g3d::ResAnmTexSrt anmSrt = this->resFile.GetResAnmTexSrt("goal_odai");
	this->animationSrt.setup(mdl, anmSrt, &this->allocator, 0);
	
	nw4r::g3d::ResAnmTexSrt anmSrtA = this->resFile.GetResAnmTexSrt("green_ring_fog");
	this->animationSrtA.setup(mdlA, anmSrtA, &this->allocator, 0); */
	
}

dActor_c* dScEndFlag::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dScEndFlag));
	return new(buffer) dScEndFlag;
}

void dScEndFlag::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	// PlaySoundWithFunctionB4(SoundRelatedClass, &starHandle, SFX_PURPLE_COIN, 1);
	// OSReport("Collided: %d -> ", WLClass::instance->flags);
	// OSReport("%d\n", WLClass::instance->flags);

	if(!this->touched) {
		this->effTimer = 0;
		doStateChange(&StateID_Touched);
		this->touched = true;
	}

	// flagtest();
}

void dScEndFlag::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
}

void dScEndFlag::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther); // :)
}

bool dScEndFlag::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dScEndFlag::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dScEndFlag::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dScEndFlag::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}

bool dScEndFlag::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dScEndFlag::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dScEndFlag::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dScEndFlag::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dScEndFlag::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

bool dScEndFlag::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}

/* const SpriteData FinishRingSpriteData = 
{ ProfileId::, 0, 0, 0, 0, 0x200, 0x300, 0, 0, 0, 0, 0 };

Profile FinishRingProfile(&dScEndFlag::build, SpriteId::513, &FinishRingSpriteData, ProfileId::, ProfileId::, "FinishRing", finish_ring); */


int dScEndFlag::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	setupModels();
	


	allocator.unlink(); 


	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	//float InitModel = flagModel.pos.y;
	isGlowing = true;
	visible = true;
	effTimer = 0;
	
	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 0.0; 
	HitMeBaby.xDistToEdge = 16.0; 
	HitMeBaby.yDistToEdge = 36.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	//Execute Animations
	bindAnimChr_and_setUpdateRate("goal_odai", 1, 0.0, 1.0);
	/* bindAnimTexSrt_and_setUpdateRate("green_ring", 1, 0.0, 1.0);
	bindAnimTexSrtA_and_setUpdateRate("green_ring_fog", 1, 0.0, 1.0); */

	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int dScEndFlag::onExecute() {
	acState.execute();

	updateModelMatrices();
	if (visible) {bodyModel._vf1C();}
	if (isGlowing) {fogModel._vf1C();}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}
	
	/* if (this->animationChrA.isAnimationDone()) {
		this->animationChrA.setCurrentFrame(0.0);
	} */

	//flagModel.pos.y = InitModel + 36.0; //Syntax hesitation,sorry!
	
	return true;
}

int dScEndFlag::onDelete() {
	return true;
}

int dScEndFlag::onDraw() {
	if (visible) {bodyModel.scheduleForDrawing();}
	if (isGlowing) {fogModel.scheduleForDrawing();}	

	return true;
}

void dScEndFlag::beginState_Wait() {
	this->timer = 0;
}
void dScEndFlag::executeState_Wait() {
	
	/* bodyModel.rot.y = 0x2000;
	fogModel.rot.y = 0x2000;
	NOTE: all these orders HAVE TO BE DEFINED IN CHR ANIMS INSTEAD */
	
	if (timer == 40 || timer == 80 || timer == 120 || timer == 160) {
		
		Vec efPos = {pos.x, pos.y, 330.0};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {2.0f, 0.8f, 1.0f};
		SpawnEffect("Wm_ob_redringkira", 0, &efPos, &nullRot, &efScale);
		
	}
	
	if (this->animationChr.isAnimationDone()) {
		this->timer = 0;
	}
	
	this->timer++;
}
void dScEndFlag::endState_Wait() {}

void dScEndFlag::beginState_Touched() {
	this->timer = 0;
	this->isGlowing = false;

	// WLClass::instance->flags |= 4;
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;

	// MakeMarioEnterDemoMode();
	StopBGMMusic();

	PlaySound(this, SE_SYS_RED_RING);

	S16Vec nullRot = {0,0,0};
	Vec twoVec = {3.0f, 4.0f, 2.0f};
	if (effTimer == 1) {SpawnEffect("Wm_ob_redringget", 0, &this->pos, &nullRot, &twoVec);}

	if (effTimer == 26) {SpawnEffect("Wm_ob_redringget_a", 0, &this->pos, &nullRot, &twoVec);}

	if (effTimer == 51) {SpawnEffect("Wm_ob_redringget_b", 0, &this->pos, &nullRot, &twoVec);}

	if (effTimer == 76) {SpawnEffect("Wm_ob_redringget_c", 0, &this->pos, &nullRot, &twoVec);}
	
	//Stop order replaced :(
	effTimer++;
}
void dScEndFlag::executeState_Touched() {

	if(this->animationChr.isAnimationDone()) {
		this->visible = false;
	}
	else {
		this->rot.y += 0600;
	}

	bool isOnGround = true;
	for(int i = 0; i < 4; i++) {
		if(Player_Active[i] && Player_Lives[i] > 0) {
			if(!(GetPlayerOrYoshi(i)->collMgr.isOnTopOfTile())) {
				// OSReport("Player %d isn't on ground\n", i);
				isOnGround = false;
			}
		}
	}

	// OSReport("isOnGround: %d, visible: %d\n", isOnGround, visible);

	if(isOnGround && this->visible) {
		this->timer++;
	}

	if(this->timer == 30) {
		doStateChange(&StateID_Anim);
	}
	
}
void dScEndFlag::endState_Touched() {}

void dScEndFlag::beginState_Anim() {
	
	this->timer = 0;
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	// WLClass::instance->flags |= 0xf;

	//PlaySound(this, STRM_BGM_CHALLENGE_CLEAR, 1);
	//FinishRingSoundPlay(1);

	BossGoalForAllPlayers();
	
}
void dScEndFlag::executeState_Anim() {
	
	this->timer++;
	// OSReport("_4 & _8: %d %d\n", WLClass::instance->_4, WLClass::instance->_8);
	// OSReport("flags: %d %d\n", WLClass::instance->flags, *((u32*)(*((u32*)(WLClass::instance->flags)) + 0x10)));

	if(this->timer == 20) {
		WLClass::instance->_4 = 5;
		WLClass::instance->_8 = 0;
	}
	else if(this->timer == 50) {
		UpdateGameMgr();
		*((u8*)((u32)(dCourseClear_c::instance) + 0x375)) = 1;
	}
	else if(this->timer == 230) {
		dGameDisplay_c::instance->_44C = 1;
		*((u8*)((u32)(dCourseClear_c::instance) + 0x376)) = 1;
	}



	// OSReport("PC: %p\n", GetCurrentPC());

	if(this->exiting) {
		this->exiting++;
	}
	//else if(!strcmp(dGameDisplay_c::instance->state.getCurrentState()->getName(), "dGameDisplay_c::StateID_ProcGoalEnd")) { // For some reason, strcmp returns true if the string ISN'T what it was compared to; weird. EDIT: that's actually normal
	else if(dGameDisplay_c::instance->state.getCurrentState() == &dGameDisplay_c::StateID_ProcGoalEnd) {
		this->exiting = 1;
	}

	if(this->exiting == 60 || this->timer == 480) {
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	}
	
}
void dScEndFlag::endState_Anim() {}

int dCourseClear_c::newOnCreate() {
	int orig_val = this->onCreate_orig();

	instance = this;

	return orig_val;
}


class daEnGlpole_c : public dEn_c {
public:
	int onExecute_orig();
	int newOnExecute();
};

int daEnGlpole_c::newOnExecute() {
	int orig_val = this->onExecute_orig();

	// OSReport("GP State: %s\n", this->acState.getCurrentState()->getName());

	return orig_val;
}

class TowerBosses : public dActorMultiState_c {
public:
	void DemoFallEd();
};

void TowerBosses::DemoFallEd() {
	dFlagMgr_c::instance->flags |= ((u64)1 << (45 - 1));
}

/*Sorry, Asu, if you guess I'm doing wrong by copying your codes. Anyway, THAT'S JUST A GAME!!!!HEHEHEHEHEHE
PLEASE COMPILE THESE CODES IF YOU GUYS ARE LOOKING FOR MORE GAMES! */

/*****************************************************************************/
// Playing frickin' sounds

/*
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);
static nw4r::snd::StrmSoundHandle handle;

u8 hijackMusicWithSongName(const char *songName, int themeID, bool hasFast, int channelCount, int trackCount, int *wantRealStreamID);

void FinishRingSoundPlay(int id) {
	//No don't worry
	if (handle.Exists())
		handle.Stop(0);

	int sID;
	// Intended use unknown
	hijackMusicWithSongName(RingsSounds[id], -1, false, 2, 1, &sID);
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, sID, 1);
} */