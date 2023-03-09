#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>

const char* BroozersSZSList [] = { "crasher", "kameck", NULL };

class daBroozerBoss_c : public daBoss {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static dActor_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	void stunPlayers();
	void unstunPlayers();

	bool GrowThis(float initScale, float endScale);
	
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	
	daPlBase_c* players[4];

	int timer;
	bool ret;
	float slideX;
	bool calculatePlayerPresence;
	bool isHurt;
	int delay;
	int slideTimer;
	int delayCounter;
	char damage;
	bool cmgr_returnValue;
	float Baseline;
	bool playerStunned[4];
	int num;
	int rand(void);
	char dying;
	int choice;
	
	///////////////////////////////////
	////// Externs and functions //////
	///////////////////////////////////
	
	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daBroozerBoss_c);
	DECLARE_STATE(Grow);
	DECLARE_STATE(Hit);
	DECLARE_STATE(Attack);
	DECLARE_STATE(Earthquake);
	//DECLARE_STATE(LastHit);
	DECLARE_STATE(Outro);
};

CREATE_STATE(daBroozerBoss_c, Grow);
CREATE_STATE(daBroozerBoss_c, Hit);
CREATE_STATE(daBroozerBoss_c, Attack);
CREATE_STATE(daBroozerBoss_c, Earthquake);
//CREATE_STATE(daBroozerBoss_c, LastHit);
CREATE_STATE(daBroozerBoss_c, Outro);

// Externs go HERE, not in the class

extern "C" u32 GenerateRandomNumber(int max);
extern "C" void unstunPlayer(void *);
extern "C" void stunPlayer(void *, int);
extern "C" void *StageScreen;

bool daBroozerBoss_c::GrowThis(float initScale, float endScale) {
	if (this->scale.x >= endScale) {
		return true;
	}

	this->scale.x += (initScale / 100);

	this->scale = (Vec){this->scale.x, this->scale.x, this->scale.x};
	return false;
}

void daBroozerBoss_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daBroozerBoss_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

dActor_c* daBroozerBoss_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBroozerBoss_c));
	return new(buffer) daBroozerBoss_c;
}

const SpriteData BROOZER_STORMSpriteData = { ProfileId::BROOZER_STORM, 0, 0, 0, 0, 0x300, 0x400, 0, 0, 0, 0, 0 };

Profile BROOZER_STORMProfile(&daBroozerBoss_c::build, SpriteId::BROOZER_STORM, &BROOZER_STORMSpriteData, ProfileId::BROOZER_STORM, ProfileId::BROOZER_STORM, "BROOZER_STORM", BroozersSZSList);

void daBroozerBoss_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	char hit = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);

	if (hit == 1 || hit == 3) {
		if (acState.getCurrentState() != &StateID_Hit) {
			this->damage += 3;

            if (this->damage >= 75) {
		        bindAnimChr_and_setUpdateRate("dead", 1, 0.0, 1.0);
		        doStateChange(&StateID_Outro);
	        }
	        else {
		        bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
		        doStateChange(&StateID_Hit);
	        }
		} else {
            DamagePlayer(this, apThis, apOther);
		}
	}
	else if (hit == 0) {
        DamagePlayer(this, apThis, apOther);
	}
}

void daBroozerBoss_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {DamagePlayer(this, apThis, apOther);}

bool daBroozerBoss_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){

	dActor_c *block = apOther->owner;
	dEn_c *blah = (dEn_c*)block;

	//OSReport("Fuzzy was hit by an actor %d (%p); its field 12C is %x\n", block->name, block, blah->_12C);
	if (block->name == AC_LIGHT_BLOCK || block->name == BLOCK_TARU || block->name == AC_PROP_BLOCK) {
		//OSReport("It's a light block, and its state is %s\n", blah->acState.getCurrentState()->getName());
		if (blah->_12C & 3 || strcmp(blah->acState.getCurrentState()->getName(), "daLightBlock_c::StateID_Throw")) {
			//OSReport("Ignoring this!\n");
			return true;
		}
	}

	/*
	if (blah->direction == 0) { blah->direction = 1; this->roly = 1; }
	else					  { blah->direction = 0; this->roly = 0; }
	*/
	
	blah->speed.x = -blah->speed.x;
	blah->pos.x += blah->speed.x;

	if (blah->speed.y < 0) {
		blah->speed.y = -blah->speed.y; }

	blah->doSpriteMovement();
	blah->doSpriteMovement();

	if (this->damage > 8) {

		if (blah->direction == 0) { blah->direction = 1; }
		else					  { blah->direction = 0; }

		return true;
	}

	this->pos.x += blah->speed.x;

	this->timer = 0;
	this->damage = this->damage + 5;


	PlaySound(this, SE_EMY_BLOW_PAKKUN_DOWN);
	S16Vec nullRot = {0,0,0};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
	SpawnEffect("Wm_mr_kickhit", 0, &blah->pos, &nullRot, &oneVec);

	if (this->damage >= 15) {
		bindAnimChr_and_setUpdateRate("dead", 1, 0.0, 1.0);
		doStateChange(&StateID_Outro);
	}
	else {
		doStateChange(&StateID_Hit);
	}
	return true;
}

bool daBroozerBoss_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
 
	bool daBroozerBoss_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) { return true; } 
	bool daBroozerBoss_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) { return true; }

int daBroozerBoss_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("crasher", "g3d/crasher.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("crasher");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	calculatePlayerPresence = false;
	isHurt = false;
	damage = 0;
	choice = 0;
	
	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 20.0; 
	HitMeBaby.xDistToEdge = 40.0; 
	HitMeBaby.yDistToEdge = 60.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFC01251; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	doStateChange(&StateID_Grow);

	this->onExecute();

	return true;
}

int daBroozerBoss_c::onExecute() {
	acState.execute();
	
	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	for (int i = 0; i < 4; i++)
	{
		this->players[i] = GetPlayerOrYoshi(i);

		if (this->players[i]) {
            if ((!isHurt) && (this->players[i]->pos.x > this->pos.x)) {
		        this->direction = 0;
		        if (this->players[i]->pos.x <= this->pos.x) {this->slideTimer = 0;}
	        }
	        else {
		        this->direction = 1;
		        if (this->players[i]->pos.x > this->pos.x) {this->slideTimer = 0;}
	        }
			break;
		}
	}

	return true;
}

int daBroozerBoss_c::onDelete() {
	return true;
}

int daBroozerBoss_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daBroozerBoss_c::beginState_Grow() {
	this->timer = 0;

	SetupKameck(this, Kameck);
	this->scale = (Vec){1.0, 1.0, 1.0};
}
void daBroozerBoss_c::executeState_Grow() {
    if (timer == 6) { PlaySound(this, SE_BOSS_IGGY_WANWAN_TO_L);  }

	this->timer += 1;
	
	ret = this->GrowThis(1.0, 3.0);

	if (ret) {doStateChange(&StateID_Attack);}
	
}
void daBroozerBoss_c::endState_Grow() {
	this->Baseline = this->pos.y;

	CleanupKameck(this, Kameck);
}

void daBroozerBoss_c::beginState_Hit() {
	aPhysics.removeFromList();
	bindAnimChr_and_setUpdateRate("damage", 1, 0.0, 1.0);
}
void daBroozerBoss_c::executeState_Hit() {
	
	if (animationChr.isAnimationDone()){ //Give back my physics
	
		ActivePhysics::Info HitMeBaby; 
		HitMeBaby.xDistToCenter = 0.0; 
		HitMeBaby.yDistToCenter = 20.0; 
		HitMeBaby.xDistToEdge = 40.0; 
		HitMeBaby.yDistToEdge = 60.0; 
		HitMeBaby.category1 = 0x3; 
		HitMeBaby.category2 = 0x0; 
		HitMeBaby.bitfield1 = 0x4F; 
		HitMeBaby.bitfield2 = 0xFFC01251; 
		HitMeBaby.unkShort1C = 0; 
		HitMeBaby.callback = &dEn_c::collisionCallback; 
		this->aPhysics.initWithStruct(this, &HitMeBaby); 
		this->aPhysics.addToList();
		
		this->doStateChange(&StateID_Attack);
		
	}
}
void daBroozerBoss_c::endState_Hit() {}

void daBroozerBoss_c::beginState_Attack() {
	this->timer = 0;
	slideX = 0.1;
	bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1.0);
	num = ((GenerateRandomNumber(6) % 6) + 1);
	delay = (300 + (num * 20));
	slideTimer = 0;
}
void daBroozerBoss_c::executeState_Attack() {
	if (timer < delay) {
		this->pos.x += (direction) ? -1.4 : 1.4 ;
	} else if (timer == delay) {
		delayCounter = delay;
		num = ((GenerateRandomNumber(6) % 6) + 1);
		delay = 600 + num * 20;
		bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
	} else if (timer < delayCounter + delay){
		if (slideTimer < 30) {
			slideX = slideX * 1.075;
			this->slideTimer++;
		}
		if (this->damage > 8 && timer == 80)
		    this->pos.x += (direction) ? -slideX : slideX;
	}
	this->timer++;
}
void daBroozerBoss_c::endState_Attack() {
	if (damage > 37){
		//choice = GenerateRandomNumber(3);
		this->doStateChange(&StateID_Earthquake);
	}
}

void daBroozerBoss_c::beginState_Earthquake() {
	this->timer = 0;
	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
	this->stunPlayers();
}
void daBroozerBoss_c::executeState_Earthquake() {
	
	if (timer <= 120) {
		if ((timer < 30) && (timer % 5 == 0)) {
			ShakeScreen(StageScreen, 2, 1, 0, 0); 
		}
		animationChr.setCurrentFrame(1.0);
	}
	
	this->timer++;
	
}
void daBroozerBoss_c::endState_Earthquake() {
	unstunPlayers();
	this->doStateChange(&StateID_Attack);
}

/*
void daBroozerBoss_c::beginState_LastHit() {}
void daBroozerBoss_c::executeState_LastHit() {}
void daBroozerBoss_c::endState_LastHit() {}
*/

void daBroozerBoss_c::beginState_Outro() {
	OutroSetup(this);
}
void daBroozerBoss_c::executeState_Outro() {

	if (this->dying == 1) {
		if (this->timer > 180) {
			ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
		}
		if (this->timer == 60) { PlayerVictoryCries(this); }

		this->timer += 1;
		return;
	}

	bool ret;
	ret = ShrinkBoss(this, &this->pos, 2.75, this->timer);

	if (ret == true) 	{
		BossExplode(this, &this->pos);
		this->dying = 1;
		this->timer = 0;
	}

	this->timer += 1;

}
void daBroozerBoss_c::endState_Outro() {}

void daBroozerBoss_c::stunPlayers() {
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

void daBroozerBoss_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}