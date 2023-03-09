#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* GoldBlocksArcFileList [] = { "block_gold", NULL };

class daGoldBlock_c : public daEnBlockMain_c {
public:

	TileRenderer brickTile;
	Physics::Info physicsInfo;
	
	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);
	
	u32 enitemsettings;
	u32 powerupToSet;
	u16 name;
	u32 coinsettings; //Coins settings (which are EN_ITEM variant)
	int timer;
	bool areHitsDone;
	bool lockHits;
	bool amItouched;
	int distPosX;
	int distPosY;	
	char coinsPoppedUp;
	char totalCoinsToPopUp;
	
	virtual bool isOutOfView();
	virtual void willBeDeleted();
	
	float wasIHit;
	
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	

	static daGoldBlock_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	//bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

	USING_STATES(daGoldBlock_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Transform);
	DECLARE_STATE(Held);
};

CREATE_STATE(daGoldBlock_c, Wait);
CREATE_STATE(daGoldBlock_c, Transform);
CREATE_STATE(daGoldBlock_c, Held);

void daGoldBlock_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

daGoldBlock_c* daGoldBlock_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daGoldBlock_c));
	return new(buffer) daGoldBlock_c;
}

void daGoldBlock_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	name = ((dEn_c*)apOther->owner)->name;
	
	if (name == PLAYER) {
		dAcPy_c* player = (dAcPy_c*)apOther->owner;
		this->pos.x = player->pos.x + 8;
		this->pos.y = player->pos.y + 8;
		this->direction = player->direction;
		this->rot.y = (direction) ? 0xD800 : 0x2800;
		this->amItouched = true;
		
		if (coinsPoppedUp >= totalCoinsToPopUp) {
			Vec ef2Pos = {this->pos.x, this->pos.y, this->pos.z + 2000.0f};
			S16Vec null2Rot = {0,0,0};
			Vec ef2Scale = {1.5f, 1.5f, 1.5f};
			SpawnEffect("Wm_en_blockcloud", 0, &ef2Pos, &null2Rot, &ef2Scale);
			
			PlaySound(this, SE_SYS_NICE_S);
			this->Delete(1);
		}
	}
}

void daGoldBlock_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){	
}

void daGoldBlock_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

int daGoldBlock_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->timer = 0;
	this->areHitsDone = false;
	this->lockHits = false;
	distPosX = this->pos.x;
	distPosY = this->pos.y;
	totalCoinsToPopUp = ((this->settings >> 16 & 0xF) + 1) * 10; //nybble 8
	
	resFile.data = getResource("block_gold", "g3d/block_gold.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("block_gold");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	
	SetupTextures_Player(&bodyModel, 0);
	blockInit(pos.y);

	physicsInfo.x1 = -8;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 8;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->add(&brickTile);

	brickTile.x = pos.x - 8;
	brickTile.y = -(16 + pos.y);
	brickTile.tileNumber = 0x30;
	
	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;



	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int daGoldBlock_c::onExecute() {

	if (areHitsDone) {
		updateModelMatrices();
		bodyModel._vf1C();
	} else {
		acState.execute();
		physics.update();
		blockUpdate();

		brickTile.setPosition(pos.x-8, -(16+pos.y), pos.z);
		brickTile.setVars(scale.x);
	}
	
	this->timer++;
	
	return true;
}

int daGoldBlock_c::onDelete() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&brickTile);
	
	return true;
}

int daGoldBlock_c::onDraw() {
	if (areHitsDone) {bodyModel.scheduleForDrawing();}

	return true;
}

void daGoldBlock_c::beginState_Wait() {
}

void daGoldBlock_c::endState_Wait() {
}

void daGoldBlock_c::executeState_Wait() {
	if (!lockHits) {
		if (!areHitsDone){
			if(this->wasIHit < 1.0f) {
				int result = blockResult();

				if (result == 0)
					return;

				if (result == 1) {
					doStateChange(&daEnBlockMain_c::StateID_UpMove);
					anotherFlag = 2;
					isGroundPound = false;
				} else {
					doStateChange(&daEnBlockMain_c::StateID_DownMove);
					anotherFlag = 1;
					isGroundPound = true;
				}
				
				if (this->timer > 60) {
					brickTile.tileNumber = 0x32;
					lockHits = true;
				}
				
				this->timer = 0;
				
			} else {
				areHitsDone = true;
			}
		} else {
			doStateChange(&StateID_Transform);
		}
	}
}

void daGoldBlock_c::beginState_Transform() {
	TileRenderer::List *list = dBgGm_c::instance->getTileRendererList(0);
	list->remove(&brickTile);	

	// Transform a standard brick block into a programmed-hitbox-interactable brick block sprite
	ActivePhysics::Info GBBlockPhysc; 
	GBBlockPhysc.xDistToCenter = 0.0; 
	GBBlockPhysc.yDistToCenter = 12.0; 
	GBBlockPhysc.xDistToEdge = 8.0; 
	GBBlockPhysc.yDistToEdge = 12.0; 
	GBBlockPhysc.category1 = 0x3; 
	GBBlockPhysc.category2 = 0x0; 
	GBBlockPhysc.bitfield1 = 0x4F; 
	GBBlockPhysc.bitfield2 = 0xFFFFFFFF; 
	GBBlockPhysc.unkShort1C = 0; 
	GBBlockPhysc.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &GBBlockPhysc); 
	this->aPhysics.addToList();
	
	this->timer = 0;
}
void daGoldBlock_c::executeState_Transform() {
	OSReport("I'm gonna wait until a player touches me :)");
	if (amItouched) {doStateChange(&StateID_Held);}
	this->timer++;
}
void daGoldBlock_c::endState_Transform() {}

void daGoldBlock_c::beginState_Held() {
	this->timer = 0;
	distPosX = pos.x;
	distPosY = pos.y;
}
void daGoldBlock_c::executeState_Held() {
	
	if (((this->pos.x - distPosX < 40.0) && (this->pos.x - distPosY > 40.0)) || ((this->pos.y - distPosY < 40.0) && (this->pos.y - distPosY > 40.0))){
		coinsPoppedUp++;
		Vec NewCoinPos = (Vec){this->pos.x + 0, this->pos.y + 8, this->pos.z}; //Setting non-GP coinRight position
		coinsettings = 0 | (0x2 << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (playerID + 8 << 16); //Setting non-GP settings
		dStageActor_c *coinPlus = dStageActor_c::create(EN_ITEM, coinsettings, &NewCoinPos, 0, 0); //Creating coin

	}
	
	if (this->timer % 100 == 0) {
		Vec efPos = {this->pos.x, this->pos.y, this->pos.z + 2000.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {0.8f, 0.8f, 0.8f};
		SpawnEffect("Wm_2d_coinlight", 0, &efPos, &nullRot, &efScale);
	}
	
	this->timer++;
}
void daGoldBlock_c::endState_Held() {}

//////////////////////////////////////
////////// OTHER BLOCK STUFF /////////
//////////////////////////////////////

extern "C" int CheckExistingPowerup(void * Player);

void daGoldBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;
	
	int p = CheckExistingPowerup(dAcPy_c::findByID(playerID)); // Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
	
	Vec coinPos = (Vec){this->pos.x + 0, this->pos.y + 8, this->pos.z}; //Setting non-GP coinRight position
	
	nw4r::snd::SoundHandle handle; //Sound Handle
	
	/*
	u32 enitemsettings; //EN_ITEM settings
	u32 reggieToIG = 0x2; //Mushroom, Star, Coin, 1UP, Fire Flower, Ice Flower, Penguin, Propeller, Mini Shroom, Hammer, 10 Coins
	u32 powerupToSet = reggieToIG; //Spritedata ID to EN_ITEM ID
	enitemsettings = 0 | (powerupToSet << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (playerID + 8 << 16); //Setting non-GP settings
	*/
	
	coinsettings = 0 | (0x2 << 0) | (2 << 18) | (4 << 9) | (2 << 10) | (playerID + 8 << 16); //Setting non-GP settings
	
	if(isGroundPound) { //If the player groundpounded the block, change some setting and positions
		enitemsettings = 0 | (powerupToSet << 0) | (3 << 18) | (4 << 9) | (2 << 10) | (playerID + 8 << 16); //Changing value 0xD98 from 2 to 3
		coinsettings = 0 | (0x2 << 0) | (3 << 18) | (4 << 9) | (2 << 10) | (playerID + 8 << 16); //Changing value 0xD98 from 2 to 3
		coinPos = (Vec){this->pos.x + 0, this->pos.y - 8, this->pos.z}; //Changing pos.y
	}
	dStageActor_c *coin = dStageActor_c::create(EN_ITEM, coinsettings, &coinPos, 0, 0); //Creating coin
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_GET_COIN, 1); //Coin sound

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.addToList();
	
	this->wasIHit += 0.1f;
	
	doStateChange(&StateID_Wait); //Back to waiting state
}



void daGoldBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daGoldBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}