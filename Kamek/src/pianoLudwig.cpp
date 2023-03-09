#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include "boss.h"


const char* PianoActorsFileList [] = { "ludwig_piano", NULL };

class daPianoLudwig : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();

	static daPianoLudwig* build();

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

	USING_STATES(daPianoLudwig);
	DECLARE_STATE(Wait);
};

CREATE_STATE(daPianoLudwig, Wait);

void daPianoLudwig::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

daPianoLudwig* daPianoLudwig::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daPianoLudwig));
	return new(buffer) daPianoLudwig;
}


int daPianoLudwig::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("ludwig_piano", "g3d/ludwig_piano.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("ludwig");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Player(&bodyModel, 0);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 0.0; 
	HitMeBaby.xDistToEdge = 16.0; 
	HitMeBaby.yDistToEdge = 24.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int daPianoLudwig::onExecute() {
	acState.execute();

	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daPianoLudwig::onDelete() {
	return true;
}

int daPianoLudwig::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daPianoLudwig::beginState_Wait() {
	
	
}
void daPianoLudwig::executeState_Wait() {}
void daPianoLudwig::endState_Wait() {}