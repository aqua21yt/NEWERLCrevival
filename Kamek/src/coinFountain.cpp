#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"

class dCoinFountain_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	char coinsPerMatch;
	char matchAmount;
	int timer;
	char EventID;
	int spawnDirection;
	float moveX;
	//float coinOffset;

	static dCoinFountain_c* build();

	USING_STATES(dCoinFountain_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Spawn);
};

CREATE_STATE(dCoinFountain_c, Wait);
CREATE_STATE(dCoinFountain_c, Spawn);

dCoinFountain_c* dCoinFountain_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dCoinFountain_c));
	return new(buffer) dCoinFountain_c;
}


int dCoinFountain_c::onCreate() {

	coinsPerMatch = settings >> 16 & 0xF; //nybble 8
	matchAmount = settings >> 12 & 0xF; //nybble 9
	EventID = settings >> 4 & 0xFF; //nybble 10-11
	spawnDirection = settings & 0xF; //nybble 12
	moveX = 3.5;
	
	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int dCoinFountain_c::onExecute() {
	acState.execute();

	return true;
}

int dCoinFountain_c::onDelete() {
	return true;
}

int dCoinFountain_c::onDraw() {
	return true;
}

void dCoinFountain_c::beginState_Wait() {
	this->timer = 0;
}
void dCoinFountain_c::executeState_Wait() {
	if ((this->timer > 0) && (dFlagMgr_c::instance->active(EventID))) {
		doStateChange(&StateID_Spawn);
	}
	this->timer++;
}
void dCoinFountain_c::endState_Wait() {}

void dCoinFountain_c::beginState_Spawn() {
	this->timer = 0;
}
void dCoinFountain_c::executeState_Spawn() {
	if (this->timer / 3 < coinsPerMatch * matchAmount){
		if (this->timer % 3 == 0) {
			dStageActor_c *flyCoin = dStageActor_c::create(EN_COIN, 0x3, &pos, 0, 0);
			Vec ef3Pos = {this->pos.x, this->pos.y - 16.0f, 0};
			S16Vec null3Rot = {0,0,0};
			Vec one3Vec = {2.0f, 0.8f, 1.0f};
			SpawnEffect("Wm_2d_coinlight", 0, &ef3Pos, &null3Rot, &one3Vec);
			if (this->timer < 60) {
				if (this->timer == 0) {
					flyCoin->coinOffset = ((GenerateRandomNumber(200)) - 100) / 100;
				}
				if (spawnDirection == 1) { // right
					flyCoin->pos.x += moveX;
					if (this->timer < 30) {
						flyCoin->pos.y += flyCoin->coinOffset;
					}
				} else if (spawnDirection == 2) { //up
					if (this->timer < 30) {
						flyCoin->pos.y += moveX;
						flyCoin->pos.x += flyCoin->coinOffset;
					}
				} else if (direction == 3) { //down
					flyCoin->pos.x += flyCoin->coinOffset;
				} else { //0 = left
					flyCoin->pos.x -= moveX;
					if (this->timer < 30) {
						flyCoin->pos.y += flyCoin->coinOffset;
					}
				}
			}
		}
		
	}
	
	this->timer++;
}
void dCoinFountain_c::endState_Spawn() {}