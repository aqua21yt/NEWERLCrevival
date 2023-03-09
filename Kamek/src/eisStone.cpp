#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <stage.h>
#include <sfx.h>
#include "boss.h"
#include <profile.h>


const char* IceStoneArcList [] = { "eisStone", NULL };

class daStoneBoss_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModels();
	void stunPlayer();
	void unstunPlayers();
	
	int distance;
	char damage;
	int timer;
	float introAnim;
	float handLAnim;
	float handRAnim;
	u16 tempSet;
	u16 name;
	
	//Sprite call settings
	u32 childSettings;
	u32 childID;
	
	daStoneBoss_c* HandLeft;
	daStoneBoss_c* HandRight;

	static daStoneBoss_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	void MakeIntermediaryAnimScene();
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c leftHand;
	m3d::mdl_c rightHand;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daStoneBoss_c);
	DECLARE_STATE(Intro);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Throw);
	DECLARE_STATE(Hit);
	DECLARE_STATE(Earthquake);
	DECLARE_STATE(Punch);
	DECLARE_STATE(Thunder);
	DECLARE_STATE(DefeatChance);
	DECLARE_STATE(FinalHit);
	DECLARE_STATE(Outro);
};

CREATE_STATE(daStoneBoss_c, Intro);
CREATE_STATE(daStoneBoss_c, Wait);
CREATE_STATE(daStoneBoss_c, Throw);
CREATE_STATE(daStoneBoss_c, Hit);
CREATE_STATE(daStoneBoss_c, Earthquake);
CREATE_STATE(daStoneBoss_c, Punch);
CREATE_STATE(daStoneBoss_c, Thunder);
CREATE_STATE(daStoneBoss_c, DefeatChance);
CREATE_STATE(daStoneBoss_c, FinalHit);
CREATE_STATE(daStoneBoss_c, Outro);

void daStoneBoss_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daStoneBoss_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daStoneBoss_c::setupModels() {
	
	//All that from this ressource
	resFile.data = getResource("eisStone", "g3d/eisStone.brres");
	
	this->nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("eisStone");
	this->bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	this->SetupTextures_Boss(&bodyModel, 0);
	
	HandLeft->nw4r::g3d::ResMdl mdlA = HandLeft->resFile.GetResMdl("handL");
	HandLeft->leftHand.setup(mdlA, &allocator, 0x224, 1, 0);
	HandLeft->SetupTextures_Boss(&leftHand, 0);
	
	HandRight->nw4r::g3d::ResMdl mdlB = HandRight->resFile.GetResMdl("handR");
	HandRight->rightHand.setup(mdlB, &allocator, 0x224, 1, 0);
	HandRight->SetupTextures_Boss(&rightHand, 0);
	
	HandLeft->pos.y = -400.0;
	HandLeft->pos.x = 0.0;
	
	HandRight->pos.y = -400.0;
	HandRight->pos.x = 0.0;
	
}

void daStoneBoss_c::stunPlayers() {
	for (int i = 0; i < 4; i++) {
		playerStunned[i] = false;

		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player) {
			if (player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) {
				stunPlayer(player, 1);
				playerStunned[i] = true;
			}
		}
	}
}

void daStoneBoss_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}

daStoneBoss_c* daStoneBoss_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daStoneBoss_c));
	return new(buffer) daStoneBoss_c;
}

const SpriteData ROCK_BOSSSpriteData = 
{ ProfileId::754, 0, 0, 0, 0, 0x700, 0xA00, 0, 0, 0, 0, 0 };

Profile ROCK_BOSSProfile(&daRex_c::build, SpriteId::ROCK_BOSS, &ROCK_BOSSSpriteData, ProfileId::ROCK_BOSS, ProfileId::ROCK_BOSS, "ROCK_BOSS", TrexSarcList);

//////////////////////////////////////////////
///// SPRITE'S REACTIONS TOWARDS PLAYERS /////
//////////////////////////////////////////////

void daStoneBoss_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	((dEn_c*)apThis->owner)->_vf220(apOther->owner);
}

void daStoneBoss_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	name = ((dEn_c*)apOther->owner)->name;
	
	if (name == ){
		
		if (direction) {this->direction = 0;}
		else {this->direction = 1;}
		
		this->pos.x += (direction) ? 4.0 : -4.0;
		
	}
	
}

void daStoneBoss_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool daStoneBoss_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	damage += 10;
	this->doStateChange(&StateID_Hit);
}

bool daStoneBoss_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
	bindAnimChr_and_setUpdateRate("resist", 1, 0.0, 1.0);
	((dEn_c*)apThis->owner)->_vf220(apOther->owner);
}

bool daStoneBoss_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
	this->collisionCatD_Drill(apThis, apOther);
}

bool daStoneBoss_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	damage += 5;
	this->doStateChange(&StateID_Hit);
}

bool daStoneBoss_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
	damage += 2;
	this->doStateChange(&StateID_Hit);
}

bool daStoneBoss_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
	damage += 1;
	this->doStateChange(&StateID_Hit);
}

bool daStoneBoss_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

int daStoneBoss_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);



	allocator.unlink(); 

	introAnim = 4.5;
	
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	/////////////////////////////////
	//// ACTOR SPAWN INSTRUCTIONS ///
	/////////////////////////////////
	
	profileID = (Actors)(0x036); //Koopa Troopa

	tempSet = settings & 0x0000;
	childSettings =
		(tempSet & 3) | ((tempSet & 0xC) << 2) |
		((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
		((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
		((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
	
	//For the body
	ActivePhysics::Info StonePhycs; 
	StonePhycs.xDistToCenter = 0.0; 
	StonePhycs.yDistToCenter = 120.0; 
	StonePhycs.xDistToEdge = 56.0; 
	StonePhycs.yDistToEdge = 120.0; 
	StonePhycs.category1 = 0x3; 
	StonePhycs.category2 = 0x0; 
	StonePhycs.bitfield1 = 0x4F; 
//	StonePhycs.bitfield2 = 0xFFFFFFFF; 
	StonePhycs.bitfield2 = 0xFFF47EF9; // - Yoshi - Ice/Fire player balls - Fence Hit - Pipe Cannon - Hammer
	StonePhycs.unkShort1C = 0; 
	StonePhycs.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &StonePhycs); 
	this->aPhysics.addToList();
	
	//And the hands
	ActivePhysics::Info HandPhyscs; 
	HandPhyscs.xDistToCenter = 8.0; 
	HandPhyscs.yDistToCenter = 24.0; 
	HandPhyscs.xDistToEdge = 16.0; 
	HandPhyscs.yDistToEdge = 24.0; 
	HandPhyscs.category1 = 0x3; 
	HandPhyscs.category2 = 0x0; 
	HandPhyscs.bitfield1 = 0x4F; 
	HandPhyscs.bitfield2 = 0xFFFFFFFF; 
	HandPhyscs.unkShort1C = 0; 
	HandPhyscs.callback = &dEn_c::collisionCallback; 
	
	//+Allocations:
	HandLeft->hlPhysics.initWithStruct(HandLeft, &HandPhyscs); 
	HandLeft->hlPhysics.addToList();
	
	HandRight->hrPhysics.initWithStruct(HandRight, &HandPhyscs); 
	HandRight->hrPhysics.addToList();

	//bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);

	this->timer = 0;
	doStateChange(&StateID_Intro);



	this->onExecute();

	return true;
}

int daStoneBoss_c::onExecute() {
	acState.execute();

	updateModelMatrices();

	if ((damage >= 30 ) && (damage < 40) || damage >= 70) {booModel._vf1C();}
	else {bodyModel._vf1C();}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int daStoneBoss_c::onDelete() {
	return true;
}

int daStoneBoss_c::onDraw() {
	bodyModel.scheduleForDrawing();
	booModel.scheduleForDrawing();

	return true;
}

//Extern Functions////////////////////////////////
extern "C" void *ShakeScreen(void *, unsigned int, unsigned int, unsigned int, unsigned int);
extern "C" u32 GenerateRandomNumber(int max);
/* extern "C" void *StopBGMMusic();
extern "C" void *StartBGMMusic();
extern "C" void *MakeMarioEnterDemoMode();
extern "C" void *MakeMarioExitDemoMode(); */
//////////////////////////////////////////////////

void daStoneBoss_c::beginState_Intro() {
	this->pos.y = -400.0;
	this->pos.z = -2000.0;
	distance = (dEn_c*)apOther->owner)->pos.x - this->pos.x;
	if ((distance < 240) && (distance > 240)) {
		
		////-----INTRO STATE FOR PLAYER-------

		// Stop the BGM Music
		StopBGMMusic();

		// Set the necessary Flags and make Mario enter Demo Mode
		dStage32C_c::instance->freezeMarioBossFlag = 1;
		WLClass::instance->_4 = 4;
		WLClass::instance->_8 = 0;

		MakeMarioEnterDemoMode();

		rot.y = 0x2800; // Y is horizontal axis
		speed.x = 0.0;
		speed.y = 0.0;
		
		////---------------------------------
		
		if (timer == 3 || timer == 6 || timer == 9 || timer == 12 || timer == 15 || timer == 18 ||
		timer == 21 || timer == 24 || timer == 27 || timer == 30) {
			this->ShakeScreen();
		}
		
		if (this->timer == 30) {
			bindAnimChr_and_setUpdateRate("unburry", 1, 0.0, 1.0);
		}
		
		if ((timer >= 30) && (timer <= 150)) {
			introAnim = introAnim / 1.25;
			this->pos.y = this->pos.y + introAnim;
			
			if (timer == 30 || timer == 33 || timer == 36 || timer == 39 || timer == 42 || timer == 45 ||
			timer == 48 || timer == 51 || timer == 54 || timer == 57 || timer == 60){
				collMgr.calculateBelowCollisionWithSmokeEffect();
			}
			
			if (timer == 210) {bindAnimChr_and_setUpdateRate("shout", 1, 0.0, 1.0);}
			
			if (timer == 213 || timer == 216 || timer == 219 || timer == 222 || timer == 225 || timer == 228
			|| timer == 231 || timer == 234 || timer == 237 || timer == 240 || timer == 243 || timer == 246 ||
			timer == 249 || timer == 252 || timer == 255 || timer == 258 || timer == 261 || timer == 264 || timer == 267 || timer == 270){
				collMgr.calculateBelowCollisionWithSmokeEffect();
			}
			
			if (timer == 330) {this->StartBGMMusic();}
			
		}
		
		this->timer++;
		
	}
	
}
void daStoneBoss_c::executeState_Intro() {
	GenerateRandomNumber(4);
	return (randomTimer * 30);
}
void daStoneBoss_c::endState_Intro() {
	MakeMarioExitDemoMode;
	this->doStateChange(&StateID_Wait);
}

void daStoneBoss_c::beginState_Wait() {
	this->timer = 0;
}
void daStoneBoss_c::executeState_Wait() {
	
	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
	
	if (animationChr.isAnimationDone()){
		animationChr.setCurrentFrame(0.0);
	}
	
	if (this->timer >= randomTimer) {
		if (this->damage >= 40) {
			if (randomTimer > 4) {GenerateRandomNumber(3);}
			
			if (randomTimer == 1) {doStateChange(&StateID_Throw);}
			if (randomTimer == 2) {doStateChange(&StateID_Earthquake);}
			if (randomTimer == 3) {doStateChange(&StateID_Punch);}
			
		} else {doStateChange(&StateID_Throw);}
	}
	
	this->timer++;
	
}
void daStoneBoss_c::endState_Wait() {}

void daStoneBoss_c::beginState_Throw() {
	
	if (damage >= 40) {bindAnimChr_and_setUpdateRate("throw_hand", 1, 0.0, 1.0);}
	else {bindAnimChr_and_setUpdateRate("throw", 1, 0.0, 1.0);}
	
	this->timer = 0;

}
void daStoneBoss_c::executeState_Throw() {
	
	if (timer == 90) {
		
		PlaySound(this, SE_EMY_HAMMER_BROS_HAMMER);
		
		//Ready to spawn a Koopa Troopa
		profileID = (Actors)(0x036);
		dStageActor_c *ac = (dStageActor_c*)fBase_c::search(childID);
		dStageActor_c *TroopaKoopa = dStageActor_c::create(profileID, childSettings, &pos, 0, 0); 
		childID = TroopaKoopa->id;
		
		introAnim = 4.5;
		
		Vec efPos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 5500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {1.2f, 1.2f, 1.2f};
		SpawnEffect("Wm_2d_timeupsmoke", 0, &efPos, &nullRot, &efScale);
		
	}
	
	if ((timer >= 90) && (timer <= 120)) {
		introAnim = introAnim / 1.075;
		TroopaKoopa->pos.y += introAnim;
		TroopaKoopa->pos.x -= TroopaKoopa->2.5;
	}
	
	this->timer++;
}
void daStoneBoss_c::endState_Throw() {
	this->doStateChange(&StateID_Wait);
}

void daStoneBoss_c::beginState_Hit() {
}
void daStoneBoss_c::executeState_Hit() {
	
	//Stupid Complex Anim Setups
	if ((damage >= 10) && (damage < 20)) {bindAnimChr_and_setUpdateRate("hit_A1", 1, 0.0, 1.0);}
	else if ((damage >= 20) && (damage < 30)) {bindAnimChr_and_setUpdateRate("hit_A2", 1, 0.0, 1.0);}
	else if ((damage >= 30) && (damage < 40)) {bindAnimChr_and_setUpdateRate("hit_ANoRock", 1, 0.0, 1.0);}
	else if ((damage >= 40) && (damage < 50)) {bindAnimChr_and_setUpdateRate("vulnerable", 1, 0.0, 1.0);}
	else if ((damage >= 50) && (damage < 60)) {bindAnimChr_and_setUpdateRate("hit_B1", 1, 0.0, 1.0);}
	else if ((damage >= 60) && (damage < 70)) {bindAnimChr_and_setUpdateRate("hit_B2", 1, 0.0, 1.0);}
	else if ((damage >= 70) && (damage < 80)) {bindAnimChr_and_setUpdateRate("hit_BNoRock", 1, 0.0, 1.0);}
	else if (damage >= 80) {bindAnimChr_and_setUpdateRate("hit_final", 1, 0.0, 1.0);}
	
	if (!animationChr.isAnimationDone()){
		return false;
	} else {
		if (damage < 80) {
			if ((damage >= 30) && (damage < 40)) {
				doStateChange(&StateID_DefeatChance);
			} else if ((damage >= 40) && (damage < 50)) {
				this->timer = 0;
				this->MakeIntermediaryAnimScene();
			} else {
				doStateChange(&StateID_Wait);
			}
		} else {
			this->timer = 0;
			doStateChange(&StateID_FinalHit);
		}
	}
	
}
void daStoneBoss_c::endState_Hit() {}

void daStoneBoss_c::beginState_Earthquake() {
	this->timer = 0;
	
	handLAnim = 4.5;
	handRAnim = 0.2;
	this->bindAnimChr_and_setUpdateRate("earthquake", 1, 0.0, 1.0);
	HandLeft->bindAnimChr_and_setUpdateRate("hand_hitL", 1, 0.0, 1.0);
	HandRight->bindAnimChr_and_setUpdateRate("hand_hitR", 1, 0.0, 1.0);
	
}
void daStoneBoss_c::executeState_Earthquake() {
	
	if (timer == 40) {
		stunPlayer();
		PlaySound(this, SE_EMY_MEGA_BROS_EARTHQUAKE);
	}
	
	if ((timer > 0) && (timer <= 40)) {
		
		HandLeft->pos.x -= HandLeft->handLAnim;
		HandLeft->pos.y -= HandLeft->handRAnim;
		HandRight->pos.x -= HandRight->handLAnim - 10;
		HandRight->pos.y -= HandRight->handRAnim;
		HandRight->pos.z -= 0x050;
		HandLeft->pos.z -= 0x050;

		handLAnim = handLAnim / 1.075;
		handRAnim = handRAnim * 1.075;
		
	}
	
	if (timer == 120) {
		unstunPlayers();
	}
	
	if ((timer >= 120) && (timer < 160)) {
		
		HandLeft->pos.x += HandLeft->handLAnim;
		HandLeft->pos.y += HandLeft->handRAnim;
		HandRight->pos.x += HandRight->handLAnim - 10;
		HandRight->pos.y += HandRight->handRAnim;
		HandRight->pos.z += 0x050;
		HandLeft->pos.z += 0x050;

		handLAnim = handLAnim * 1.075;
		handRAnim = handRAnim / 1.075;
		
	}
	
	if (timer == 160) {
		doStateChange(&StateID_Wait);
	}
	
	this->timer++;
}
void daStoneBoss_c::endState_Earthquake() {}

void daStoneBoss_c::beginState_Punch() {
	
	this->timer = 0;
	this->bindAnimChr_and_setUpdateRate("punch", 1, 0.0, 1.0);
	HandLeft->bindAnimChr_and_setUpdateRate("punch", 1, 0.0, 1.0);
	HandRight->bindAnimChr_and_setUpdateRate("punch", 1, 0.0, 1.0);
	HandRight->pos.x, pos.y = -100.0, -100.0;
	HandLeft->pos.x, pos.y = -100.0, -100.0;
}
void daStoneBoss_c::executeState_Punch() {
	
	if (timer < 60) {
		
		if (timer % 5 == 0) {
			Vec ef6Pos = {this->pos.x + 7.0f, this->pos.y + 20.0f, this->pos.z - 330.0f};
			Vec ef6Scale = {1.7f, 1.5f, 1.7f};
			SpawnEffect("Wm_mr_cliffcatch_cd", 0, &ef6Pos, &nullRot, &ef6Scale);
		} else if (timer % 5 == 2) {
			Vec ef7Pos = {this->pos.x + 7.0f, this->pos.y + 20.0f, this->pos.z - 330.0f};
			Vec ef7Scale = {1.6f, 1.5f, 1.6f};
			SpawnEffect("Wm_mr_foot_ice", 0, &ef7Pos, &nullRot, &ef7Scale);
		}
		
		HandLeft->pos.y += 100.0 / 60.0;
		HandRight->pos.x += 0.1;
		
	} else if (timer < 170) {
		
		HandLeft->pos.x -= 4.0;
		
		if (timer % 4 == 0) {
			Vec ef8Pos = {HandLeft->pos.x + 7.0f, HandLeft->pos.y + 20.0f, HandLeft->pos.z - 330.0f};
			Vec ef8Scale = {1.4f, 1.4f, 1.4f};
			SpawnEffect("Wm_mr_yssweat", 0, &ef8Pos, &nullRot, &ef8Scale);
		}
		
	} else if (timer > 200) {
		
		if (timer == 290 || timer == 295) {
			this->ShakeScreen();
		}
		
		if (timer == 330) {bindAnimChr_and_setUpdateRate("unpunch", 1, 0.0, 1.0);}
		
		if (timer > 350 && timer < 790) {
			HandLeft->pos.x += 1.0;
			HandLeft->pos.y -= 0.05;
		}
		
		if (timer == 490) {
			doStateChange(&StateID_Wait);
		}
	}
	
	this->timer++;
}
void daStoneBoss_c::endState_Punch() {}

void daStoneBoss_c::beginState_Thunder() {
	this->timer = 0;
	
	//Ready to spawn a Thunder Cloud
	profileID = (Actors)(0x2a6);
	tempSet = settings & 0x0001;
	
	dStageActor_c *ac = (dStageActor_c*)fBase_c::search(childID);
	dStageActor_c *ThunderCloud = dStageActor_c::create(profileID, childSettings, &pos, 0, 0); 
	childID = ThunderCloud->id;
	
	bindAnimChr_and_setUpdateRate("spawn_thunder", 1, 0.0, 1.0);
}
void daStoneBoss_c::executeState_Thunder() {
	
	while (!animationChr.isAnimationDone()){
		//nothing
	}
	
}
void daStoneBoss_c::endState_Thunder() {}

void daStoneBoss_c::beginState_DefeatChance() {
	
	this->timer = 0;
	this->pos.y = 40.0;
	HandLeft->pos.y = -400.0;
	HandRight->pos.y = -400.0;
	this->direction = 0;
}
void daStoneBoss_c::executeState_DefeatChance() {
	
	this->pos.x += (direction) ? -1.0 : 1.0;
	
	if (timer % 5 == 0) {
		Vec ef5Pos = {this->pos.x + 7.0f, this->pos.y + 20.0f, this->pos.z - 330.0f};
		//S16Vec null3Rot = {0,0,0};
		Vec ef5Scale = {1.4f, 1.4f, 1.4f};
		SpawnEffect("Wm_mr_yssweat", 0, &ef5Pos, &nullRot, &ef5Scale);
	}
	
	if (timer == 240) {
		//NOTE: the excepted instruction may not be correct
		profileID = (Actors)(0x085); //Bob-omb
		tempSet = settings & 0xF400; //uses model "t03.brres"
	
		dStageActor_c *ac = (dStageActor_c*)fBase_c::search(childID);
		dStageActor_c *SnowyBobOmb = dStageActor_c::create(profileID, childSettings, &pos, 0, 0); 
		childID = SnowyBobOmb->id;
	}
	
	this->timer++;
}
void daStoneBoss_c::endState_DefeatChance() {}

void daStoneBoss_c::beginState_FinalHit() {
	this->isGlowing = false;

	// WLClass::instance->flags |= 4;
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	
	aPhysics.removeFromList();
	
	MakeMarioEnterDemoMode();
	StopBGMMusic();
	
	if ((timer < 180) && (timer % 5 == 0)) {
		Vec ef3Pos = {this->pos.x + 7.0f, this->pos.y + 20.0f, this->pos.z - 330.0f};
		S16Vec null3Rot = {0,0,0};
		Vec ef3Scale = {1.4f, 1.4f, 1.4f};
		SpawnEffect("Wm_mr_yssweatrun", 0, &ef3Pos, &null3Rot, &ef3Scale);
	}
	
	if ((timer == 200) {
		Vec ef4Pos = {this->pos.x + 8.0f, this->pos.y + 8.0f, this->pos.z + 1000.0};
		Vec ef4Scale = {4.8f, 4.8f, 4.8f};
		SpawnEffect("Wm_2d_timeupsmoke", 0, &ef4Pos, &nullRot, &ef4Scale);
		PlaySound(this, SE_EMY_BH_BOMB);
	}
	
	this->timer++;
}
void daStoneBoss_c::executeState_FinalHit() {
	
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
void daStoneBoss_c::endState_FinalHit() {}

void daStoneBoss_c::beginState_Outro() {
	
	this->timer = 0;
	WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	// WLClass::instance->flags |= 0xf;

	//PlaySound(this, STRM_BGM_CHALLENGE_CLEAR, 1);
	//FinishRingSoundPlay(1);

	BossGoalForAllPlayers();
	
}
void daStoneBoss_c::executeState_Outro() {
	
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
void daStoneBoss_c::endState_Outro() {}

void MakeIntermediaryAnimScene() {
	
	if (timer == 1) {
		MakeMarioEnterDemoMode;
		StopBGMMusic;
		this->aPhysics.removeFromList();
	}
	
	if ((timer < 180) && (timer & 4 == 0)) { //Modulo test
		
		Vec ef2Pos = {this->pos.x + 7.0f, this->pos.y - 30.0f, this->pos.z - 5500.0f};
		S16Vec null2Rot = {0,0,0};
		Vec ef2Scale = {7.0f, 4.0f, 1.0f};
		SpawnEffect("Wm_mr_foot_snow", 0, &ef2Pos, &null2Rot, &ef2Scale);
		
	}
	
	if (timer == 200) {
		Vec ef2Pos = {this->pos.x + 7.0f, this->pos.y - 30.0f, this->pos.z - 5500.0f};
		S16Vec null2Rot = {0,0,0};
		Vec ef2Scale = {7.0f, 4.0f, 1.0f};
		SpawnEffect("Wm_en_sphitsmoke", 0, &ef2Pos, &null2Rot, &ef2Scale);
	}
	
	if (timer == 380) {
		PlaySound(this, SE_OBJ_KAZAN_ERUPTION);
		bindAnimChr_and_setUpdateRate("revival", 1, 0.0, 1.0);
	}
	
	if ((timer >= 380) && (timer < 560) && (timer % 5 == 0)) {this->ShakeScreen();}
	
	if (timer >= 600 && timer < 610) {
		if (timer == 600) {
			MakeMarioExitDemoMode;
			StartBGMMusic;
			
			//Remake the hitboxes
			ActivePhysics::Info StonePhycs; 
			StonePhycs.xDistToCenter = 0.0; 
			StonePhycs.yDistToCenter = 120.0; 
			StonePhycs.xDistToEdge = 56.0; 
			StonePhycs.yDistToEdge = 120.0; 
			StonePhycs.category1 = 0x3; 
			StonePhycs.category2 = 0x0; 
			StonePhycs.bitfield1 = 0x4F; 
			StonePhycs.bitfield2 = 0xFFFFFFFF; 
			aPhysics.addToList();
			
		}
		StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
	}
	
	if (timer == 610) {doStateChange(&StateID_Wait);}
	
	this->timer++;
	
}