#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>

const char* CoinRuletSrcList [] = { "block_roulette_coin", NULL };

class EnCoinRoulette_c : public daEnBlockMain_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	bool isModelEmpty;
	
	Physics::Info physicsInfo;

	void updateModelMatrices();
	//void bindAnimTexPat_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModels();
	
	int result;
	int timer = 0;
	bool isBig;
	int coinSpawned;
	u16 tempSet;
	u32 childSettings;
	int hitTimer;
	int mdlTimer;
	char whatMdl;

	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);
	
	static EnCoinRoulette_c* build();


	mHeapAllocator_c allocator;
	m3d::mdl_c CoinFiv;
	m3d::mdl_c CoinTen;
	m3d::mdl_c CoinThy;
	m3d::mdl_c CoinFifty;
	m3d::mdl_c CoinFiftyBig;
	m3d::mdl_c CoinHdrd;
	m3d::mdl_c emptyModel;
	m3d::anmChr_c animationChr;
//	m3d::anmTexPat_c animationPat;
//	m3d::anmTexPat_c animationBig;
	nw4r::g3d::ResFile resFile;

	USING_STATES(EnCoinRoulette_c);
	DECLARE_STATE(Wait);
};

CREATE_STATE(EnCoinRoulette_c, Wait);

void EnCoinRoulette_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	CoinFiv.setDrawMatrix(matrix);
	CoinFiv.setScale(&scale);
	CoinFiv.calcWorld(false);
	
	CoinTen.setDrawMatrix(matrix);
	CoinTen.setScale(&scale);
	CoinTen.calcWorld(false);
	
	CoinThy.setDrawMatrix(matrix);
	CoinThy.setScale(&scale);
	CoinThy.calcWorld(false);
	
	CoinFifty.setDrawMatrix(matrix);
	CoinFifty.setScale(&scale);
	CoinFifty.calcWorld(false);
	
	CoinFiftyBig.setDrawMatrix(matrix);
	CoinFiftyBig.setScale(&scale);
	CoinFiftyBig.calcWorld(false);
	
	CoinHdrd.setDrawMatrix(matrix);
	CoinHdrd.setScale(&scale);
	CoinHdrd.calcWorld(false);
	
	emptyModel.setDrawMatrix(matrix);
	emptyModel.setScale(&scale);
	emptyModel.calcWorld(false);
}

/*
void EnCoinRoulette_c::bindAnimTexPat_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmTexPat anmA = this->resFile.GetResAnmTexPat(name);
	this->animationPat.bindEntry(&this->CoinFiv, anmA, 0, 0);
	this->CoinFiv.bindAnim(&this->animationPat, unk2);
	this->animationPat.setUpdateRate(rate);
}
*/
void EnCoinRoulette_c::setupModels(){
	
	////From one file
	resFile.data = getResource("block_roulette_coin", "g3d/block_roulette_coin.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("block_roulette_coin5");
	CoinFiv.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinFiv, 0);
	
	nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("block_roulette_coin10");
	CoinTen.setup(mdlA, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinTen, 0);
	
	nw4r::g3d::ResMdl mdlB = this->resFile.GetResMdl("block_roulette_coin30");
	CoinThy.setup(mdlB, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinThy, 0);
	
	nw4r::g3d::ResMdl mdlC = this->resFile.GetResMdl("block_roulette_coin50");
	CoinFifty.setup(mdlC, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinFifty, 0);
	
	nw4r::g3d::ResMdl mdlD = this->resFile.GetResMdl("block_roulette_coin50_Big");
	CoinFiftyBig.setup(mdlD, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinFiftyBig, 0);
	
	nw4r::g3d::ResMdl mdlE = this->resFile.GetResMdl("block_roulette_coin100");
	CoinHdrd.setup(mdlE, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&CoinHdrd, 0);
	
	nw4r::g3d::ResMdl mdlF = this->resFile.GetResMdl("block_roulette_coin_empty");
	emptyModel.setup(mdlF, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&emptyModel, 0);


}

EnCoinRoulette_c* EnCoinRoulette_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(EnCoinRoulette_c));
	return new(buffer) EnCoinRoulette_c;
}
/*
const SpriteData COIN_ROULETTESpriteData = { ProfileId::COIN_ROULETTE, 0, 0, 0, 0, 0x200, 0x200, 0, 0, 0, 0, 0 };

Profile COIN_ROULETTEProfile(&EnCoinRoulette_c::build, SpriteId::COIN_ROULETTE, &COIN_ROULETTESpriteData, ProfileId::COIN_ROULETTE, ProfileId::COIN_ROULETTE, "COIN_ROULETTE", CoinRuletSrcList);
*/
int EnCoinRoulette_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	setupModels();

	allocator.unlink(); 

	whatMdl = 0;
	mdlTimer = 0;
	isBig = settings >> 12 & 0xF; //nybble 9
	coinSpawned = 0;
	isModelEmpty = false;
	hitTimer = 0;
	
	if (isBig) {
		this->scale.x = 4.0; 
		this->scale.y = 4.0; 
		this->scale.z = 4.0;
	} else {
		this->scale.x = 2.0; 
		this->scale.y = 2.0; 
		this->scale.z = 2.0;
	}
	
	blockInit(pos.y);
	
	if (isBig){
		
		physicsInfo.x1 = -32;
		physicsInfo.y1 = 40;
		physicsInfo.x2 = 36;
		physicsInfo.y2 = -59;
		
	} else {
		
		physicsInfo.x1 = -16;
		physicsInfo.y1 = 20;
		physicsInfo.x2 = 16;
		physicsInfo.y2 = -29;
		
	}
	
	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int EnCoinRoulette_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	if (!isModelEmpty) {
		if (whatMdl == 1) {CoinTen._vf1C();}
		else if (whatMdl == 2) {CoinThy._vf1C();}
		else if (whatMdl == 3) {CoinFifty._vf1C();}
		else if (whatMdl == 4) {CoinFiftyBig._vf1C();}
		else if (whatMdl == 5) {CoinHdrd._vf1C();}
		else {CoinFiv._vf1C();}
	}
	else {emptyModel._vf1C();}

	// now check zone bounds based on state
	if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
		checkZoneBoundaries(0);
	}
	
	if (isBig){
		if (mdlTimer % 6 == 0){
			this->whatMdl++;
			if (whatMdl == 3) {PlaySound(this, SE_GAKKI_U_1_ON);}
			else {
				PlaySound(this, SE_OBJ_ROULETTE_BLOCK_ROLL);
				if (whatMdl > 3) {
					this->whatMdl = 0;
					this->mdlTimer = 0;
				}
			}
		}
	} else {
		if (mdlTimer % 6 == 0){
			this->whatMdl++;
			if (whatMdl == 2) {whatMdl = 4;}
			if (whatMdl == 3) {whatMdl = 5;}
			if (whatMdl == 5) {PlaySound(this, SE_GAKKI_U_1_ON);}
			else {
				PlaySound(this, SE_OBJ_ROULETTE_BLOCK_ROLL);
				if (whatMdl > 5) {
					this->whatMdl = 0;
					this->mdlTimer = 0;
				}
			}
		}
	}
	
	this->mdlTimer++;
	
	return true;
}

int EnCoinRoulette_c::onDelete() {
	return true;
}

void EnCoinRoulette_c::blockWasHit(bool isDown) {
	pos.y = initialY;

	PlaySound(this, SE_OBJ_COIN_POP_UP);
	
	//(dStageActor_c*)FindActorByType(AC_WATER_MOVE_REGULAR, 0); //(big coin)
	isModelEmpty = true;
	
	if (!isBig) {
		if (whatMdl == 1){
			
			dStageActor_c *CoinLot = CreateActor(188, 0, this->pos, 0, 0);
			CoinLot->pos.y += 16.0;
			CoinLot->pos.z = 2000.0;
			
		} else if (whatMdl == 2){
			
			tempSet = 0x0100;
			childSettings =
				(tempSet & 3) | ((tempSet & 0xC) << 2) |
				((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
				((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
				((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
			
			dStageActor_c *CoinLot = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot->pos.y += 16.0;
			CoinLot->pos.z = 2000.0;
			

			
		} else if (whatMdl == 3) {
			
			tempSet = 0x0200;
			childSettings =
				(tempSet & 3) | ((tempSet & 0xC) << 2) |
				((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
				((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
				((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
			
			dStageActor_c *CoinLot = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot->pos.y += 16.0;
			CoinLot->pos.z = 2000.0;
			
		} else {
			StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
			StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
			StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
			StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
			StageE4::instance->spawnCoinJump(pos, 0, 2, 0);
		}

		this->scale.x = 2.0; 
		this->scale.y = 2.0; 
		this->scale.z = 2.0;
		
	} else {
		if (whatMdl == 1) {
			
			tempSet = 0x0100;
			childSettings =
				(tempSet & 3) | ((tempSet & 0xC) << 2) |
				((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
				((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
				((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
			
			dStageActor_c *CoinLot = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot->pos.y += 120.0;
			CoinLot->pos.z = 2000.0;
			
		} else if (whatMdl == 4) {
			
			tempSet = 0x0200;
			childSettings =
				(tempSet & 3) | ((tempSet & 0xC) << 2) |
				((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
				((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
				((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
			
			dStageActor_c *CoinLot = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot->pos.y += 120.0;
			CoinLot->pos.z = 2000.0;
			
		} else if (whatMdl == 5){
			
			tempSet = 0x0200;
			childSettings =
				(tempSet & 3) | ((tempSet & 0xC) << 2) |
				((tempSet & 0x30) << 4) | ((tempSet & 0xC0) << 6) |
				((tempSet & 0x300) << 8) | ((tempSet & 0xC00) << 10) |
				((tempSet & 0x3000) << 12) | ((tempSet & 0xC000) << 14);
			
			dStageActor_c *CoinLot0 = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot0->pos.y += 120.0;
			CoinLot0->pos.z = 2000.0;
			CoinLot0->pos.x -= 24.0;
			dStageActor_c *CoinLot1 = CreateActor(188, childSettings, this->pos, 0, 0);
			CoinLot1->pos.y += 120.0;
			CoinLot1->pos.z = 2000.0;
			CoinLot1->pos.x += 24.0;
			
			
		} else {
			dStageActor_c *CoinLot = CreateActor(188, 0, this->pos, 0, 0);
			CoinLot->pos.y += 96.0;
			CoinLot->pos.z = 2000.0;
		}
		
		this->scale.x = 4.0; 
		this->scale.y = 4.0; 
		this->scale.z = 4.0;
		
	}
	
	doStateChange(&StateID_Wait);
}

void EnCoinRoulette_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void EnCoinRoulette_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}

int EnCoinRoulette_c::onDraw() {
	if (!isModelEmpty) {
		if (whatMdl == 1){CoinTen.scheduleForDrawing();}
		else if (whatMdl == 2){CoinThy.scheduleForDrawing();}
		else if (whatMdl == 3){CoinFifty.scheduleForDrawing();}
		else if (whatMdl == 4){CoinFiftyBig.scheduleForDrawing();}
		else if (whatMdl == 5){CoinHdrd.scheduleForDrawing();}
		else {CoinFiv.scheduleForDrawing();}
	}
	else {emptyModel.scheduleForDrawing();}

	return true;
}

void EnCoinRoulette_c::beginState_Wait() {}
void EnCoinRoulette_c::executeState_Wait() {
	result = blockResult();

	if (result == 0)
		return;

	if (result == 1) {
		if (hitTimer < 1) {
			doStateChange(&daEnBlockMain_c::StateID_UpMove);
			anotherFlag = 2;
			isGroundPound = false;
		}
		this->hitTimer++;
	} else {
		if (hitTimer < 1) {
			doStateChange(&daEnBlockMain_c::StateID_DownMove);
			anotherFlag = 1;
			isGroundPound = true;
		}
		this->hitTimer++;
	}
}
void EnCoinRoulette_c::endState_Wait() {}