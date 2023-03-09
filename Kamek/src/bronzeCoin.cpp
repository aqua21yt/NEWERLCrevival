#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>


extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

const char* BronzeCoinFileList [] = { "obj_coin", NULL };

extern int RedCoinsCollected;
extern int caughtRedCoins;
extern int GlobalRedCoinsToCatch;

class dEventCoin_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;

	mEf::es2 effect;

	u64 eventFlag;
	s32 timer;
	bool collected;
	int efTimer;

	static dEventCoin_c *build();

	void updateModelMatrices();
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);

};


void dEventCoin_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

	if (collected) {
		this->Delete(1);
		return; }


	PlaySound(this, SE_OBJ_GET_COIN_OLD);
	S16Vec nullRot = {0,0,0};
	Vec efScale = {0.8f, 0.8f, 0.8f};
	SpawnEffect("Wm_ob_redcoinkira", 0, &this->pos, &nullRot, &efScale);

	RedCoinsCollected--;
	caughtRedCoins++;
	if (RedCoinsCollected == 0) {
		dFlagMgr_c::instance->flags |= this->eventFlag;
	}

	collected = true;

	this->Delete(1);
}

void dEventCoin_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool dEventCoin_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dEventCoin_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dEventCoin_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dEventCoin_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}
bool dEventCoin_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
	return true;
}


dEventCoin_c *dEventCoin_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dEventCoin_c));
	return new(buffer) dEventCoin_c;
}


int dEventCoin_c::onCreate() {

	collected = false;
	char die = this->settings & 0xF;
	if (GetSpecificPlayerActor(die) == 0) { this->Delete(1); return 2; }

	RedCoinsCollected++;
	GlobalRedCoinsToCatch++;

	allocator.link(-1, GameHeaps[0], 0, 0x20);

	nw4r::g3d::ResFile rf(getResource("obj_coin", "g3d/obj_coin.brres"));
	bodyModel.setup(rf.GetResMdl("obj_coin_bronze"), &allocator, 0x224, 1, 0);
	SetupTextures_Map(&bodyModel, 0);

	allocator.unlink();

	ActivePhysics::Info BCoinBox;
	BCoinBox.xDistToCenter = 0.0;
	BCoinBox.yDistToCenter = 3.0;
	BCoinBox.xDistToEdge = 6.0;
	BCoinBox.yDistToEdge = 6.0;
	BCoinBox.category1 = 0x5;
	BCoinBox.category2 = 0x0;
	BCoinBox.bitfield1 = 0x4F;
	BCoinBox.bitfield2 = 0x200;
	BCoinBox.unkShort1C = 0;
	BCoinBox.callback = &dEn_c::collisionCallback;

	this->aPhysics.initWithStruct(this, &BCoinBox);
	this->aPhysics.addToList();

	char eventNum	= (this->settings >> 24)	& 0xFF; //nybbles 5-6
	this->eventFlag = (u64)1 << (eventNum - 1);

	this->scale.x = 1.0;
	this->scale.y = 1.0;
	this->scale.z = 1.0;
	
	this->pos.x += 8.0;
	this->pos.y -= 8.0;

	this->onExecute();
	return true;
	
}


int dEventCoin_c::onDelete() {
	return true;
}

int dEventCoin_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}


void dEventCoin_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

int dEventCoin_c::onExecute() {
	updateModelMatrices();

	if ((this->efTimer == 30) && (!collected)){
		S16Vec nullRot = {0,0,0};
		Vec efScale = {1.2f, 1.2f, 1.2f};
		effect.spawn("Wm_ob_redcoinkira", 0, &this->pos, &nullRot, &efScale);
		
		this->efTimer = 0;
	}
	
	this->rot.y += 0x200;
	
	this->efTimer++;
	
	return true;
}

