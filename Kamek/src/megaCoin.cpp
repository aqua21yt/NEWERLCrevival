#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"
#include <profile.h>


const char* megaCoinSarcList [] = { "obj_coin_big", NULL };

class daBigCoin_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	int coinValue;
	bool isCaught;
	int timer;
	int collTimer;

	static dActor_c* build();

	void powBlockActivated(bool isNotMPGP);
	
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
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

};


void daBigCoin_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

dActor_c* daBigCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daBigCoin_c));
	return new(buffer) daBigCoin_c;
}

const SpriteData COIN_LOTSpriteData = { ProfileId::COIN_LOT, 0, 0, 0, 0, 0x200, 0x200, 0, 0, 0, 0, 0 };

Profile COIN_LOTProfile(&daBigCoin_c::build, SpriteId::COIN_LOT, &COIN_LOTSpriteData, ProfileId::COIN_LOT, ProfileId::COIN_LOT, "COIN_LOT", megaCoinSarcList);

void daBigCoin_c::powBlockActivated(bool isNotMPGP) {
	this->collMgr.calculateBelowCollisionWithSmokeEffect();
	if (collMgr.isOnTopOfTile()) {this->pos.y -= 7.0;}
}

void daBigCoin_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther){	
	if (apOther->owner->name == PLAYER){
		
		dAcPy_c* player = (dAcPy_c*)apOther->owner;
		
		if (this->collTimer == 0) {
			
			PlaySound(this, SE_OBJ_GET_DRAGON_COIN);
			
			Vec ef2Pos = {pos.x + 7.0f, pos.y + 14.0f, pos.z + 100.0f};
			S16Vec null2Rot = {0,0,0};
			Vec ef2Scale = {0.5f, 0.5f, 0.5f};
			SpawnEffect("Wm_ob_keyget01", 0, &ef2Pos, &null2Rot, &ef2Scale);
			
			isCaught = true;
			aPhysics.removeFromList();
			
		}
		
		if (this->collTimer < 40) {
			
			if (collTimer < 20) {
				this->pos.y += 0.1;
			} else {
				this->pos.y -= 0.1;
			}
			
		}
		
		if (this->collTimer == 10) {
			
			Vec ef3Pos = {pos.x + 4.0f, pos.y + 7.0f, pos.z + 300.0f};
			S16Vec null3Rot = {0,0,0};
			Vec ef3Scale = {0.8f, 0.8f, 0.8f};
			SpawnEffect("Wm_ob_keyget01_a", 0, &ef3Pos, &null3Rot, &ef3Scale);
			
		}
		
		if (this->collTimer == 20) {
			
			Vec ef3Pos = {pos.x + 2.0f, pos.y + 4.5f, pos.z + 500.0f};
			S16Vec null3Rot = {0,0,0};
			Vec ef3Scale = {1.1f, 1.1f, 1.1f};
			SpawnEffect("Wm_ob_keyget01_b", 0, &ef3Pos, &null3Rot, &ef3Scale);
			
		}
		
		if (this->collTimer == 30) {
			
			Vec ef4Pos = {pos.x + 1.0f, pos.y + 2.0f, pos.z + 300.0f};
			S16Vec null4Rot = {0,0,0};
			Vec ef4Scale = {0.8f, 0.8f, 0.8f};
			SpawnEffect("Wm_ob_keyget01_c", 0, &ef4Pos, &null4Rot, &ef4Scale);
			
		}
		
		if (this->collTimer == 40) {
			
			Vec ef5Pos = {pos.x + 0.0f, pos.y + 0.0f, pos.z - 0.0f};
			S16Vec null5Rot = {0,0,0};
			Vec ef5Scale = {0.5f, 0.5f, 0.5f};
			SpawnEffect("Wm_ob_keyget01_d", 0, &ef5Pos, &null5Rot, &ef5Scale);
			
		}
		

		
	}
	
	if (this->collTimer == 0) {
		if (coinValue == 1){
			StageE4::instance->spawnCoinJump(this->pos, 0, 30, 0);
		} else if (coinValue == 2){
			StageE4::instance->spawnCoinJump(this->pos, 0, 50, 0);
		} else {
			StageE4::instance->spawnCoinJump(this->pos, 0, 10, 0);
			
			//Wondering why didn't I use a loop just like other source codes? Because that one won't work. Bruh!//
		}
	}
	
	if (this->collTimer == 101){
		
		Vec ef6Pos = {pos.x + 0.0f, pos.y + 0.0f, pos.z - 0.0f};
		S16Vec null6Rot = {0,0,0};
		Vec ef6Scale = {0.0f, 0.0f, 0.0f};
		SpawnEffect("Wm_ob_keyget01_d", 0, &ef6Pos, &null6Rot, &ef6Scale);
		this->Delete(1);
		
	}
	
	this->collTimer++;
	
	/*
	if (NewCoin[collTimer]->pos.x != player->pos.x && NewCoin[collTimer]->pos.y != player->pos.y){
		daBigCoin_c *NewCoin[collTimer]->pos.x = player->pos.x;
		daBigCoin_c *NewCoin[collTimer]->pos.y = player->pos.y;
	}
	*/
	
}

void daBigCoin_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther){
}

void daBigCoin_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}


bool daBigCoin_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daBigCoin_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daBigCoin_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daBigCoin_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther){
	this->collisionCat7_GroundPoundYoshi(apThis, apOther);
}

bool daBigCoin_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

bool daBigCoin_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther){
	this->playerCollision(apThis, apOther);
}

bool daBigCoin_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daBigCoin_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daBigCoin_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}

int daBigCoin_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	//VARIABLES:
	coinValue = settings >> 16 & 0xF; //nybble 8
	this->isCaught = false;
	this->timer = 0;
	collTimer = 0;
	
	resFile.data = getResource("obj_coin_big", "g3d/obj_coin_big.brres");
	
	if (coinValue == 1) {
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Coin30");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Item(&bodyModel, 0);
	}
	else if (coinValue == 2) {
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Coin50");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Item(&bodyModel, 0);
	}
	else {
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("Coin10");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Item(&bodyModel, 0);
	}
	


	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info LotPcePhysc; 
	LotPcePhysc.xDistToCenter = 0.0; 
	LotPcePhysc.yDistToCenter = 0.0; 
	LotPcePhysc.xDistToEdge = 18.0; 
	LotPcePhysc.yDistToEdge = 24.0; 
	LotPcePhysc.category1 = 0x5; 
	LotPcePhysc.category2 = 0x0; 
	LotPcePhysc.bitfield1 = 0x4F; 
	LotPcePhysc.bitfield2 = 0xFFFFFFFF; 
	LotPcePhysc.unkShort1C = 0; 
	LotPcePhysc.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &LotPcePhysc); 
	this->aPhysics.addToList();



	this->onExecute();

	return true;
}

int daBigCoin_c::onExecute() {
	updateModelMatrices();
	if (!isCaught) {bodyModel._vf1C();}

	this->rot.y += 0x200;
	
	if ((this->timer == 1) && (!isCaught)) {
		
		Vec efPos = {pos.x + 7.0f, pos.y + 14.0f, pos.z - 5500.0f};
		S16Vec nullRot = {0,0,0};
		Vec efScale = {2.8f, 2.8f, 1.0f};
		SpawnEffect("Wm_ob_redcoinkira", 0, &efPos, &nullRot, &efScale);
		
	}
	
	this->timer++;
	
	return true;
}

int daBigCoin_c::onDelete() {
	return true;
}

int daBigCoin_c::onDraw() {
	if (!isCaught) {bodyModel.scheduleForDrawing();}

	return true;
}