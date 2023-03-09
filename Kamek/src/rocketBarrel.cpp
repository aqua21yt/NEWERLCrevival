#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>


const char* RBarrelsArcList [] = { "rocketBarrel", NULL };

#define cPlayerOccupying (*(dStageActor_c**)(((u32)(this)) + 0x738 ))

class dEnRocketBarrel_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static dActor_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool isFlightFinished;
	int timer;
	bool isBarrelTriggered;
	bool isBarrelNearGoal;
	float smoothMove;
	u32 buttonPushed;
	int speedTimer;
	int efTimer = 0;
	u16 name;
	
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(dEnRocketBarrel_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Fly);
	DECLARE_STATE(Falls);
};

CREATE_STATE(dEnRocketBarrel_c, Wait);
CREATE_STATE(dEnRocketBarrel_c, Fly);
CREATE_STATE(dEnRocketBarrel_c, Falls);

void dEnRocketBarrel_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void dEnRocketBarrel_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

dActor_c* dEnRocketBarrel_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dEnRocketBarrel_c));
	return new(buffer) dEnRocketBarrel_c;
}
/*
const SpriteData BARREL_CANNONSpriteData = { ProfileId::BARREL_CANNON, 0, 0, 0, 0, 0x150, 0x150, 0, 0, 0, 0, 0 };

Profile BARREL_CANNONProfile(&dEnRocketBarrel_c::build, SpriteId::BARREL_CANNON, &BARREL_CANNONSpriteData, ProfileId::BARREL_CANNON, ProfileId::BARREL_CANNON, "BARREL_CANNON", RBarrelsArcList);
*/
void dEnRocketBarrel_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	if (apOther->owner->name == PLAYER) {
		dAcPy_c* player = (dAcPy_c*)apOther->owner;
		isBarrelTriggered = true;
		
		if (!isFlightFinished) {
			
			player->pos.x = this->pos.x;
			player->pos.y = this->pos.y;
			player->scale.x = 0.0;
			player->scale.y = 0.0;
			player->scale.z = 0.0;
			
		}
		
	}
	
	return;
}

void dEnRocketBarrel_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	
	name = ((dEn_c*)apOther->owner)->name;
	
	//The barrel will interact with the sprites down below:
	if (name == 51 || name == 52 || name == 53 || name == 54 || name == 55 || name == 56
	|| name == 348 || name == 144 || name == 81 || name == 82 || name == 83 || name == 87
	|| name == 91 || name == 92 || name == 93 || name == 94 || name == 95 || name == 96
	|| name == 97 || name == 98 || name == 99 || name == 100 || name == 101 || name == 102
	|| name == 103 || name == 106 || name == 107 || name == 174 || name == 175
	|| name == 127 || name == 128 || name == 129 || name == 130 || name == 131 || name == 133
	|| name == 134 || name == 135 || name == 196 ) {
		
		if (apOther->owner->name == PLAYER) {
			dAcPy_c* player = (dAcPy_c*)apOther->owner;
			
			PlaySound(this, SE_EMY_KERONPA_BOMB);
			
			Vec ef3Pos = {pos.x - 0.0f, pos.y - 0.0f, pos.z + 2000.0f};
			S16Vec null3Rot = {0,0,0};
			Vec ef3Scale = {4.0f, 4.0f, 1.0f};
			SpawnEffect("Wm_en_bombheibreak", 0, &ef3Pos, &null3Rot, &ef3Scale);
			
			killByDieFall(this);
			
			aPhysics.removeFromList();
			this->Delete(1);
			
		}
		
	}
	
}

void dEnRocketBarrel_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

bool dEnRocketBarrel_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool dEnRocketBarrel_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool dEnRocketBarrel_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool dEnRocketBarrel_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool dEnRocketBarrel_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool dEnRocketBarrel_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool dEnRocketBarrel_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool dEnRocketBarrel_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

int dEnRocketBarrel_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	isFlightFinished = false;
	isBarrelTriggered = false;
	smoothMove = 4.0;
	isBarrelNearGoal = false;
	speedTimer = 0;
	
	resFile.data = getResource("rocketBarrel", "g3d/rocketBarrel.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("rocketBarrel");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	ActivePhysics::Info BarrelBbb; 
	BarrelBbb.xDistToCenter = 0.0; 
	BarrelBbb.yDistToCenter = 0.0; 
	BarrelBbb.xDistToEdge = 12.0; 
	BarrelBbb.yDistToEdge = 15.0; 
	BarrelBbb.category1 = 0x3; 
	BarrelBbb.category2 = 0x0; 
	BarrelBbb.bitfield1 = 0x4F; 
	BarrelBbb.bitfield2 = 0xFFFFFFFF; 
	BarrelBbb.unkShort1C = 0; 
	BarrelBbb.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &BarrelBbb); 
	this->aPhysics.addToList();

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;
	
	this->rot.z = 0x0000; //Initial direction

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	doStateChange(&StateID_Wait);


	this->onExecute();

	return true;
}

int dEnRocketBarrel_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	if (isBarrelTriggered){
		if (efTimer == 5) {
		
			efTimer = 0;
		
			Vec efPos = {pos.x - 16.0f, pos.y - 2.0f, pos.z + 100.0f};
			S16Vec nullRot = {0,0,0};
			Vec efScale = {2.0f, 0.0f, 2.0f};
			SpawnEffect("Wm_en_burst_s", 0, &efPos, &nullRot, &efScale);
			
		} else {
			this->efTimer++;
		}
	}
	
	return true;
}

int dEnRocketBarrel_c::onDelete() {
	return true;
}

int dEnRocketBarrel_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void dEnRocketBarrel_c::beginState_Wait() {}
void dEnRocketBarrel_c::executeState_Wait() {
	if (isBarrelTriggered) {
		
		if (timer == 1) {
			StopBGMMusic();
			MakeMarioEnterDemoMode();
		}
		
		if (timer >= 180 && timer <= 240) {
			
			this->rot.z -= 0x0110;
			this->pos.y += smoothMove;
			this->pos.x += 2.0;
			smoothMove = smoothMove / 1.075;
			
		}
		
		if (timer == 180) {StartBGMMusic();}
		
		if (timer > 240) {
			PlaySound(this, SE_EMY_KG_KILLER_FLY);
			doStateChange(&StateID_Fly);
		}
		
		this->timer++;
	}
}
void dEnRocketBarrel_c::endState_Wait() {}

void dEnRocketBarrel_c::beginState_Fly() {}
void dEnRocketBarrel_c::executeState_Fly() {
	if (!isBarrelNearGoal){
		
		buttonPushed = Remocon_GetPressed(GetActiveRemocon());
		
		this->pos.x += 2.0;
		
		if (buttonPushed & 0x0002) {
			this->pos.y += 6.5;
		} else if (buttonPushed & 0x0001) {
			this->pos.y -= 6.5;
		} else if (buttonPushed & 0x0100) {
			
			Vec ef2Pos = {pos.x - 24.0f, pos.y - 0.0f, pos.z + 100.0f};
			S16Vec null2Rot = {0,0,0};
			Vec ef2Scale = {2.5f, 0.5f, 2.5f};
			SpawnEffect("Wm_mr_spindown_a", 0, &ef2Pos, &null2Rot, &ef2Scale);
			
			this->pos.x += 5.0;
			
		}
		
	} else {doStateChange(&StateID_Falls);}
}
void dEnRocketBarrel_c::endState_Fly() {}

void dEnRocketBarrel_c::beginState_Falls() {}
void dEnRocketBarrel_c::executeState_Falls() {}
void dEnRocketBarrel_c::endState_Falls() {}