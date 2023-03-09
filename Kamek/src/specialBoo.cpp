#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* NewBoosFileList [] = { "teresa", NULL };

class dGreaterBoo_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, m3d::mdl_c modelName);
	void setupModels();

	int type;
	int modelNum;
	int timer;
	u8 facing;
	char randChoice;
	bool haveIShot;
	float neg;
	char dupDir;
	bool isDiagonal;
	u16 name;
	int crushTimer;
	
	dStageActor_c *thrownObject;
	
	static dGreaterBoo_c* build();
	/*
	bool playerStunned[4];
	
	void stunPlayers();
	void unstunPlayers();
	*/
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
	//bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

	lineSensor_s belowSensor;
	
	mHeapAllocator_c allocator;
	m3d::mdl_c normalModel;
	m3d::mdl_c hiddenModel;
	m3d::mdl_c funnyModelA;
	m3d::mdl_c funnyModelB;
	m3d::mdl_c funnyModelC;
	m3d::mdl_c modelName; //REFERENCE VALUE
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(dGreaterBoo_c);
	DECLARE_STATE(InvisibleBoo);
	DECLARE_STATE(InvisibleBooBig);
	DECLARE_STATE(ThrowerBoo);
	DECLARE_STATE(LaserBoo);
	DECLARE_STATE(DuplicatedBoo);
	DECLARE_STATE(FighterBoo);
	DECLARE_STATE(CrusherBoo);
	DECLARE_STATE(Fall);
	
};

CREATE_STATE(dGreaterBoo_c, InvisibleBoo);
CREATE_STATE(dGreaterBoo_c, InvisibleBooBig);
CREATE_STATE(dGreaterBoo_c, ThrowerBoo);
CREATE_STATE(dGreaterBoo_c, LaserBoo);
CREATE_STATE(dGreaterBoo_c, DuplicatedBoo);
CREATE_STATE(dGreaterBoo_c, FighterBoo);
CREATE_STATE(dGreaterBoo_c, CrusherBoo);
CREATE_STATE(dGreaterBoo_c, Fall);

void dGreaterBoo_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	normalModel.setDrawMatrix(matrix);
	normalModel.setScale(&scale);
	normalModel.calcWorld(false);
	
	hiddenModel.setDrawMatrix(matrix);
	hiddenModel.setScale(&scale);
	hiddenModel.calcWorld(false);
	
	funnyModelA.setDrawMatrix(matrix);
	funnyModelA.setScale(&scale);
	funnyModelA.calcWorld(false);
	
	funnyModelB.setDrawMatrix(matrix);
	funnyModelB.setScale(&scale);
	funnyModelB.calcWorld(false);
	
	funnyModelC.setDrawMatrix(matrix);
	funnyModelC.setScale(&scale);
	funnyModelC.calcWorld(false);
}

void dGreaterBoo_c::setupModels(){
	
	resFile.data = getResource("teresa", "g3d/teresa.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("teresaA");
	normalModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&normalModel, 0);
	
	nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("teresaB");
	hiddenModel.setup(mdlA, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&hiddenModel, 0);
	
	nw4r::g3d::ResMdl mdlB = this->resFile.GetResMdl("teresa_otherA");
	funnyModelA.setup(mdlB, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&funnyModelA, 0);
	
	nw4r::g3d::ResMdl mdlC = this->resFile.GetResMdl("teresa_otherB");
	funnyModelB.setup(mdlC, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&funnyModelB, 0);
	
	nw4r::g3d::ResMdl mdlD = this->resFile.GetResMdl("teresa_otherC");
	funnyModelC.setup(mdlD, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&funnyModelC, 0);
	
	//// (No fog, sorry !)	
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	
	nw4r::g3d::ResAnmChr anmChrA = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdlA, anmChrA, &this->allocator, 0);
	
	nw4r::g3d::ResAnmChr anmChrB = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdlB, anmChrB, &this->allocator, 0);
	
	nw4r::g3d::ResAnmChr anmChrC = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdlC, anmChrC, &this->allocator, 0);
	
	nw4r::g3d::ResAnmChr anmChrD = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdlD, anmChrD, &this->allocator, 0);
	
}

void dGreaterBoo_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate, m3d::mdl_c modelName) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->modelName, anmChr, unk);
	this->modelName.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

dGreaterBoo_c* dGreaterBoo_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dGreaterBoo_c));
	return new(buffer) dGreaterBoo_c;
}

////EXTERNS AND FUNCTIONS////
/*
extern "C" void stunPlayer(void *, int);
extern "C" void unstunPlayer(void *);

void dGreaterBoo_c::stunPlayers() {
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

void dGreaterBoo_c::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}
*/
void dGreaterBoo_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	name = ((dEn_c*)apOther->owner)->name;
	
	if (name == EN_SLIP_PENGUIN){
		PlaySound(this, SE_PLY_HIT_BLOCK);
		if (isDiagonal) {
			dupDir += 2;
		}
		else {
			dupDir += 4;
		}
	}
	
	if (name == PLAYER){
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
	
}

void dGreaterBoo_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void dGreaterBoo_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool dGreaterBoo_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playEnemyDownSound1();
	if (type == 1 || type == 5 || type == 6){PlaySound(this, SE_EMY_BIG_TERESA_DEAD);}
	else {PlaySound(this, SE_EMY_TERESA_DEAD);}
	
	Vec ef3Pos = {this->pos.x + 0.0f, this->pos.y + 0.0f, 1000.0f};
	S16Vec null3Rot = {0,0,0};
	Vec one3Vec = {1.0f * this->scale.x, 1.0f * this->scale.y, 1.0f * this->scale.z};
	SpawnEffect("Wm_en_teresatail", 0, &ef3Pos, &null3Rot, &one3Vec);
	this->Delete(1);
	
	return false;
}

bool dGreaterBoo_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool dGreaterBoo_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool dGreaterBoo_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool dGreaterBoo_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

int dGreaterBoo_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);


	setupModels();

	allocator.unlink(); 

	type = settings >> 12 & 0xF; //nybble 9
	modelNum = 0;
	this->timer = 0;
	haveIShot = false;
	dupDir = 0;
	isDiagonal = settings >> 8 & 0xF; //nybble 10
	crushTimer = 0;
	
	randChoice = GenerateRandomNumber(3);
	
	if (type == 1) {
		this->scale.x = 4.4; 
		this->scale.y = 4.4; 
		this->scale.z = 4.4;
	} else if (type == 5 || type == 6) {
		this->scale.x = 2.3; 
		this->scale.y = 2.3; 
		this->scale.z = 2.3;
	} else {
		this->scale.x = 1.2; 
		this->scale.y = 1.2; 
		this->scale.z = 1.2;
	}

	ActivePhysics::Info NBooPhysc;
	
	NBooPhysc.xDistToCenter = 8.0; 
	NBooPhysc.yDistToCenter = 8.0; 
	if (type == 1) {
		NBooPhysc.xDistToEdge = 60.0; 
		NBooPhysc.yDistToEdge = 72.0; 
	} else if (type == 5 || type == 6){
		NBooPhysc.xDistToEdge = 20.0; 
		NBooPhysc.yDistToEdge = 27.0; 
	} else {
		NBooPhysc.xDistToEdge = 12.0; 
		NBooPhysc.yDistToEdge = 15.0; 
	}
	NBooPhysc.category1 = 0x3; 
	NBooPhysc.category2 = 0x0; 
	NBooPhysc.bitfield1 = 0x4F; 
	NBooPhysc.bitfield2 = 0xFFFFFFFF; 
	NBooPhysc.unkShort1C = 0; 
	NBooPhysc.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &NBooPhysc); 
	this->aPhysics.addToList();	

	//bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0, normalModel);

	if (type == 1) {doStateChange(&StateID_InvisibleBooBig);}
	else if (type == 2) {doStateChange(&StateID_ThrowerBoo);}
	else if (type == 3) {doStateChange(&StateID_LaserBoo);}
	else if (type == 4) {doStateChange(&StateID_DuplicatedBoo);}
	else if (type == 5) {doStateChange(&StateID_FighterBoo);}
	else if (type == 6) {doStateChange(&StateID_CrusherBoo);}
	else {doStateChange(&StateID_InvisibleBoo);}
	



	this->onExecute();

	return true;
}

int dGreaterBoo_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	if (modelNum == 2) {
		hiddenModel._vf1C();
	} else if (modelNum == 3) {
		funnyModelA._vf1C();
		//bindAnimChr_and_setUpdateRate("wait_otherA1", 1, 0.0, 1.0, funnyModelA);
	} else if (modelNum == 4) {
		funnyModelB._vf1C();
		//bindAnimChr_and_setUpdateRate("wait_otherB", 1, 0.0, 1.0, funnyModelB);
	} else if (modelNum == 5) {
		funnyModelC._vf1C();
		//bindAnimChr_and_setUpdateRate("wait_otherC", 1, 0.0, 1.0, funnyModelC);
	} else {
		normalModel._vf1C();
		//bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0, normalModel);
	}
	
	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int dGreaterBoo_c::onDelete() {
	return true;
}

int dGreaterBoo_c::onDraw() {
	
	if (modelNum == 2) {hiddenModel.scheduleForDrawing();}
	else if (modelNum == 3) {funnyModelA.scheduleForDrawing();}
	else if (modelNum == 4) {funnyModelB.scheduleForDrawing();}
	else if (modelNum == 5) {funnyModelC.scheduleForDrawing();}
	else {normalModel.scheduleForDrawing();}
	
	return true;
}

void dGreaterBoo_c::beginState_InvisibleBoo() {
	modelNum = randChoice;
	if (modelNum != 3) {modelNum = 5;}
	this->timer = 0;
}
void dGreaterBoo_c::executeState_InvisibleBoo() {
	
	this->timer++;
}
void dGreaterBoo_c::endState_InvisibleBoo() {}

void dGreaterBoo_c::beginState_InvisibleBooBig() {
	modelNum = randChoice;
	if (modelNum != 3) {modelNum = 5;}
	this->timer = 0;
}
void dGreaterBoo_c::executeState_InvisibleBooBig() {
	
	this->timer++;
}
void dGreaterBoo_c::endState_InvisibleBooBig() {}

void dGreaterBoo_c::beginState_ThrowerBoo() {
	this->timer = 0;
	modelNum = 1;
}
void dGreaterBoo_c::executeState_ThrowerBoo() {
	
	if (this->timer == 60) {
		
		this->pos.x = GenerateRandomNumber(480.0) - 240.0;
		this->pos.y = GenerateRandomNumber(480.0) - 240.0;
		
		Vec efPos = {this->pos.x + 0.0f, this->pos.y + 0.0f, 5500.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {2.0f, 2.0f, 2.0f};
		SpawnEffect("Wm_mr_landsmoke", 0, &efPos, &nullRot, &oneVec);
		
		PlaySound(this, SE_BOSS_CMN_MAGIC_EXTINCT);
		
	}
	
	if ((this->timer >= 60) && (this->timer <= 180)) {
		facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

		if (facing != this->direction) {
			this->direction = facing;
			if (this->direction == 1) {
				this->rot.y = 0xD800;
			}
			else {
				this->rot.y = 0x2800;
			}
		}
	}
	
	if (this->timer == 180){
		OSReport(":)");
	}
	
	if (this->timer == 200){
		randChoice = GenerateRandomNumber(5);
		
		if (randChoice == 2) { //IWAO_ROCK
			thrownObject = CreateActor(344, 0, pos, 0, 0);
			PlaySound(this, SE_EMY_KANIBO_THROW);
		}
		else if (randChoice == 3) { //EN_MANHOLE_CHOROPU
			thrownObject = CreateActor(349, 0, pos, 0, 0);
			PlaySound(this, SE_EMY_HAMMER_BROS_HAMMER);
		}
		else if (randChoice == 4) {//KAMECK_MAGIC
			thrownObject = CreateActor(212, 0, pos, 0, 0);
			PlaySound(this, SE_BOSS_KAMECK_MAGIC_MAKE);
		}
		else if (randChoice == 5) { //KOOPA_FIRE
			thrownObject = CreateActor(261, 0, pos, 0, 0);
			PlaySound(this, SE_BOSS_KOOPA_FIRE_SHOT);
		}
		else { //HAMMER
			thrownObject = CreateActor(115, 0, pos, 0, 0);
			PlaySound(this, SE_EMY_HAMMER_BROS_HAMMER);
		}
		
	}
	
	if (this->timer == 240) {
		
	}
	
	if (this->timer == 300) {
		
		Vec ef2Pos = {this->pos.x + 0.0f, this->pos.y + 0.0f, 5500.0f};
		S16Vec null2Rot = {0,0,0};
		Vec one2Vec = {2.0f, 2.0f, 2.0f};
		SpawnEffect("Wm_mr_landsmoke", 0, &ef2Pos, &null2Rot, &one2Vec);
		
		PlaySound(this, SE_BOSS_CMN_MAGIC_EXTINCT);
		this->pos.x -= 3000.0;
		this->pos.y -= 4000.0;
		
	}
	
	this->timer++;
}
void dGreaterBoo_c::endState_ThrowerBoo() {}

void dGreaterBoo_c::beginState_LaserBoo() {
	this->timer = 0;
	haveIShot = false;
	randChoice = GenerateRandomNumber(8);
}
void dGreaterBoo_c::executeState_LaserBoo() {
	this->timer++;
	
	if ((this->timer > 0 && this->timer < 30) || (this->timer > 30 && this->timer < 60) ||
	(this->timer > 60 && this->timer < 90) || (this->timer == 90 && this->timer < 120)){
		if (randChoice == 2) {
			this->pos.x += 1.0;
		}
		else if (randChoice == 3) {
			this->pos.x += 1.0;
			this->pos.y -= 1.0;
		}
		else if (randChoice == 4) {
			this->pos.y -= 1.0;
		}
		else if (randChoice == 5) {
			this->pos.y -= 1.0;
			this->pos.x -= 1.0;
		}
		else if (randChoice == 6) {
			this->pos.x -= 1.0;
		}
		else if (randChoice == 7) {
			this->pos.x -= 1.0;
			this->pos.y += 1.0;
		}
		else if (randChoice == 8) {
			this->pos.y += 1.0;
		}
		else {
			this->pos.x += 1.0;
			this->pos.y += 1.0;
		}
	}
	if (timer == 30 || timer == 60 || timer == 90){
		randChoice = GenerateRandomNumber(8);
	}
	if (this->timer == 180) {
		
        PlaySound(this, SE_EMY_PAKKUN_FIRE);
		S16Vec null4Rot = {0,0,0};
		Vec one4Vec = {1.0f, 1.0f, 1.0f};
        SpawnEffect("Wm_en_explosion", 0, &this->pos, &null4Rot, &one4Vec);
        
        
        this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);
        neg = -1.0;
        if (this->direction == 0) {neg = 1.0;}
        dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 6.0 * neg;
        spawner->speed.y = -2.5;
        spawner->pos.z = 5550.0;
            
        spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 0.0 * neg;
        spawner->speed.y = -6.0;
        spawner->pos.z = 5550.0;
        
        spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 3.5 * neg;
        spawner->speed.y = -6.0;
        spawner->pos.z = 5550.0;

        haveIShot = true;
		
	}
	if (this->timer >= 240){
		this->timer = 0;
	}
	
}
void dGreaterBoo_c::endState_LaserBoo() {}

void dGreaterBoo_c::beginState_DuplicatedBoo() {
	this->timer = 0;
	dupDir = 1;
	modelNum = randChoice;
}
void dGreaterBoo_c::executeState_DuplicatedBoo() {
	
	//AUTOMATIC INTERACTIONS WITH MAGIC PLATFORMS
	if (dupDir == 2) {
		this->pos.x += 1.4;
		this->pos.y -= 1.4;
	}
	else if (dupDir == 3) {
		this->pos.y -= 1.4;
	}
	else if (dupDir == 4) {
		this->pos.x -= 1.4;
		this->pos.y -= 1.4;
	}
	else if (dupDir == 5) {
		this->pos.x -= 1.4;
	}
	else if (dupDir == 6) {
		this->pos.x -= 1.4;
		this->pos.y += 1.4;
	}
	else if (dupDir == 7) {
		this->pos.y += 1.4;
	}
	else if (dupDir == 8) {
		this->pos.x += 1.4;
		this->pos.y += 1.4;
	}
	else {
		this->pos.x += 1.4;
	}
	
	if (randChoice == 2) {
		
	} else if (randChoice == 3) {
		
	} else {
		
	}
	
	this->timer++;
}
void dGreaterBoo_c::endState_DuplicatedBoo() {}

void dGreaterBoo_c::beginState_FighterBoo() {
	this->timer = 0;
	modelNum = 4;
}
void dGreaterBoo_c::executeState_FighterBoo() {
	
	belowSensor.flags = SENSOR_LINE;
	
	belowSensor.lineA = -24 << 12;
	belowSensor.lineB = 24 << 12;
	belowSensor.distanceFromCenter = 0;

	collMgr.init(this, &belowSensor, 0, 0);

	facing = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos);

	if (facing != this->direction) {
		this->direction = facing;
		if (this->direction == 1) {
			this->rot.y = 0xD800;
			this->pos.x -= 2.0;
		}
		else {
			this->rot.y = 0x2800;
			this->pos.x += 2.0;
		}
	}
		
	speed.y = 0.0f;
	y_speed_inc = -0.1875f;
	max_speed.y = -4.0f;

	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_OP_LAND_JR_0983f, 1);
	
	this->timer++;
	
}
void dGreaterBoo_c::endState_FighterBoo() {
	
	HandleYSpeed();
	doSpriteMovement();
	UpdateObjectPosBasedOnSpeedValuesReal();

	if (collMgr.calculateBelowCollision() & (~0x400000)) {
		
		Vec efPos = {this->pos.x + 38.0f, this->pos.y + 32.0f, 5500.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_dossunfall02", 0, &efPos, &nullRot, &oneVec);
		
		this->timer = 0;
		PlaySound(this, SE_EMY_DOSSUN);
	}
	
	this->timer++;
	
}

void dGreaterBoo_c::beginState_CrusherBoo() {
	this->timer = 0;
	crushTimer = 0;
}
void dGreaterBoo_c::executeState_CrusherBoo() {
	//if (this->timer == 180) {unstunPlayers();}

	if (this->timer < 120){modelNum = 3;}
	
	if (this->timer >= 120) {
		
		modelNum = 4;
		
		if (this->timer < 320) {
			this->pos.y += 0.4;
		}
		
		if (this->timer == 370) {
			PlaySound(this, SE_OBJ_ITEM_APPEAR_HAUNT);
			doStateChange(&StateID_Fall);
		}
		
	}
	
	this->timer++;
}
void dGreaterBoo_c::endState_CrusherBoo() {
	crushTimer = 0;
}

void dGreaterBoo_c::beginState_Fall(){
	belowSensor.flags = SENSOR_LINE;
	belowSensor.flags |= SENSOR_10000000 | SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK;
	// 10000000 makes it pass through bricks

	belowSensor.lineA = -24 << 12;
	belowSensor.lineB = 24 << 12;
	belowSensor.distanceFromCenter = 0;

	collMgr.init(this, &belowSensor, 0, 0);

	speed.y = 0.0f;
	y_speed_inc = -0.1875f;
	max_speed.y = -4.0f;

	/*
	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_DEMO_OP_LAND_JR_0983f, 1);
	*/
}
void dGreaterBoo_c::executeState_Fall(){
	HandleYSpeed();
	doSpriteMovement();
	UpdateObjectPosBasedOnSpeedValuesReal();

	if ((collMgr.calculateBelowCollision() & (~0x400000)) && (crushTimer == 0)) {
		
		Vec efPos = {this->pos.x + 0, this->pos.y + 0.0f, 5500.0f};
		S16Vec nullRot = {0,0,0};
		Vec oneVec = {1.0f, 1.0f, 1.0f};
		SpawnEffect("Wm_en_dossunfall01", 0, &efPos, &nullRot, &oneVec);
		
		this->timer = 0;
		PlaySound(this, SE_EMY_DOSSUN);
		//stunPlayers();
		this->crushTimer++;
		
		doStateChange(&StateID_CrusherBoo);
	}
}
void dGreaterBoo_c::endState_Fall(){
}