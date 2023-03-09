#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
//#include <profile.h>


const char* FirebarssArcFilesList [] = { "center_firebar", NULL };

class daEnNewBar_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void setupRoundabout();
	
	int armAmount;
	int ballAmount;
	int timer = 0;

	static dActor_c* build();

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

};


void daEnNewBar_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daEnNewBar_c::setupRoundabout() {
	this->timer++;
}

dActor_c* daEnNewBar_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEnNewBar_c));
	return new(buffer) daEnNewBar_c;
}

/*
const SpriteData BETTER_FIREBARSpriteData = { ProfileId::BETTER_FIREBAR, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile BETTER_FIREBARProfile(&daEnNewBar_c::build, SpriteId::BETTER_FIREBAR, &BETTER_FIREBARSpriteData, ProfileId::BETTER_FIREBAR, ProfileId::BETTER_FIREBAR, "BETTER_FIREBAR", FirebarssArcFilesList);
*/

int daEnNewBar_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	resFile.data = getResource("center_firebar", "g3d/center_firebar.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("center_firebar");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);

	armAmount = settings >> 4 & 0xF; //nybble 11
	ballAmount = settings & 0xF; //nybble 12
	
	setupRoundabout();
	
	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 0.0; 
	HitMeBaby.xDistToEdge = 8.0; 
	HitMeBaby.yDistToEdge = 12.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	PlaySound(this, SE_EMY_FIRE_SNAKE_BIGGER);


	this->onExecute();

	return true;
}

int daEnNewBar_c::onExecute() {
	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}

int daEnNewBar_c::onDelete() {
	return true;
}

int daEnNewBar_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}