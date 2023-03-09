#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
//#include "boss.h"
#include "path.h"


const char* BeanStalksSARCList [] = { "lift_han_leaf", NULL };

class daLeafBean_c : public dEnPath_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static daLeafBean_c* build();
	
	char eventID;
	int timer;

	//void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	//void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

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

	USING_STATES(daLeafBean_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Grow);
	DECLARE_STATE(LeafBear);
	DECLARE_STATE(LeafDie);
	DECLARE_STATE(Stop);
};

static nw4r::snd::StrmSoundHandle beanHANDLE;
extern "C" void PlaySoundWithFunctionB4(void *spc, nw4r::snd::SoundHandle *handle, int id, int unk);

CREATE_STATE(daLeafBean_c, Wait);
CREATE_STATE(daLeafBean_c, Grow);
CREATE_STATE(daLeafBean_c, LeafBear);
CREATE_STATE(daLeafBean_c, LeafDie);
CREATE_STATE(daLeafBean_c, Stop);

void daLeafBean_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void daLeafBean_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

daLeafBean_c* daLeafBean_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daLeafBean_c));
	return new(buffer) daLeafBean_c;
}


int daLeafBean_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	eventID = settings >> 12 & 0xFF; //nybbles 8-9
	
	this->timer = 0;
	
	resFile.data = getResource("lift_han_leaf", "g3d/obj_tsuru.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("obj_tsuru");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_MapObj(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("tsuru_start");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	this->disableEatIn();
	
	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info BeanPhysics; 
	BeanPhysics.xDistToCenter = 0.0; 
	BeanPhysics.yDistToCenter = 0.0; 
	BeanPhysics.xDistToEdge = 0.0; 
	BeanPhysics.yDistToEdge = 0.0; 
	BeanPhysics.category1 = 0x3; 
	BeanPhysics.category2 = 0x0; 
	BeanPhysics.bitfield1 = 0x4F; 
	BeanPhysics.bitfield2 = 0xFFFFFFFF; 
	BeanPhysics.unkShort1C = 0; 
	BeanPhysics.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &BeanPhysics); 
	this->aPhysics.addToList();

	//bindAnimChr_and_setUpdateRate("spawn", 1, 0.0, 1.0);

	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int daLeafBean_c::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int daLeafBean_c::onDelete() {
	return true;
}

int daLeafBean_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daLeafBean_c::beginState_Wait() {
	this->timer = 0;
}
void daLeafBean_c::executeState_Wait() {
	if ((this->timer > 0) && (dFlagMgr_c::instance->active(eventID))) {
		PlaySoundWithFunctionB4(SoundRelatedClass, &beanHANDLE, EN_ITEM_IVY, 1);
		doStateChange(&StateID_Grow);
	}
	OSReport("I might be considered as the best NSMBW modder if I succeed in programming that complex sprite :D");
	this->timer++;
}
void daLeafBean_c::endState_Wait() {}

void daLeafBean_c::beginState_Grow() {
	bindAnimChr_and_setUpdateRate("tsuru_start", 1, 0.0, 1.0);
	doStateChange(&StateID_FollowPath);
	this->timer = 0;
}
void daLeafBean_c::executeState_Grow() {
	this->timer++;
}
void daLeafBean_c::endState_Grow() {}

void daLeafBean_c::beginState_LeafBear() {}
void daLeafBean_c::executeState_LeafBear() {}
void daLeafBean_c::endState_LeafBear() {}

void daLeafBean_c::beginState_LeafDie() {}
void daLeafBean_c::executeState_LeafDie() {}
void daLeafBean_c::endState_LeafDie() {}

void daLeafBean_c::beginState_Stop() {}
void daLeafBean_c::executeState_Stop() {}
void daLeafBean_c::endState_Stop() {}