#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <actors.h>
#include <profile.h>

const char* IceBonesFileList [] = { "karon", NULL };

class daEnKoriKaron_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	bool direction;
	int timer;
	int collTimer;
	int shouldUseRandomSetupButNoChoice;
	
	char damage;
	char isDown;
	Vec initialPos;
	int distance;
	float XSpeed;
	u32 cmgr_returnValue;
	bool isBouncing;
	char hitType;
	//char isDown;
	float disappY;
	float Ypose;
	bool willBodyBeUndone;
	bool isCalculationDone;
	bool isBuilt;
	
	daEnKoriKaron_c *PartsA;
	daEnKoriKaron_c *PartsB;
	daEnKoriKaron_c *PartsC;
	daEnKoriKaron_c *PartsD;
	daEnKoriKaron_c *PartsE;
	daEnKoriKaron_c *PartsF;
	daEnKoriKaron_c *Main;

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModels();
	bool calculateTileCollisions();
	void boneDestroyingAnim();
	void distanceCalculate(ActivePhysics *apOther);
	void isNearPlayer(ActivePhysics *apOther);

	static dActor_c* build();
	
	daBeltNeedle_c *TopSpike;
	u32 calculateBelowCollisionWithSmokeEffect();
	u16 name;
	float EnemyBounceValue;
	void addScoreWhenHit(void *other);
	void powBlockActivated(bool isNotMPGP);
	char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	u32 GenerateRandomNumber(int max);

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
//	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
//	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	Actor *CreateActor(Actors type, u32 settings, VEC3 *pos, void *rot, char layer);
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::mdl_c partsAmodel;
	m3d::mdl_c partsBmodel;
	m3d::mdl_c partsCmodel;
	m3d::mdl_c partsDmodel;
	m3d::mdl_c partsEmodel;
	m3d::mdl_c partsFmodel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;
	nw4r::snd::SoundHandle handle;

	ActivePhysics *apThis;
	ActivePhysics *apOther;
	
	USING_STATES(daEnKoriKaron_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Approach);
	DECLARE_STATE(Freeze);
	DECLARE_STATE(Fire);
	DECLARE_STATE(Hammer);
	DECLARE_STATE(Boomerang);
	DECLARE_STATE(Defeated);
	DECLARE_STATE(Turn);
};

CREATE_STATE(daEnKoriKaron_c, Walk);
CREATE_STATE(daEnKoriKaron_c, Approach);
CREATE_STATE(daEnKoriKaron_c, Freeze);
CREATE_STATE(daEnKoriKaron_c, Fire);
CREATE_STATE(daEnKoriKaron_c, Hammer);
CREATE_STATE(daEnKoriKaron_c, Boomerang);
CREATE_STATE(daEnKoriKaron_c, Defeated);
CREATE_STATE(daEnKoriKaron_c, Turn);

void daEnKoriKaron_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daEnKoriKaron_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daEnKoriKaron_c::setupModels() {
	
	resFile.data = getResource("karon", "g3d/t01.brres");

	if (!willBodyBeUndone) {
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("karon");
		//nw4r::g3d::ResMdl mdlG = Main->resFile.GetResMdl("karon");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&bodyModel, 0);

		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("walk");
		this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
		
	} else {
		nw4r::g3d::ResMdl mdlA = PartsA->resFile.GetResMdl("karon_partsA");
		partsAmodel.setup(mdlA, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsAmodel, 0);
	
		nw4r::g3d::ResMdl mdlB = PartsB->resFile.GetResMdl("karon_partsB");
		partsBmodel.setup(mdlB, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsBmodel, 0);
	
		nw4r::g3d::ResMdl mdlC = PartsC->resFile.GetResMdl("karon_partsC");
		partsCmodel.setup(mdlC, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsCmodel, 0);
	
		nw4r::g3d::ResMdl mdlD = PartsD->resFile.GetResMdl("karon_partsD");
		partsDmodel.setup(mdlD, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsDmodel, 0);
	
		nw4r::g3d::ResMdl mdlE = PartsE->resFile.GetResMdl("karon_partsE");
		partsEmodel.setup(mdlE, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsEmodel, 0);
	
		nw4r::g3d::ResMdl mdlF = PartsF->resFile.GetResMdl("karon_partsF");
		partsFmodel.setup(mdlF, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&partsFmodel, 0);
		
	}
	
	/* nw4r::g3d::ResAnmChr anmChr = Main->resFile.GetResAnmChr("walk");
	Main->animationChr.setup(mdl, anmChr, &Main->allocator, 0); */
	
}

bool daEnKoriKaron_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	if (isBouncing) {
		stuffRelatingToCollisions(0.1875f, 1.0f, 0.5f);
		if (speed.y != 0.0f)
			isBouncing = false;
	}

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		if (cmgr_returnValue == 0)
			isBouncing = true;

		if (speed.x != 0.0f) {
			//playWmEnIronEffect();
		}

		speed.y = 0.0f;

		// u32 blah = collMgr.s_80070760();
		// u8 one = (blah & 0xFF);
		// static const float incs[5] = {0.00390625f, 0.0078125f, 0.015625f, 0.0234375f, 0.03125f};
		// x_speed_inc = incs[one];
		max_speed.x = (direction == 1) ? -XSpeed : XSpeed;
	} else {
		x_speed_inc = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}
		return true;
	}
	return false;
}

void daEnKoriKaron_c::boneDestroyingAnim(){
	this->XSpeed = 0.0;
	willBodyBeUndone = true;
	
	setupModels();
	
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_KALON, 1);
	if (collTimer == 0) {
		PartsA->pos.x = this->pos.x;
		PartsB->pos.x = this->pos.x;
		PartsC->pos.x = this->pos.x;
		PartsD->pos.x = this->pos.x;
		PartsE->pos.x = this->pos.x;
		PartsF->pos.x = this->pos.x;
	}
	if (collTimer < 120) {
		PartsA->pos.x -= 0.1;
		PartsB->pos.x -= 0.5;
		PartsC->pos.x -= 0.2;
		PartsD->pos.x += 0.0;
		PartsE->pos.x += 0.3;
		PartsF->pos.x += 0.5;
		
		if (collTimer < 60) {
			PartsA->Ypose = PartsA->Ypose / 1.075;
			PartsB->Ypose = PartsB->Ypose / 1.075;
			PartsC->Ypose = PartsC->Ypose / 1.075;
			PartsD->Ypose = PartsD->Ypose / 1.075;
			PartsE->Ypose = PartsE->Ypose / 1.075;
			PartsF->Ypose = PartsF->Ypose / 1.075;
			PartsA->pos.y += Ypose;
			PartsB->pos.y += Ypose;
			PartsC->pos.y += Ypose;
			PartsD->pos.y += Ypose;
			PartsE->pos.y += Ypose;
			PartsF->pos.y += Ypose;
		} else if (collTimer == 60) {
			PartsA->Ypose = PartsA->Ypose - PartsA->Ypose * 2;
			PartsB->Ypose = PartsB->Ypose - PartsB->Ypose * 2;
			PartsC->Ypose = PartsC->Ypose - PartsC->Ypose * 2;
			PartsD->Ypose = PartsD->Ypose - PartsD->Ypose * 2;
			PartsE->Ypose = PartsE->Ypose - PartsE->Ypose * 2;
			PartsF->Ypose = PartsF->Ypose - PartsF->Ypose * 2;
		} else {
			PartsA->Ypose = PartsA->Ypose * 1.075;
			PartsB->Ypose = PartsB->Ypose * 1.075;
			PartsC->Ypose = PartsC->Ypose * 1.075;
			PartsD->Ypose = PartsD->Ypose * 1.075;
			PartsE->Ypose = PartsE->Ypose * 1.075;
			PartsF->Ypose = PartsF->Ypose * 1.075;
			PartsA->pos.y += Ypose;
			PartsB->pos.y += Ypose;
			PartsC->pos.y += Ypose;
			PartsD->pos.y += Ypose;
			PartsE->pos.y += Ypose;
			PartsF->pos.y += Ypose;
		}
	} else {this->Delete(1);}
	
	collTimer++;
}

void daEnKoriKaron_c::distanceCalculate(ActivePhysics *apOther) {
	
	if (((this->pos.x - ((dEn_c*)apOther->owner)->pos.x <= 40.0) && (this->pos.x - ((dEn_c*)apOther->owner)->pos.x >= -40.0))
	&& ((this->pos.y - ((dEn_c*)apOther->owner)->pos.y <= 40.0) && (this->pos.x - ((dEn_c*)apOther->owner)->pos.x >= -40.0))) {
		this->doStateChange(&StateID_Approach);
	}
}

void daEnKoriKaron_c::isNearPlayer(ActivePhysics *apOther) {
	
	if (((this->pos.x - ((dEn_c*)apOther->owner)->pos.x <= 40.0) && (this->pos.x - ((dEn_c*)apOther->owner)->pos.x >= -40.0)) &&
	((this->pos.y - ((dEn_c*)apOther->owner)->pos.y <= 40.0)&& (this->pos.x - ((dEn_c*)apOther->owner)->pos.x >= -40.0))) {
		
		if (timer < 210) {
			animationChr.setUpdateRate(2.0f);

			this->pos.x += (direction) ? -1.0 : 1.0;
			this->rot.y = (direction) ? 0xD800 : 0x2800;
			if (this->pos.x > ((dEn_c*)apOther->owner)->pos.x) {this->direction = 1;}
			else {this->direction = 0;}
			
		} else {
			
			Vec3 SpikePos = (Vec){pos.x, pos.y, pos.z};
			S16Vec SpikeRot = (S16Vec){0, 0, 0};
			
			TopSpike = (daBeltNeedle_c*)createChild(BELT_NEEDLE, (dStageActor_c*)this, 0, &SpikePos, &SpikeRot, 0);
			//TopSpike->doStateChange(&daKameckDemo::StateID_DemoSt);
			TopSpike->pos.x = this->pos.x - 16.0;
			TopSpike->pos.y = this->pos.y + 8.0;
			TopSpike->isBuilt = true;
			
			isCalculationDone = true;
			
			this->GenerateRandomNumber(4);
			this->shouldUseRandomSetupButNoChoice += 1;
			if (shouldUseRandomSetupButNoChoice > 4)
				shouldUseRandomSetupButNoChoice = 1;
			
			if (shouldUseRandomSetupButNoChoice == 1) {doStateChange(&StateID_Freeze);}
			if (shouldUseRandomSetupButNoChoice == 2) {doStateChange(&StateID_Fire);}
			if (shouldUseRandomSetupButNoChoice == 3) {
				this->timer = 0;
				bindAnimChr_and_setUpdateRate("throw", 1, 0.0, 1.0);
				doStateChange(&StateID_Hammer);
			}
			if (shouldUseRandomSetupButNoChoice == 4) {
				this->timer = 0;
				bindAnimChr_and_setUpdateRate("throw", 1, 0.0, 1.0);
				doStateChange(&StateID_Boomerang);
			}
			
		}
		
	} else {doStateChange(&StateID_Walk);}
	
	this->timer += 1;
	
}

dActor_c* daEnKoriKaron_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnKoriKaron_c));
	return new(buffer) daEnKoriKaron_c;
}

const SpriteData BlueDryBonesSpriteData = { ProfileId::BlueDryBones, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };

Profile BlueDryBonesProfile(&daEnKoriKaron_c::build, SpriteId::BlueDryBones, &BlueDryBonesSpriteData, ProfileId::BlueDryBones, ProfileId::BlueDryBones, "BlueDryBones", IceBonesFileList);

void daEnKoriKaron_c::addScoreWhenHit(void *other) { }
void daEnKoriKaron_c::powBlockActivated(bool isNotMPGP) { }

void daEnKoriKaron_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	
	if (this->isDown == 0) {
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
	} else {
		hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
	}
	
	if (hitType == 1 || hitType == 3) { // = a regular jump
		this->XSpeed = 0.0;
		bindAnimChr_and_setUpdateRate("big_stop", 1, 0.0, 1.0);
		this->animationChr.setCurrentFrame(0.0);
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_STEP_BIG_KALON, 1);
		
		Vec efPos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 1000.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_explosion_ln", 0, &efPos, &nullRot, &oneVec);
	} else if (hitType == 2) {
		bindAnimChr_and_setUpdateRate("notice_upper", 1, 0.0, 1.0);
	} else if (hitType == 0) {
		this->dEn_c::playerCollision(apThis, apOther);
		
		if (apOther->owner->name != PLAYER) { return; }
		((dEn_c*)apThis->owner)->_vf220(apOther->owner);
		this->_vf220(apOther->owner);
	}
}

void daEnKoriKaron_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	if (name != ((dEn_c*)apOther->owner)->name) {
		name = ((dEn_c*)apOther->owner)->name;
		collTimer = 0;
	}
	
	if (name == EN_LIFT_REMOCON_TRPLN || name == EN_WHITE_BLOCK) {
		this->calculateBelowCollisionWithSmokeEffect();
		this->EnemyBounceValue;
	} else if (name == EN_JUMPDAI) {
		this->calculateBelowCollisionWithSmokeEffect();
		this->EnemyBounceValue;
	} else if (name == AC_BG_LAVA || AC_BG_POISON) {
		
		Vec ef2Pos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 1000.0f};
		S16Vec null2Rot = {0,0,0};
		Vec twoVec = {2.0f, 2.0f, 2.0f};
		SpawnEffect("Wm_en_explosion_ln", 0, &ef2Pos, &null2Rot, &twoVec);
		if (name == AC_BG_LAVA) {PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_CMN_SPLASH_LAVA, 1);}
		else {PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_CMN_SPLASH_POISON, 1);}
		
		this->disappY = this->pos.y;
		this->pos.y = 400.0;
		this->scale.x = 0.0;
		
	}
	
	if (this->scale.x == 0.0) {collTimer++;}
	if (this->collTimer >= 120.0) {
		this->scale.x = 1.0;
		this->pos.y = disappY;
		Vec ef3Pos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 1000.0f};
		S16Vec null3Rot = {0,0,0};
		Vec dreiVec = {2.0f, 2.0f, 2.0f};
		SpawnEffect("Wm_en_explosion_ln", 0, &ef3Pos, &null3Rot, &dreiVec);
		
		this->EnemyBounceValue;
		this->doStateChange(&StateID_Walk);
	}
}

void daEnKoriKaron_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	
	this->aPhysics.removeFromList();
	Vec ef4Pos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 1000.0f};
	S16Vec null4Rot = {0,0,0};
	Vec fourVec = {2.8f, 2.8f, 2.8f};
	SpawnEffect("Wm_en_explosion_ln", 0, &ef4Pos, &null4Rot, &fourVec);
	
	if (TopSpike->isBuilt) {TopSpike->Delete(1);}
	//this->addScoreWhenHit();
	
	PartsA->boneDestroyingAnim();
	PartsB->boneDestroyingAnim();
	PartsC->boneDestroyingAnim();
	PartsD->boneDestroyingAnim();
	PartsE->boneDestroyingAnim();
	PartsF->boneDestroyingAnim();	
}

bool daEnKoriKaron_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {

	this->XSpeed = 0.0;
	if (this->Ypose != 6.0) {
		this->Ypose = 6.0;
		this->collTimer = 0;
	}
	if (this->collTimer == 1) {
		Vec ef5Pos = {this->pos.x + 7.0f, this->pos.y + 14.0f, this->pos.z - 1000.0f};
		S16Vec null5Rot = {0,0,0};
		Vec fiveVec = {2.4f, 2.4f, 2.4f};
		SpawnEffect("Wm_en_explosion_ln", 0, &ef5Pos, &null5Rot, &fiveVec);
		
		this->bindAnimChr_and_setUpdateRate("down", 1, 0.0, 1.0);
		if (TopSpike->isBuilt) {TopSpike->Delete(1);}
		//this->addScoreWhenHit();
	}
	if (this->collTimer == 300) {
		this->bindAnimChr_and_setUpdateRate("revive", 1, 0.0, 1.0);
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_KALON_REVIVAL, 1);
	}
	if (this->collTimer == 360) {PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_KALON_REVIVAL_HEAD, 1);}
	
	if ((this->collTimer > 360) && animationChr.isAnimationDone()) {
		this->doStateChange(&StateID_Walk);
		//this->Ypose = 7.0;
	}
	
	collTimer++;
	
}

bool daEnKoriKaron_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCat3_StarPower(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCatD_Drill(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCat3_StarPower(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCat3_StarPower(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->collisionCat3_StarPower(apThis, apOther);
}

bool daEnKoriKaron_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_EMY_DOWN_NO_SCORE, 1);
	for (int yo = 0; yo < 10; yo++) {StageE4::instance->spawnCoinJump(pos, 0, 2, 0);}
	this->collisionCat3_StarPower(apThis, apOther);
}

/*bool daEnKoriKaron_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
} Freeze sprite */


int daEnKoriKaron_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	setupModels();
	

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;
	this->x_speed_inc = 0.15;
	willBodyBeUndone = false;
	Ypose = 4.5;

	direction = 0;
	shouldUseRandomSetupButNoChoice = 0;
	disappY = 0;
	collTimer = 0;
	isBuilt = false;

	ActivePhysics::Info iceBonePhysics; 
	iceBonePhysics.xDistToCenter = 0.0; 
	iceBonePhysics.yDistToCenter = 12.0; 
	iceBonePhysics.xDistToEdge = 8.0; 
	iceBonePhysics.yDistToEdge = 12.0; 
	iceBonePhysics.category1 = 0x3; 
	iceBonePhysics.category2 = 0x0; 
	iceBonePhysics.bitfield1 = 0x4F; 
	iceBonePhysics.bitfield2 = 0xFFFFFFFF; 
	iceBonePhysics.unkShort1C = 0; 
	iceBonePhysics.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &iceBonePhysics); 
	this->aPhysics.addToList();
	
	isCalculationDone = false;

	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);

	doStateChange(&StateID_Walk);



	this->onExecute();

	return true;
}

int daEnKoriKaron_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	if (!willBodyBeUndone) {bodyModel._vf1C();
	} else {
		partsAmodel._vf1C();
		partsBmodel._vf1C();
		partsCmodel._vf1C();
		partsDmodel._vf1C();
		partsEmodel._vf1C();
		partsFmodel._vf1C();
	}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int daEnKoriKaron_c::onDelete() {
	return true;
}

int daEnKoriKaron_c::onDraw() {
	if (!willBodyBeUndone) {bodyModel.scheduleForDrawing();
	} else {
		partsAmodel._vf1C();
		partsBmodel._vf1C();
		partsCmodel._vf1C();
		partsDmodel._vf1C();
		partsEmodel._vf1C();
		partsFmodel._vf1C();
	}

	return true;
}

//"Extern" Functions
extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
//extern "C" dStageActor_c *CreateActor(u16 classID, int settings, Vec pos, char rot, char layer);
//extern "C" u32 GenerateRandomNumber(int max);
//extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
extern "C" void PlaySoundWithFunctionB4(void *src, nw4r::snd::SoundHandle *handle, int id, int unk); //Recopied
extern "C" nw4r::snd::SoundHandle *PlaySound(dStageActor_c *, int soundID);


/////////////////////////
//// State Time /////////
/////////////////////////

void daEnKoriKaron_c::beginState_Walk() {
	isCalculationDone = false;
	this->timer = 0;
	this->rot.y = (direction) ? 0xD800 : 0x2800;
	
	this->max_speed.x = 0.0;
	this->speed.x = 0.0;
	this->x_speed_inc = 0.0;
	
}
void daEnKoriKaron_c::executeState_Walk() {
	
	if (!collMgr.isOnTopOfTile()) {this->pos.y -= 3.0;}
	else {this->pos.y += 1.0;}
	animationChr.setUpdateRate(1.0f);

	this->pos.x += (direction) ? -0.4 : 0.4;

	if (this->timer > (this->distance * 32)) {
		doStateChange(&StateID_Turn);
	}

	if(this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}
	
	//Easiest distance calculator
	//distanceCalculate(ActivePhysics *apOther);

	this->timer += 1;
}

void daEnKoriKaron_c::endState_Walk() {
}

void daEnKoriKaron_c::beginState_Approach() {
	this->timer = 0;
}
void daEnKoriKaron_c::executeState_Approach() {
	if (!isCalculationDone) { return; }
}
void daEnKoriKaron_c::endState_Approach() {}

void daEnKoriKaron_c::beginState_Freeze() {
	animationChr.setUpdateRate(1.0f);
	bindAnimChr_and_setUpdateRate("big_stop", 1, 0.0, 1.0);
	PlaySound(this, SE_EMY_ICE_BROS_ICE);
}
void daEnKoriKaron_c::executeState_Freeze() {

	// Make sure to use the correct position
	VEC3 IcePos = (VEC3){this->pos.x, this->pos.y, this->pos.z};
	S16Vec IceRot = (S16Vec){0, 0, 0};
	
	// Create And use Fire bros.' iceball
	/*Iceball = (daKameckDemo*)createChild(BROS_ICEBALL, (dStageActor_c*)this, 0, &IcePos, &IceRot, 0);
	Iceball->doStateChange(&daKameckDemo::StateID_DemoSt);
	Iceball->pos.x = this->pos.x - 16.0;
	Iceball->pos.y = this->pos.y + 8.0; */
	
	//CreateActor(BROS_ICEBALL, 0, IcePos, IceRot, 0);
	dStageActor_c::create(BROS_ICEBALL, 0x000, &this->pos, 0, 0);
	
	if(animationChr.isAnimationDone()){
		if (TopSpike->isBuilt) {TopSpike->Delete(1);}
		doStateChange(&StateID_Walk);
	}
}
void daEnKoriKaron_c::endState_Freeze() {}

void daEnKoriKaron_c::beginState_Fire() {
	animationChr.setUpdateRate(1.0f);
	bindAnimChr_and_setUpdateRate("big_stop", 1, 0.0, 1.0);
	PlaySound(this, SE_EMY_FIRE_BROS_FIRE);
}
void daEnKoriKaron_c::executeState_Fire() {

	// Make sure to use the correct position
	VEC3 FirePos = (VEC3){this->pos.x, this->pos.y, this->pos.z};
	S16Vec FireRot = (S16Vec){0, 0, 0};
	
	// Create And use Fire bros.' fireball
	/* Fireball = (daKameckDemo*)createChild(BROS_FIREBALL, (dStageActor_c*)this, 0, &FirePos, &FireRot, 0);
	Fireball->doStateChange(&daKameckDemo::StateID_DemoSt);
	Fireball->pos.x = this->pos.x - 16.0;
	Fireball->pos.y = this->pos.y + 8.0; */
	
	//CreateActor(BROS_FIREBALL, 0, FirePos, FireRot, 0);
	dStageActor_c::create(BROS_FIREBALL, 0x000, &this->pos, 0, 0);
	
	if(animationChr.isAnimationDone()){
		if (TopSpike->isBuilt) {TopSpike->Delete(1);}
		doStateChange(&StateID_Walk);
	}
}
void daEnKoriKaron_c::endState_Fire() {}

void daEnKoriKaron_c::beginState_Hammer() {
	animationChr.setUpdateRate(1.0f);
	if (timer == 60) {PlaySound(this, SE_EMY_HAMMER_BROS_HAMMER);}
	this->timer += 1;
}
void daEnKoriKaron_c::executeState_Hammer() {

	// Make sure to use the correct position
	VEC3 HammerPos = (VEC3){this->pos.x, this->pos.y, this->pos.z};
	S16Vec HammerRot = (S16Vec){0, 0, 0};
	
	// Create And use Hammer bros.' hammers
	/* Hammer = (daKameckDemo*)createChild(HAMMER, (dStageActor_c*)this, 0, &HammerPos, &HammerRot, 0);
	Hammer->doStateChange(&daKameckDemo::StateID_DemoSt);
	Hammer->pos.x = this->pos.x - 16.0;
	Hammer->pos.y = this->pos.y + 8.0; */
	
	//(daEnKoriKaron_c) *CreateActor(115, 0, HammerPos, HammerRot, 0);
	//CreateActor(HAMMER, 0, HammerPos, HammerRot, 0);
	dStageActor_c::create(HAMMER, 0x000, &this->pos, 0, 0);

	if(animationChr.isAnimationDone()){
		if (TopSpike->isBuilt) {TopSpike->Delete(1);}
		doStateChange(&StateID_Walk);
	}
}
void daEnKoriKaron_c::endState_Hammer() {}

void daEnKoriKaron_c::beginState_Boomerang() {
	animationChr.setUpdateRate(1.0f);
	if (timer > 60) {PlaySound(this, SE_EMY_BOOMERANG_BROS_BOOMERANG);}
	this->timer += 1;
}
void daEnKoriKaron_c::executeState_Boomerang() {	

	// Make sure to use the correct position
	VEC3 BoomerangPos = (VEC3){this->pos.x, this->pos.y, this->pos.z};
	S16Vec BoomerangRot = (S16Vec){0, 0, 0};
	
	// Create And use Boomerang bros.' boomerangs
	/* Boomerang = (daKameckDemo*)createChild(BOOMERANG, (dStageActor_c*)this, 0, &BoomerangPos, &BoomerangRot, 0);
	Boomerang->doStateChange(&daKameckDemo::StateID_DemoSt);
	Boomerang->pos.x = this->pos.x - 16.0;
	Boomerang->pos.y = this->pos.y + 8.0; */
	
	//(daEnKoriKaron_c) *CreateActor(108, 0, BoomerangPos, BoomerangRot, 0);
	//CreateActor(BOOMERANG, 0, BoomerangPos, BoomerangRot, 0);
	dStageActor_c::create(BOOMERANG, 0x000, &this->pos, 0, 0);

	if(animationChr.isAnimationDone()){
		if (TopSpike->isBuilt) {TopSpike->Delete(1);}
		doStateChange(&StateID_Walk);
	}
}
void daEnKoriKaron_c::endState_Boomerang() {}

void daEnKoriKaron_c::beginState_Defeated() {}
void daEnKoriKaron_c::executeState_Defeated() {}
void daEnKoriKaron_c::endState_Defeated() {}

void daEnKoriKaron_c::beginState_Turn() {
	this->direction ^= 1;
	this->speed.x = 0.0;
}
void daEnKoriKaron_c::executeState_Turn() {

	if(this->animationChr.isAnimationDone())
		this->animationChr.setCurrentFrame(0.0);

	u16 amt = (this->direction == 0) ? 0x2800 : 0xD800;
	int done = SmoothRotation(&this->rot.y, amt, 0x800);

	if(done) {
		this->doStateChange(&StateID_Walk);
	}
}
void daEnKoriKaron_c::endState_Turn() {}