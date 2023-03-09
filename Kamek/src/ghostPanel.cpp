#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* PosterSarcList [] = { "obake_painting", NULL };

class daMansionPanel : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModel();

	static daMansionPanel* build();
	
	int timer;
	int randChoice;
	bool hasCollided;
	int collTimer;
	int actType;
	int action;

	//Called to spawn a power-up:
	u32 enitemsettings;
	static u32 reggieToIG[];
	u32 powerupToSet;
	int playerID;
	u16 tempSet;
	u32 childSettings;
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	//bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
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


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	m3d::anmTexPat_c animationPat;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daMansionPanel);
	DECLARE_STATE(GiveCoins);
	//DECLARE_STATE(AspiratePlayer);
	DECLARE_STATE(GiveRandomPower);
	DECLARE_STATE(SpawnsEnemies);
	
};

CREATE_STATE(daMansionPanel, GiveCoins);
//CREATE_STATE(daMansionPanel, AspiratePlayer);
CREATE_STATE(daMansionPanel, GiveRandomPower);
CREATE_STATE(daMansionPanel, SpawnsEnemies);

void daMansionPanel::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daMansionPanel::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void daMansionPanel::setupModel(){
	
	resFile.data = getResource("obake_painting", "g3d/obake_painting.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("obake_painting");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);
	
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
	
	nw4r::g3d::ResAnmTexPat anmA = this->resFile.GetResAnmTexPat("obake_painting");
	this->animationPat.setup(mdl, anmA, &this->allocator, 0, 0);
	
}

daMansionPanel* daMansionPanel::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daMansionPanel));
	return new(buffer) daMansionPanel;
}

void daMansionPanel::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	if (apOther->owner->name == PLAYER){
		dAcPy_c* player = (dAcPy_c*)apOther->owner;
		
		if (actType == 1){
			if (this->collTimer < 180){
				player->scale.x -= 1 / 180;
				player->scale.y -= 1 / 180;
				player->scale.z -= 1 / 180;
				player->pos.x = this->pos.x;
				player->pos.y = this->pos.y;
			}
			if (this->collTimer == 180){
				if (action > 0){
					
					tempSet = this->settings & 0xFFFF;
					childSettings =
						(tempSet & 3) | ((tempSet & 0xC) << 2) |
						((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
						((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
						((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
					
					dStageActor_c::create(AC_NEXTGOTO_BLOCK, childSettings, &this->pos, 0, 0);
					
				} else {killByDieFall(this);}
				
				hasCollided = true;
			}
		}
		else if (actType == 2){this->doStateChange(&StateID_GiveRandomPower);}
		else if (actType == 3){this->doStateChange(&StateID_SpawnsEnemies);}
		else {this->doStateChange(&StateID_GiveCoins);}
		
	}
}

void daMansionPanel::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daMansionPanel::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool daMansionPanel::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMansionPanel::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMansionPanel::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daMansionPanel::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

int daMansionPanel::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	actType = settings >> 12 & 0xF; //nybble 7
	action = settings >> 8 & 0xF; //nybble 8
	
	this->collTimer = 0;
	hasCollided = false;
	setupModel();

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	randChoice = GenerateRandomNumber(15);
	this->timer = 0;
	
	ActivePhysics::Info PntngPhyscs; 
	PntngPhyscs.xDistToCenter = 0.0; 
	PntngPhyscs.yDistToCenter = 0.0; 
	PntngPhyscs.xDistToEdge = 18.0; 
	PntngPhyscs.yDistToEdge = 36.0; 
	PntngPhyscs.category1 = 0x3; 
	PntngPhyscs.category2 = 0x0; 
	PntngPhyscs.bitfield1 = 0x4F; 
	PntngPhyscs.bitfield2 = 0xFFFFFFFF; 
	PntngPhyscs.unkShort1C = 0; 
	PntngPhyscs.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &PntngPhyscs); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	//doStateChange(&StateID_GiveCoins);

	this->onExecute();

	return true;
}

int daMansionPanel::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();

	if (hasCollided) {animationPat.setCurrentFrame(8.0);}
	else {animationPat.setCurrentFrame(action);}
	
	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int daMansionPanel::onDelete() {
	return true;
}

int daMansionPanel::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daMansionPanel::beginState_GiveCoins() {
	this->timer = 0;
	aPhysics.removeFromList();
}
void daMansionPanel::executeState_GiveCoins() {
	
	if (this->timer < randChoice * 4){
		StageE4::instance->spawnCoinJump(this->pos, 0, 2, 0);
	}
	
	if (this->timer == randChoice + 1) {
		hasCollided = true;
		Vec efPos = {pos.x + 0.0f, pos.y + 0.0f, pos.z - 0.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {3.5f, 3.5f, 1.0f};
		SpawnEffect("Wm_2d_timeupsmoke", 0, &efPos, &nullRot, &efScale);
	}
	
	this->timer++;
}
void daMansionPanel::endState_GiveCoins() {}
/*
void daMansionPanel::beginState_AspiratePlayer() {}
void daMansionPanel::executeState_AspiratePlayer() {}
void daMansionPanel::endState_AspiratePlayer() {}
*/
void daMansionPanel::beginState_GiveRandomPower() {
	this->timer = 0;
}
void daMansionPanel::executeState_GiveRandomPower() {
	if (this->timer == 0) {
		
		u32 enitemsettings; //EN_ITEM settings
		u32 reggieToIG[] = {0x0,0x1,0x2,0x7,0x9,0xE,0x11,0x15,0x19,0x6,0x2}; //Mushroom, Star, Coin, 1UP, Fire Flower, Ice Flower, Penguin, Propeller, Mini Shroom, Hammer, 10 Coins
		powerupToSet = reggieToIG[randChoice]; //Spritedata ID to EN_ITEM ID
		enitemsettings = 0 | (powerupToSet << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (this->playerID + 8 << 16); //Setting non-GP settings
		
		dStageActor_c *power = dStageActor_c::create(EN_ITEM, enitemsettings, &this->pos, 0, 0);
		
		hasCollided = true;
		
	}
	this->timer++;
}
void daMansionPanel::endState_GiveRandomPower() {}

void daMansionPanel::beginState_SpawnsEnemies() {
	
	if (action == 1) {dStageActor_c::create(EN_NOKONOKO, 0, &this->pos, 0, 0);} //Koopa Troopa
	else if (action == 2) {dStageActor_c::create(EN_MET, 0, &this->pos, 0, 0);} //Buzzy Beetle
	else if (action == 3) {dStageActor_c::create(EN_TOGEZO, 0, &this->pos, 0, 0);} //Spiny
	else if (action == 4) {dStageActor_c::create(EN_TERESA, 0, &this->pos, 0, 0);} //Boo
	else if (action == 5) {dStageActor_c::create(EN_BIG_TERESA, 0, &this->pos, 0, 0);} //Big Boo
	else if (action == 6) {dStageActor_c::create(EN_CHOROPU, 0, &this->pos, 0, 0);} //Monty Mole
	else if (action == 7) {dStageActor_c::create(EN_GABON, 0, &this->pos, 0, 0);} //Spike
	else if (action == 8) {dStageActor_c::create(EN_KANIBO, 0, &this->pos, 0, 0);} //Crab
	else if (action == 9) {dStageActor_c::create(EN_KARON, 0, &this->pos, 0, 0);} //Dry Bones
	else if (action == 10) {dStageActor_c::create(EN_BIGKARON, 0, &this->pos, 0, 0);} //Big Dry Bones
	else if (action == 11) {dStageActor_c::create(EN_HANACHAN, 0, &this->pos, 0, 0);} //Wiggler
	else if (action == 12) {dStageActor_c::create(EN_CRASHER, 0, &this->pos, 0, 0);} //Broozer
	else if (action == 13) {dStageActor_c::create(EN_HAMMERBROS, 0, &this->pos, 0, 0);} //Hammer bro
	else if (action == 14) {dStageActor_c::create(EN_BOOMERANGBROS, 0, &this->pos, 0, 0);} //Boomerang bro
	else if (action == 15) {dStageActor_c::create(EN_FIREBROS, 0, &this->pos, 0, 0);} //Fire bro
	else {dStageActor_c::create(EN_KURIBO, 0, &this->pos, 0, 0);} //Goomba
	
	hasCollided = true;
	Vec ef2Pos = {pos.x + 0.0f, pos.y + 0.0f, pos.z - 0.0f};
	S16Vec null2Rot = {0,0,0};
	Vec ef2Scale = {3.5f, 3.5f, 1.0f};
	SpawnEffect("Wm_2d_timeupsmoke", 0, &ef2Pos, &null2Rot, &ef2Scale);
	
}
void daMansionPanel::executeState_SpawnsEnemies() {}
void daMansionPanel::endState_SpawnsEnemies() {}