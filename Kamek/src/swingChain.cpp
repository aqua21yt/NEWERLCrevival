#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>


const char* BallsResourceList [] = { "guru", NULL };

class dSwingHangBall_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void setupModels();
	
	bool isAxe;

	static dSwingHangBall_c* build();

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
	m3d::mdl_c axeModel;
	m3d::anmChr_c animationChr;
	m3d::anmChr_c animationChrA;
	nw4r::g3d::ResFile resFile;

};


void dSwingHangBall_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
	
	axeModel.setDrawMatrix(matrix);
	axeModel.setScale(&scale);
	axeModel.calcWorld(false);
}


void dSwingHangBall_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

void dSwingHangBall_c::bindAnimChrA_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChrA = this->resFile.GetResAnmChr(name);
	this->animationChrA.bind(&this->axeModel, anmChrA, unk);
	this->axeModel.bindAnim(&this->animationChrA, unk2);
	this->animationChrA.setUpdateRate(rate);
}

void dSwingHangBall_c::setupModels(){
	
	resFile.data = getResource("guru", "g3d/guru.brres");
	
	if (isAxe) {
		
		nw4r::g3d::ResMdl mdlA = this->resFile.GetResMdl("axe");
		axeModel.setup(mdlA, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&axeModel, 0);
		
		nw4r::g3d::ResAnmChr anmChrA = this->resFile.GetResAnmChr("rotate");
		this->animationChrA.setup(mdlA, anmChrA, &this->allocator, 0);
		
	} else {
		
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("guru");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Enemy(&bodyModel, 0);
		
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("rotate");
		this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
		
	}
	
}

dSwingHangBall_c* dSwingHangBall_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dSwingHangBall_c));
	return new(buffer) dSwingHangBall_c;
}


int dSwingHangBall_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	isAxe = settings >> 12 & 0xF; //nybble 9
	setupModels();

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;


	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = -40.0; 
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

	if (isAxe) {bindAnimChrA_and_setUpdateRate("rotate", 1, 0.0, 1.0);}
	else {bindAnimChr_and_setUpdateRate("rotate", 1, 0.0, 1.0);}
	
	this->rot.z += 0x900;

	this->onExecute();

	return true;
}

int dSwingHangBall_c::onExecute() {
	updateModelMatrices();
	if (isAxe) {axeModel._vf1C();}
	else {bodyModel._vf1C();}

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int dSwingHangBall_c::onDelete() {
	return true;
}

int dSwingHangBall_c::onDraw() {
	if (isAxe) {axeModel.scheduleForDrawing();}
	else {bodyModel.scheduleForDrawing();}

	return true;
}